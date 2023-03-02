
#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/Weapon/Weapon.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	//拿到BlasterCharacter代码
	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)//本端更新动画
{
	Super::NativeUpdateAnimation(DeltaTime);
	//拿到BlasterCharacter代码
	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if (BlasterCharacter == nullptr) return;
	//返回根组件的速度(cm/s),这是一个向量来的,把这个速度向量在Z方向的速度归0
	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();//xy轴速度,平移速度
	//查看Player其是否在空中
	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
	//查看Player其加速度
	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	//查看武器是否被装备
	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
	//查看角色是否蹲下,注意BlasterCharacter的bIsCrouched和UBlasterAnimInstance的bIsCrouched不一样,BlasterCharacter的是系统内置的
	bIsCrouched = BlasterCharacter->bIsCrouched;
	//在这将动画的瞄准状态与BlasterCharacter的瞄准状态绑定,
	//而BlasterCharacter的瞄准状态与CombatComponent是否瞄准为准,
	bAiming = BlasterCharacter->IsAiming();
	TurningInPlace = BlasterCharacter->GetTurningInPlace();//转身状态

	//我猜这个是获取鼠标瞄准偏移值的,而且FRotator这个是个组件.
	FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();//鼠标方向
	//传入角色力的方向变量//键盘方向
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	//鼠标方向和键盘方向的差值,NormalizedDeltaRotator我猜就是计算差值的把
	YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;//未来要走的方向
	CharacterRotation = BlasterCharacter->GetActorRotation();//现在的方向
	//NormalizedDeltaRotator我猜就是计算差值的把
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	//现在Delta还是很小的值,需要变大
	const float Target = Delta.Yaw / DeltaTime;
	//从Lean从Target。按距离缩放到目标，所以它有一个强大的开始速度6.0f
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);//将Interp限制在-90到90之间

	AO_Yaw = BlasterCharacter->GetAO_Yaw();//获得人物持枪Yaw值
	AO_Pitch = BlasterCharacter->GetAO_Pitch();

	//首先是否装备了武器,武器是否存在,拉到武器网格,还要角色的网格,这里要搞武器的插槽对应左手绑定一起
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh()) {
		//我们需要从我们装备的武器上的插槽LeftHandSocket获取到代码的LeftHandTransform变量,后面那个RTS_World搞不懂???question
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;//存结果,存着左手插槽的位置
		FRotator OutRotation;//存结果,存着左手插槽的旋转
		//我们想将武器的左手插槽转化为人物骨骼之一blaster character mesh,那左手插槽将成为我们右手骨骼之一,
		//找出左手插槽相对于右手的相对位置,这些都是通过引用传入的,所以要及时把结果存起来
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetTranslation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));//这时他已经转换为我们的骨骼空间了,可以在蓝图中使用了,
	}
}
















//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "BlasterAnimInstance.h"
//#include "BlasterCharacter.h"
//#include "GameFramework/CharacterMovementComponent.h"
//
//void UBlasterAnimInstance::NativeInitializeAnimation() {
//	Super::NativeInitializeAnimation();
//
//	//拿到BlasterCharacter代码
//	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
//}
//
//void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
//{
//	Super::NativeUpdateAnimation(DeltaTime);
//
//	//拿到BlasterCharacter代码
//	if (BlasterCharacter == nullptr) {
//		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
//	}
//	if (BlasterCharacter == nullptr) return;
//
//	//返回根组件的速度(cm/s),这是一个向量来的,把这个速度向量在Z方向的速度归0
//	FVector Velocity = BlasterCharacter->GetVelocity();
//	Velocity.Z = 0.f;
//	Speed = Velocity.Size();
//
//	//查看Player其是否在空中
//	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
//	//查看Player其加速度
//	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
//	//查看武器是否被装备
//	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
//	//查看角色是否蹲下,注意BlasterCharacter的bIsCrouched和UBlasterAnimInstance的bIsCrouched不一样,BlasterCharacter的是系统内置的
//	bIsCrouched = BlasterCharacter->bIsCrouched;
//	//调用BlasterCharacter的代码来获取是否在瞄准状态
//	bAiming = BlasterCharacter->IsAiming();
//
//}
