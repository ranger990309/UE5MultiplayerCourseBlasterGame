
#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/Weapon/Weapon.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	//�õ�BlasterCharacter����
	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)//���˸��¶���
{
	Super::NativeUpdateAnimation(DeltaTime);
	//�õ�BlasterCharacter����
	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if (BlasterCharacter == nullptr) return;
	//���ظ�������ٶ�(cm/s),����һ����������,������ٶ�������Z������ٶȹ�0
	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();//xy���ٶ�,ƽ���ٶ�
	//�鿴Player���Ƿ��ڿ���
	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
	//�鿴Player����ٶ�
	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	//�鿴�����Ƿ�װ��
	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
	//�鿴��ɫ�Ƿ����,ע��BlasterCharacter��bIsCrouched��UBlasterAnimInstance��bIsCrouched��һ��,BlasterCharacter����ϵͳ���õ�
	bIsCrouched = BlasterCharacter->bIsCrouched;
	//���⽫��������׼״̬��BlasterCharacter����׼״̬��,
	//��BlasterCharacter����׼״̬��CombatComponent�Ƿ���׼Ϊ׼,
	bAiming = BlasterCharacter->IsAiming();
	TurningInPlace = BlasterCharacter->GetTurningInPlace();//ת��״̬

	//�Ҳ�����ǻ�ȡ�����׼ƫ��ֵ��,����FRotator����Ǹ����.
	FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();//��귽��
	//�����ɫ���ķ������//���̷���
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	//��귽��ͼ��̷���Ĳ�ֵ,NormalizedDeltaRotator�Ҳ¾��Ǽ����ֵ�İ�
	YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;//δ��Ҫ�ߵķ���
	CharacterRotation = BlasterCharacter->GetActorRotation();//���ڵķ���
	//NormalizedDeltaRotator�Ҳ¾��Ǽ����ֵ�İ�
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	//����Delta���Ǻ�С��ֵ,��Ҫ���
	const float Target = Delta.Yaw / DeltaTime;
	//��Lean��Target�����������ŵ�Ŀ�꣬��������һ��ǿ��Ŀ�ʼ�ٶ�6.0f
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);//��Interp������-90��90֮��

	AO_Yaw = BlasterCharacter->GetAO_Yaw();//��������ǹYawֵ
	AO_Pitch = BlasterCharacter->GetAO_Pitch();

	//�����Ƿ�װ��������,�����Ƿ����,������������,��Ҫ��ɫ������,����Ҫ�������Ĳ�۶�Ӧ���ְ�һ��
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh()) {
		//������Ҫ������װ���������ϵĲ��LeftHandSocket��ȡ�������LeftHandTransform����,�����Ǹ�RTS_World�㲻��???question
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;//����,�������ֲ�۵�λ��
		FRotator OutRotation;//����,�������ֲ�۵���ת
		//�����뽫���������ֲ��ת��Ϊ�������֮һblaster character mesh,�����ֲ�۽���Ϊ�������ֹ���֮һ,
		//�ҳ����ֲ����������ֵ����λ��,��Щ����ͨ�����ô����,����Ҫ��ʱ�ѽ��������
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetTranslation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));//��ʱ���Ѿ�ת��Ϊ���ǵĹ����ռ���,��������ͼ��ʹ����,
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
//	//�õ�BlasterCharacter����
//	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
//}
//
//void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
//{
//	Super::NativeUpdateAnimation(DeltaTime);
//
//	//�õ�BlasterCharacter����
//	if (BlasterCharacter == nullptr) {
//		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
//	}
//	if (BlasterCharacter == nullptr) return;
//
//	//���ظ�������ٶ�(cm/s),����һ����������,������ٶ�������Z������ٶȹ�0
//	FVector Velocity = BlasterCharacter->GetVelocity();
//	Velocity.Z = 0.f;
//	Speed = Velocity.Size();
//
//	//�鿴Player���Ƿ��ڿ���
//	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
//	//�鿴Player����ٶ�
//	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
//	//�鿴�����Ƿ�װ��
//	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
//	//�鿴��ɫ�Ƿ����,ע��BlasterCharacter��bIsCrouched��UBlasterAnimInstance��bIsCrouched��һ��,BlasterCharacter����ϵͳ���õ�
//	bIsCrouched = BlasterCharacter->bIsCrouched;
//	//����BlasterCharacter�Ĵ�������ȡ�Ƿ�����׼״̬
//	bAiming = BlasterCharacter->IsAiming();
//
//}
