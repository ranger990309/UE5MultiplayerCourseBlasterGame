#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UCombatComponent();
	friend class ABlasterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void EquipWeapon(class AWeapon* WeaponToEquip);
protected:
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	// RPC的可靠性与校验 指定Reliable关键字保证可靠性
	UFUNCTION(Server, Reliable)//这个作用是只有服务端才能运行,数据回传客户端 Reliable=可靠
	void ServerSetAiming(bool bIsAiming);//这个方法的内部逻辑呢?没写?????????(好像不用写的)

	//可以说OnRep_EquippedWeapon就是专门在客户端运行的
	UFUNCTION()
	void OnRep_EquippedWeapon();

private:
	UPROPERTY()//???????不知道这个能不能起到同步瞄准的作用(不行)
	class ABlasterCharacter* Character;

	//当EquippedWeapon被服务端传到客户端时OnRep_EquippedWeapon他也跟着运行,
	//可以说OnRep_EquippedWeapon就是专门在客户端运行的
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)//Replicated=复制
	AWeapon* EquippedWeapon;//已经装备的武器

	UPROPERTY(Replicated)//这东西是将瞄准这个动作复制给客户端
	bool bAiming = false; //先判断是否在瞄准状态

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;//基础走路速度

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;//瞄准时走路速度



};
















// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Components/ActorComponent.h"
//#include "CombatComponent.generated.h"
//
//
//UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
//class BLASTER_API UCombatComponent : public UActorComponent
//{
//	GENERATED_BODY()
//
//public:	
//	UCombatComponent();
//	friend class ABlasterCharacter;
//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
//	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
//
//	void EquipWeapon(class AWeapon* WeaponToEquip);
//protected:
//	// Called when the game starts
//	virtual void BeginPlay() override;
//	void SetAiming(bool bIsAiming);
//
//	UFUNCTION(Server, Reliable)//数据回传客户端
//	void ServerSetAiming(bool bIsAiming);
//
//private:
//	class ABlasterCharacter* Character;
//
//	UPROPERTY(Replicated)//这东西是将装备武器这个动作分享给客户端
//		AWeapon* EquippedWeapon;
//
//	UPROPERTY(Replicated)
//		bool bAiming;
//
//public:
//
//		
//};
