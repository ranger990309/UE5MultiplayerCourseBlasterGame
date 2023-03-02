
#include "CombatComponent.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);//�������Լ�д��,Դ����û��

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;//���û����ٶ�
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;//�����������׼״̬Ҳ�����Ƿ���׼״̬

	//HasAuthority //�ж���ǰ�Ƿ�������Authority����Ȩ����������
	//��client����Ҫ��׼,��Ҫ����������׼��ָ��������,�����ͬ��������׼
	ServerSetAiming(bIsAiming);//����������ڲ��߼���?ûд?????????(������д��)
	//��ʵ�����ж��ǲ��Ƿ�����,ServerSetAimingд��UFUNCTION(Server, Reliable),��ֻ�ڷ���������

	if (Character) {
		//�������׼״̬�������ٶȻᱻ����
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

//���ԭ����ServerSetAiming��ݴ������ּ���_Implementation(ʵʩ),ɶ��˼??????????
void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;//��ֵ�����еĿͻ���????
	if (Character) {
		//�������׼״̬�������ٶȻᱻ����
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon() {
	//�������װ����������Ч,�ͻ��F���м��,��װ�����������ǾͲ�Ӧ�ö����ƶ�
	if (EquippedWeapon && Character) {
		//???????��ɫ����orient rotationȥ�ƶ�,������
		//���Ϊtrue�����ַ���ת�����ٵķ���ʹ��RotationRate��Ϊ��ת�仯�����ʡ�����UseControllerDesiredRotation��
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;//�һ������false����ͷ��ͽ�ͬ������
		//���Ϊ�棬�����ɫ��ƫ����������Ϊƥ���������ControlRotationƫ���������һ����ҿ��������ơ�
		Character->bUseControllerRotationYaw = true;//�һ������true����ͷ����귽��һ��
	}
}

//����client������׼,��Ҫ����Ϊ�ش���Server��,����Server�˲ſ��ü�???????
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

//�Ҳ������Server������׼,��Ҫ����Ϊ�ش���client��,����client�˲ſ��ü�
void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{//(�������������д�ڲ��ĺ�������)
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);//�ⶫ���ǽ�װ���������������������пͻ���
	DOREPLIFETIME(UCombatComponent, bAiming);//�ⶫ���ǽ����������������пͻ���
}

//װ������
void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);//��������״̬
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());//�������ֲ�����
	}
	EquippedWeapon->SetOwner(Character);//��������������

	//???????��ɫ����orient rotationȥ�ƶ�,������
	//���Ϊtrue�����ַ���ת�����ٵķ���ʹ��RotationRate��Ϊ��ת�仯�����ʡ�����UseControllerDesiredRotation��
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;//�һ������false����ͷ��ͽ�ͬ������
	//���Ϊ�棬�����ɫ��ƫ����������Ϊƥ���������ControlRotationƫ���������һ����ҿ��������ơ�
	Character->bUseControllerRotationYaw = true;//�һ������true����ͷ����귽��һ��
}

















//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "CombatComponent.h"
//#include "Blaster/Weapon/Weapon.h"
//#include "Blaster/Character/BlasterCharacter.h"
//#include "Engine/SkeletalMeshSocket.h"
//#include "Components/SphereComponent.h"
//#include "Net/UnrealNetwork.h"
//
//// Sets default values for this component's properties
//
//UCombatComponent::UCombatComponent()
//{
//	PrimaryComponentTick.bCanEverTick = false;
//}
//
//
//
//void UCombatComponent::BeginPlay()
//{
//	Super::BeginPlay();
//
//}
//void UCombatComponent::SetAiming(bool bIsAiming)
//{
//	bAiming = bIsAiming;
//	//����client������׼,��Ҫ����Ϊ�ش���Server��,����Server�˲ſ��ü�
//	ServerSetAiming(bIsAiming);//
//}
//void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)//Implementationִ��
//{
//	bAiming = bIsAiming;
//}
//void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//}
//
//
//void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(UCombatComponent, EquippedWeapon);//�ⶫ���ǽ�װ�������������������ͻ���
//	DOREPLIFETIME(UCombatComponent, bAiming);//�ⶫ���ǽ��������������ͻ���
//}
//
//
////װ������
//void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
//{
//	if (Character == nullptr || WeaponToEquip == nullptr) return;
//	EquippedWeapon = WeaponToEquip;
//	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);//��������״̬
//	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));//�õ�RightHandSocket
//	if (HandSocket)
//	{
//		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());//�������ֲ�����
//	}
//	EquippedWeapon->SetOwner(Character);//��������������
//}
