
#include "BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/BlasterComponent/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//就是我们移动鼠标视角左右转时Player小绿人不能也跟着转
	bUseControllerRotationYaw = false;
	//现在小绿人转弯靠的是按键按下出来的加速度,
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	//创建个组件绑定在主角身上
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));//?????????????别的代码显示的是Combat
	Combat->SetIsReplicated(true);//注意了,这个是将combat组件里的Component Replicates设为true,但不知道为什么不起作用并没有变为true
	Combat->SetIsReplicated(true);//不变为true就会使服务端的东西复制不去客户端
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	//把摄像头方面的弹簧臂不会被其他player的实体受影响,没改之前其他人跑到自己的后面会卡的弹簧臂一伸一缩
	//这是把BP_BlasterCharacter里的Mesh组件的Detail的Collision Presets[CharacterMesh]的Camera的影响为ignore
	//碰撞体和网格mesh都要对摄像机的弹簧臂忽视
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;//一开始就不设置转向
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
}
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

//每帧都调用里面的方法
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);//计算鼠标偏移
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
}
void ABlasterCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}
void ABlasterCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}
void ABlasterCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}
void ABlasterCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}
void ABlasterCharacter::EquipButtonPressed()
{
	if (Combat)
	{
		//判定当前是否运行在Authority（授权）服务器上
		if (HasAuthority())//若主角是服务器则直接装备武器
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else//若主角是客户端需要呼叫服务端自己要装备武器了,服务端发送同意指令后才给她装备
		{
			ServerEquipButtonPressed();//???????????????
		}
	}
}
void ABlasterCharacter::ServerEquipButtonPressed_Implementation()//???????????????
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}
void ABlasterCharacter::CrouchButtonPressed()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ABlasterCharacter::AimButtonPressed()
{//一按右键瞄准就先触发战斗代码的瞄准状态
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{//一释放右键瞄准就先改变战斗代码的瞄准状态
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

//这个方法就是想让静止不动的人物的上半身和枪随着鼠标的移动跟着走.计算鼠标偏移,每帧都调用这方法
void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;//没有武器直接跳出方法
	//这个是在人物静止时影响人物,运动和在空中不应该用这个,所以我们需要知道我的角色和速度,
	//返回根组件的速度(cm/s),这是一个向量来的,把这个速度向量在Z方向的速度归0
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();//xy轴速度,平移速度
	bool bIsInAir = GetCharacterMovement()->IsFalling();//是否在空中

	if (Speed == 0.f && !bIsInAir) {//静止且在空中
		FRotator CurrentAimRotator = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);//已经停止时Yaw轴方面的旋转(左右旋转)
		//从快停下到停下持枪Yaw轴的缓慢移动变化,(后面发现鼠标左移枪也左移,方向反了,改一下)
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotator, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		bUseControllerRotationYaw = false;//就是静止时我们移动鼠标视角左右转时Player小绿人不能也跟着转
		TurnInPlace(DeltaTime);//原地转弯
	}
	//需要存储跑步时或在空中的值
	if (Speed > 0.f || bIsInAir) {
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);//快落地时或快停下时Yaw轴方面的旋转(左右旋转)
		AO_Yaw = 0.f;//当我们移动的时候要时刻保持Yaw为0
		bUseControllerRotationYaw = true;//就是运动时我们移动鼠标视角左右转时Player小绿人也要也跟着转
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;//在空中时不设置转向
	}

	//Pitch就不用这么麻烦,
	AO_Pitch = GetBaseAimRotation().Pitch;
	//90还不明白不过平行线到头顶时360->270符合>90条件,
	//后面那个就是只需要改变其他吃瓜的客户端和服务端的Pitch值就好了,就可以看到正确的抬枪
	if (AO_Pitch > 90.f && !IsLocallyControlled()) {
		//将Pitch在[270,360)转换为[-90,0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::TurnInPlace(float DeltaTime) {
	//那什么时候原地转身呢?当Yaw>90°或<-90°时
	//UE_LOG(LogTemp, Warning, TEXT("AO_Yaw: %f"), AO_Yaw);
	if (AO_Yaw > 90.f) {
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f) {
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
}

//只有本地控制的角色重叠武器后会显示E-pickup字
void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{//Overlapping =重叠
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())//只有本地控制的角色重叠武器后会显示E-pickup字
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

//复制变量时会自动调用它,他是OverlappingWeapon的属性里的东西,有关AWeapon的指针的
void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)//?????????????
{//Rep=代表
	//只要服务端发生变化新值就会赋值给客户端
	//个人猜测这个方法是多个武器重叠在一起时显示哪个E-pickup,我觉得显示最开始那个武器E-pickup,
	//E-pickup会在最开始那个武器头上,但你如果按E会捡起最新的那个武器
	if (OverlappingWeapon)
	{//
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

bool ABlasterCharacter::IsWeaponEquipped()//是否已经装备了武器
{
	return (Combat && Combat->EquippedWeapon);
}

//ABlasterCharacter就是类来的,代表IsAiming()是在ABlasterCharacter这个类里
bool ABlasterCharacter::IsAiming()
{//是否已经在瞄准,这要获取CombatComponent战斗代码的瞄准状态
	return (Combat && Combat->bAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapon()
{
	if(Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}


























//#include "BlasterCharacter.h"
//#include "GameFramework/SpringArmComponent.h"
//#include "Camera/CameraComponent.h"
//#include "GameFramework/CharacterMovementComponent.h"
//#include "Components/WidgetComponent.h"
//#include "Net/UnrealNetwork.h"
//#include "Blaster/Weapon/Weapon.h"
//#include "Blaster/BlasterComponent/CombatComponent.h"
//
//ABlasterCharacter::ABlasterCharacter()
//{
// 	
//	PrimaryActorTick.bCanEverTick = true;
//	//创建默认摄像头弹簧臂
//	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
//	//摄像头弹簧臂建立联系
//	CameraBoom->SetupAttachment(GetMesh());
//	//摄像头弹簧臂目标长度
//	CameraBoom->TargetArmLength = 600.0f;
//	//摄像头弹簧臂是否可以旋转
//	CameraBoom->bUsePawnControlRotation = true;
//
//	//创建默认摄像头
//	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
//	//摄像头与弹簧臂建立联系
//	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
//	//摄像头不需要旋转,弹簧臂旋转就好了
//	FollowCamera->bUsePawnControlRotation = false;
//	
//	//就是我们移动鼠标视角左右转时Player小绿人不能也跟着转
//	bUseControllerRotationYaw = false;
//	//现在小绿人转弯靠的是按键按下出来的加速度,
//	GetCharacterMovement()->bOrientRotationToMovement = true;
//
//	//创建UWidgetComponent部件叫OverheadWidget,并将其绑定在第二级
//	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
//	OverheadWidget->SetupAttachment(RootComponent);//将其绑定在第二级
//
//	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
//	Combat->SetIsReplicated(true);//????????????????
//
//	//将Charater的NavMovement的CanCrouch设为true
//	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
//}
//
////??????????????????????????我怀疑这是个角色身上的探针,刺探角色是否与武器重叠
//void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	//???????????将刺探到的消息复制给操控自己角色的客户端
//	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);//?????????????
//}
//
//void ABlasterCharacter::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}
//
////Tick我猜测是角色身上的触发器
//void ABlasterCharacter::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//
//void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//	//??????????????????????????
//	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
//	//给this(Player)绑定映射,将MoveForward()方法绑到按键(MoveForward)上,
//	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
//	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
//	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
//	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);
//
//	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipButtonPressed);//按下E键就装备武器
//	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);//按下shift键就蹲下
//	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABlasterCharacter::AimButtonPressed);
//	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABlasterCharacter::AimButtonReleased);
//}
//
//void ABlasterCharacter::PostInitializeComponents()//????????????????
//{
//	Super::PostInitializeComponents();
//	if (Combat) {
//		Combat->Character = this;
//	}
//}
//
///// <summary>
///// 前后移动
///// </summary>
///// <param name="Value"></param>
//void ABlasterCharacter::MoveForward(float Value)
//{
//	//
//	if (Controller != nullptr && Value != 0.f) {
//		//通俗点:
//		//Yaw就是绕着Z轴旋转,Roll围绕X轴旋转,Pitch围绕Y轴旋转
//		//这个就是获取Player面向哪个角度
//		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
//		//这个就是将Player正面向的角度(X)做成向量,后面那个X是Player正面对的方向
//		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
//		//这个就是Player往向量方向移动
//		AddMovementInput(Direction, Value);
//	}
//}
//
///// <summary>
///// 左右移动
///// </summary>
///// <param name="Value"></param>
//void ABlasterCharacter::MoveRight(float Value)
//{
//	if (Controller != nullptr && Value != 0.f) {
//	//这个就是获取Player面向哪个角度
//	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
//	//这个就是将Player侧面(Y)的角度做成向量,后面那个Y是Player侧面的方向
//	const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
//	//这个就是Player往向量方向移动
//	AddMovementInput(Direction, Value);
//	}
//}
//
///// <summary>
///// 左右看
///// </summary>
///// <param name="Value"></param>
//void ABlasterCharacter::Turn(float Value)
//{
//	AddControllerYawInput(Value);
//}
//
///// <summary>
///// 上下看
///// </summary>
///// <param name="Value"></param>
//void ABlasterCharacter::LookUp(float Value)
//{
//	AddControllerPitchInput(Value);
//}
//
///// <summary>
///// ??????????????????装备武器
///// </summary>
//void ABlasterCharacter::EquipButtonPressed()
//{
//	if (Combat) {
//		if (HasAuthority()) {
//			Combat->EquipWeapon(OverlappingWeapon);//装备触发器中的武器
//		}
//		else {
//			ServerEquipButtonPressed();
//		}
//	}
//
//
//}
//
//void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)//?????????????
//{
//	if (OverlappingWeapon) {
//		OverlappingWeapon->ShowPickupWidget(true);
//	}
//	if (LastWeapon) {
//		LastWeapon->ShowPickupWidget(false);//?????????????
//	}
//}
//
//void ABlasterCharacter::ServerEquipButtonPressed_Implementation()//???????????
//{
//	if (Combat) {
//		Combat->EquipWeapon(OverlappingWeapon);//装备触发器中的武器
//	}
//}
//
//void ABlasterCharacter::CrouchButtonPressed() {
//	if (bIsCrouched) {
//		UnCrouch();
//	}
//	else {
//		Crouch();
//	}
//	
//}
//
//void ABlasterCharacter::AimButtonPressed()
//{
//	if (Combat) {
//		Combat->SetAiming(true);
//	}
//}
//
//void ABlasterCharacter::AimButtonReleased()
//{
//	if (Combat) {
//		Combat->SetAiming(false);
//	}
//}
//
////????????????????????????
//void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
//{
//	OverlappingWeapon = Weapon;
//	if (IsLocallyControlled()) {//如果是本地操控player
//		if (OverlappingWeapon) {
//			OverlappingWeapon->ShowPickupWidget(true);
//		}
//	}
//}
//
////是否是已被装备武器
//bool ABlasterCharacter::IsWeaponEquipped()
//{
//	return (Combat && Combat->EquippedWeapon);
//}
//
////Player是否在瞄准
//bool ABlasterCharacter::IsAiming()
//{
//	return (Combat && Combat->bAiming);
//}
//
//

