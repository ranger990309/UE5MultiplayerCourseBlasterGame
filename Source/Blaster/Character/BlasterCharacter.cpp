
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

	//���������ƶ�����ӽ�����תʱPlayerС���˲���Ҳ����ת
	bUseControllerRotationYaw = false;
	//����С����ת�俿���ǰ������³����ļ��ٶ�,
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	//���������������������
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));//?????????????��Ĵ�����ʾ����Combat
	Combat->SetIsReplicated(true);//ע����,����ǽ�combat������Component Replicates��Ϊtrue,����֪��Ϊʲô�������ò�û�б�Ϊtrue
	Combat->SetIsReplicated(true);//����Ϊtrue�ͻ�ʹ����˵Ķ������Ʋ�ȥ�ͻ���
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	//������ͷ����ĵ��ɱ۲��ᱻ����player��ʵ����Ӱ��,û��֮ǰ�������ܵ��Լ��ĺ���Ῠ�ĵ��ɱ�һ��һ��
	//���ǰ�BP_BlasterCharacter���Mesh�����Detail��Collision Presets[CharacterMesh]��Camera��Ӱ��Ϊignore
	//��ײ�������mesh��Ҫ��������ĵ��ɱۺ���
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;//һ��ʼ�Ͳ�����ת��
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

