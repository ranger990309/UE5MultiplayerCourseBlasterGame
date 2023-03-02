

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{//这是武器状态,比如处于初始化,被持有,被丢下
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	//武器数据发生变化客户端服务端都知道
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ShowPickupWidget(bool bShowWidget);//???????????????
protected:
	virtual void BeginPlay() override;
	//???????????????????????????????????进入武器触发圈
	UFUNCTION()
		virtual void OnSphereOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);

	UFUNCTION()
		void OnSphereEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex
		);

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		class USphereComponent* AreaSphere;//武器触发圈

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
		EWeaponState WeaponState;//武器状态

	UFUNCTION()
		void OnRep_WeaponState();//武器被其他人装备,那靠近别人的装备就不要显示E-Pickup

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		class UWidgetComponent* PickupWidget;//武器E-pickup字
public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }//方便外面得到武器范围圈
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }//方便外面得到武器骨骼网格

};

















//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
//#include "Weapon.generated.h"
//
//UENUM(BlueprintType)
//enum class EWeaponState : uint8//这是武器状态,比如处于初始化,被持有,被丢下
//{
//	EWS_Initial UMETA(DisplayName = "Initial State"),
//	EWS_Equipped UMETA(DisplayName = "Equipped"),
//	EWS_Dropped UMETA(DisplayName = "Dropped"),
//	EWS_MAX UMETA(DisplayName = "DefaultMAX")
//};
//
//UCLASS()
//class BLASTER_API AWeapon : public AActor
//{
//	GENERATED_BODY()
//	
//public:	
//	AWeapon();
//	virtual void Tick(float DeltaTime) override;
//	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;//武器数据发生变化客户端服务端都知道
//	void ShowPickupWidget(bool bShowWidget);//???????????????
//
//protected:
//	virtual void BeginPlay() override;
//
//	//???????????????????????????????????进入武器触发圈
//	UFUNCTION()
//		virtual void OnSphereOverlap(
//			UPrimitiveComponent* OverlappedComponent,
//			AActor* OtherActor,
//			UPrimitiveComponent* OtherComp,
//			int32 OtherBodyIndex,
//			bool bFromSweep,
//			const FHitResult& SweepResult
//		);
//
//	UFUNCTION()
//	void OnSphereEndOverlap(
//		UPrimitiveComponent* OverlappedComponent,
//		AActor* OtherActor,
//		UPrimitiveComponent* OtherComp,
//		int32 OtherBodyIndex
//		);
//	
//private:
//	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
//		USkeletalMeshComponent* WeaponMesh;
//
//	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
//		class USphereComponent* AreaSphere;//武器触发圈
//
//	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
//		EWeaponState WeaponState;//武器状态
//
//	UFUNCTION()
//	void OnRep_WeaponState();//武器被其他人装备,那靠近别人的装备就不要显示E-Pickup
//
//	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
//		class UWidgetComponent* PickupWidget;//武器E-pickup字
//public:
//	void SetWeaponState(EWeaponState State);
//	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }//方便外面得到武器范围圈
//};
