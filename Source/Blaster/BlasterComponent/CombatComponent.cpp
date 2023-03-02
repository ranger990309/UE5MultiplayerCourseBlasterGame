
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
	SetIsReplicated(true);//这是我自己写的,源代码没有

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character) {
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;//设置基础速度
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;//这个可以是瞄准状态也可以是非瞄准状态

	//HasAuthority //判定当前是否运行在Authority（授权）服务器上
	//若client端想要瞄准,需要发送请求瞄准的指令向服务端,服务端同意后才能瞄准
	ServerSetAiming(bIsAiming);//这个方法的内部逻辑呢?没写?????????(好像不用写的)
	//其实无需判断是不是服务器,ServerSetAiming写了UFUNCTION(Server, Reliable),其只在服务器运行

	if (Character) {
		//如果是瞄准状态下行走速度会被限制
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

//这个原本是ServerSetAiming快捷创建的又加上_Implementation(实施),啥意思??????????
void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;//赋值到所有的客户端????
	if (Character) {
		//如果是瞄准状态下行走速度会被限制
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon() {
	//如果我们装备的武器有效,就会对F进行检查,若装备了武器我们就不应该定向移动
	if (EquippedWeapon && Character) {
		//???????角色禁用orient rotation去移动,看不懂
		//如果为true，将字符旋转到加速的方向，使用RotationRate作为旋转变化的速率。覆盖UseControllerDesiredRotation。
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;//我怀疑这个false是让头别和脚同个方向
		//如果为真，这个角色的偏航将被更新为匹配控制器的ControlRotation偏航，如果由一个玩家控制器控制。
		Character->bUseControllerRotationYaw = true;//我怀疑这个true是让头和鼠标方向一致
	}
}

//若是client端在瞄准,还要将行为回传给Server端,这样Server端才看得见???????
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

//我猜这个是Server端在瞄准,还要将行为回传给client端,这样client端才看得见
void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{//(这个方法是在重写内部的函数方法)
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);//这东西是将装备武器这个动作分享给所有客户端
	DOREPLIFETIME(UCombatComponent, bAiming);//这东西是将这个动作分享给所有客户端
}

//装备武器
void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);//设置武器状态
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());//武器和手槽连接
	}
	EquippedWeapon->SetOwner(Character);//设置武器的主人

	//???????角色禁用orient rotation去移动,看不懂
	//如果为true，将字符旋转到加速的方向，使用RotationRate作为旋转变化的速率。覆盖UseControllerDesiredRotation。
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;//我怀疑这个false是让头别和脚同个方向
	//如果为真，这个角色的偏航将被更新为匹配控制器的ControlRotation偏航，如果由一个玩家控制器控制。
	Character->bUseControllerRotationYaw = true;//我怀疑这个true是让头和鼠标方向一致
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
//	//若是client端在瞄准,还要将行为回传给Server端,这样Server端才看得见
//	ServerSetAiming(bIsAiming);//
//}
//void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)//Implementation执行
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
//	DOREPLIFETIME(UCombatComponent, EquippedWeapon);//这东西是将装备武器这个动作分享给客户端
//	DOREPLIFETIME(UCombatComponent, bAiming);//这东西是将这个动作分享给客户端
//}
//
//
////装备武器
//void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
//{
//	if (Character == nullptr || WeaponToEquip == nullptr) return;
//	EquippedWeapon = WeaponToEquip;
//	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);//设置武器状态
//	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));//得到RightHandSocket
//	if (HandSocket)
//	{
//		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());//武器和手槽连接
//	}
//	EquippedWeapon->SetOwner(Character);//设置武器的主人
//}
