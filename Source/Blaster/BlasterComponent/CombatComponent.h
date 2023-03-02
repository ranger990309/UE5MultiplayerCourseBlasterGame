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

	// RPC�Ŀɿ�����У�� ָ��Reliable�ؼ��ֱ�֤�ɿ���
	UFUNCTION(Server, Reliable)//���������ֻ�з���˲�������,���ݻش��ͻ��� Reliable=�ɿ�
	void ServerSetAiming(bool bIsAiming);//����������ڲ��߼���?ûд?????????(������д��)

	//����˵OnRep_EquippedWeapon����ר���ڿͻ������е�
	UFUNCTION()
	void OnRep_EquippedWeapon();

private:
	UPROPERTY()//???????��֪������ܲ�����ͬ����׼������(����)
	class ABlasterCharacter* Character;

	//��EquippedWeapon������˴����ͻ���ʱOnRep_EquippedWeapon��Ҳ��������,
	//����˵OnRep_EquippedWeapon����ר���ڿͻ������е�
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)//Replicated=����
	AWeapon* EquippedWeapon;//�Ѿ�װ��������

	UPROPERTY(Replicated)//�ⶫ���ǽ���׼����������Ƹ��ͻ���
	bool bAiming = false; //���ж��Ƿ�����׼״̬

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;//������·�ٶ�

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;//��׼ʱ��·�ٶ�



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
//	UFUNCTION(Server, Reliable)//���ݻش��ͻ���
//	void ServerSetAiming(bool bIsAiming);
//
//private:
//	class ABlasterCharacter* Character;
//
//	UPROPERTY(Replicated)//�ⶫ���ǽ�װ�������������������ͻ���
//		AWeapon* EquippedWeapon;
//
//	UPROPERTY(Replicated)
//		bool bAiming;
//
//public:
//
//		
//};