//ÿ֡����������ķ���
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);//�������ƫ��
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
		//�ж���ǰ�Ƿ�������Authority����Ȩ����������
		if (HasAuthority())//�������Ƿ�������ֱ��װ������
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else//�������ǿͻ�����Ҫ���з�����Լ�Ҫװ��������,����˷���ͬ��ָ���Ÿ���װ��
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
{//һ���Ҽ���׼���ȴ���ս���������׼״̬
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{//һ�ͷ��Ҽ���׼���ȸı�ս���������׼״̬
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

//��������������þ�ֹ������������ϰ����ǹ���������ƶ�������.�������ƫ��,ÿ֡�������ⷽ��
void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;//û������ֱ����������
	//����������ﾲֹʱӰ������,�˶����ڿ��в�Ӧ�������,����������Ҫ֪���ҵĽ�ɫ���ٶ�,
	//���ظ�������ٶ�(cm/s),����һ����������,������ٶ�������Z������ٶȹ�0
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();//xy���ٶ�,ƽ���ٶ�
	bool bIsInAir = GetCharacterMovement()->IsFalling();//�Ƿ��ڿ���

	if (Speed == 0.f && !bIsInAir) {//��ֹ���ڿ���
		FRotator CurrentAimRotator = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);//�Ѿ�ֹͣʱYaw�᷽�����ת(������ת)
		//�ӿ�ͣ�µ�ͣ�³�ǹYaw��Ļ����ƶ��仯,(���淢���������ǹҲ����,������,��һ��)
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotator, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		bUseControllerRotationYaw = false;//���Ǿ�ֹʱ�����ƶ�����ӽ�����תʱPlayerС���˲���Ҳ����ת
		TurnInPlace(DeltaTime);//ԭ��ת��
	}
	//��Ҫ�洢�ܲ�ʱ���ڿ��е�ֵ
	if (Speed > 0.f || bIsInAir) {
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);//�����ʱ���ͣ��ʱYaw�᷽�����ת(������ת)
		AO_Yaw = 0.f;//�������ƶ���ʱ��Ҫʱ�̱���YawΪ0
		bUseControllerRotationYaw = true;//�����˶�ʱ�����ƶ�����ӽ�����תʱPlayerС����ҲҪҲ����ת
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;//�ڿ���ʱ������ת��
	}

	//Pitch�Ͳ�����ô�鷳,
	AO_Pitch = GetBaseAimRotation().Pitch;
	//90�������ײ���ƽ���ߵ�ͷ��ʱ360->270����>90����,
	//�����Ǹ�����ֻ��Ҫ�ı������ԹϵĿͻ��˺ͷ���˵�Pitchֵ�ͺ���,�Ϳ��Կ�����ȷ��̧ǹ
	if (AO_Pitch > 90.f && !IsLocallyControlled()) {
		//��Pitch��[270,360)ת��Ϊ[-90,0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::TurnInPlace(float DeltaTime) {
	//��ʲôʱ��ԭ��ת����?��Yaw>90���<-90��ʱ
	//UE_LOG(LogTemp, Warning, TEXT("AO_Yaw: %f"), AO_Yaw);
	if (AO_Yaw > 90.f) {
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f) {
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
}

//ֻ�б��ؿ��ƵĽ�ɫ�ص����������ʾE-pickup��
void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{//Overlapping =�ص�
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())//ֻ�б��ؿ��ƵĽ�ɫ�ص����������ʾE-pickup��
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

//���Ʊ���ʱ���Զ�������,����OverlappingWeapon��������Ķ���,�й�AWeapon��ָ���
void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)//?????????????
{//Rep=����
	//ֻҪ����˷����仯��ֵ�ͻḳֵ���ͻ���
	//���˲²���������Ƕ�������ص���һ��ʱ��ʾ�ĸ�E-pickup,�Ҿ�����ʾ�ʼ�Ǹ�����E-pickup,
	//E-pickup�����ʼ�Ǹ�����ͷ��,���������E��������µ��Ǹ�����
	if (OverlappingWeapon)
	{//
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

bool ABlasterCharacter::IsWeaponEquipped()//�Ƿ��Ѿ�װ��������
{
	return (Combat && Combat->EquippedWeapon);
}

//ABlasterCharacter����������,����IsAiming()����ABlasterCharacter�������
bool ABlasterCharacter::IsAiming()
{//�Ƿ��Ѿ�����׼,��Ҫ��ȡCombatComponentս���������׼״̬
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
//	//����Ĭ������ͷ���ɱ�
//	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
//	//����ͷ���ɱ۽�����ϵ
//	CameraBoom->SetupAttachment(GetMesh());
//	//����ͷ���ɱ�Ŀ�곤��
//	CameraBoom->TargetArmLength = 600.0f;
//	//����ͷ���ɱ��Ƿ������ת
//	CameraBoom->bUsePawnControlRotation = true;
//
//	//����Ĭ������ͷ
//	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
//	//����ͷ�뵯�ɱ۽�����ϵ
//	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
//	//����ͷ����Ҫ��ת,���ɱ���ת�ͺ���
//	FollowCamera->bUsePawnControlRotation = false;
//	
//	//���������ƶ�����ӽ�����תʱPlayerС���˲���Ҳ����ת
//	bUseControllerRotationYaw = false;
//	//����С����ת�俿���ǰ������³����ļ��ٶ�,
//	GetCharacterMovement()->bOrientRotationToMovement = true;
//
//	//����UWidgetComponent������OverheadWidget,��������ڵڶ���
//	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
//	OverheadWidget->SetupAttachment(RootComponent);//������ڵڶ���
//
//	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
//	Combat->SetIsReplicated(true);//????????????????
//
//	//��Charater��NavMovement��CanCrouch��Ϊtrue
//	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
//}
//
////??????????????????????????�һ������Ǹ���ɫ���ϵ�̽��,��̽��ɫ�Ƿ��������ص�
//void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	//???????????����̽������Ϣ���Ƹ��ٿ��Լ���ɫ�Ŀͻ���
//	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);//?????????????
//}
//
//void ABlasterCharacter::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}
//
////Tick�Ҳ²��ǽ�ɫ���ϵĴ�����
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
//	//��this(Player)��ӳ��,��MoveForward()�����󵽰���(MoveForward)��,
//	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
//	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
//	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);
//	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);
//
//	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABlasterCharacter::EquipButtonPressed);//����E����װ������
//	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABlasterCharacter::CrouchButtonPressed);//����shift���Ͷ���
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
///// ǰ���ƶ�
///// </summary>
///// <param name="Value"></param>
//void ABlasterCharacter::MoveForward(float Value)
//{
//	//
//	if (Controller != nullptr && Value != 0.f) {
//		//ͨ�׵�:
//		//Yaw��������Z����ת,RollΧ��X����ת,PitchΧ��Y����ת
//		//������ǻ�ȡPlayer�����ĸ��Ƕ�
//		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
//		//������ǽ�Player������ĽǶ�(X)��������,�����Ǹ�X��Player����Եķ���
//		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
//		//�������Player�����������ƶ�
//		AddMovementInput(Direction, Value);
//	}
//}
//
///// <summary>
///// �����ƶ�
///// </summary>
///// <param name="Value"></param>
//void ABlasterCharacter::MoveRight(float Value)
//{
//	if (Controller != nullptr && Value != 0.f) {
//	//������ǻ�ȡPlayer�����ĸ��Ƕ�
//	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
//	//������ǽ�Player����(Y)�ĽǶ���������,�����Ǹ�Y��Player����ķ���
//	const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
//	//�������Player�����������ƶ�
//	AddMovementInput(Direction, Value);
//	}
//}
//
///// <summary>
///// ���ҿ�
///// </summary>
///// <param name="Value"></param>
//void ABlasterCharacter::Turn(float Value)
//{
//	AddControllerYawInput(Value);
//}
//
///// <summary>
///// ���¿�
///// </summary>
///// <param name="Value"></param>
//void ABlasterCharacter::LookUp(float Value)
//{
//	AddControllerPitchInput(Value);
//}
//
///// <summary>
///// ??????????????????װ������
///// </summary>
//void ABlasterCharacter::EquipButtonPressed()
//{
//	if (Combat) {
//		if (HasAuthority()) {
//			Combat->EquipWeapon(OverlappingWeapon);//װ���������е�����
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
//		Combat->EquipWeapon(OverlappingWeapon);//װ���������е�����
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
//	if (IsLocallyControlled()) {//����Ǳ��زٿ�player
//		if (OverlappingWeapon) {
//			OverlappingWeapon->ShowPickupWidget(true);
//		}
//	}
//}
//
////�Ƿ����ѱ�װ������
//bool ABlasterCharacter::IsWeaponEquipped()
//{
//	return (Combat && Combat->EquippedWeapon);
//}
//
////Player�Ƿ�����׼
//bool ABlasterCharacter::IsAiming()
//{
//	return (Combat && Combat->bAiming);
//}
//
//

