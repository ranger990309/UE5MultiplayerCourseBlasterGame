#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//??????得到复制函数
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
protected:
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* OverheadWidget;

	//可以说OnRep_OverlappingWeapon就是专门在客户端运行的
	//当OverlappingWeapon被服务端传到客户端时OnRep_OverlappingWeapon他也跟着运行
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
		class AWeapon* OverlappingWeapon;
	UFUNCTION()
		void OnRep_OverlappingWeapon(AWeapon* LastWeapon);
	UPROPERTY(VisibleAnywhere)
		class UCombatComponent* Combat;
	UFUNCTION(Server, Reliable)
		void ServerEquipButtonPressed();

	float AO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;//转身状态
	void TurnInPlace(float DeltaTime);//

public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	//我觉得有点像属性,用GetAO_Yaw()就能调用私有的AO_Yaw,
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }//转身状态
};
 

















//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "BlasterCharacter.generated.h"
//
//UCLASS()
//class BLASTER_API ABlasterCharacter : public ACharacter
//{
//	GENERATED_BODY()
//
//public:
//	ABlasterCharacter();
//	virtual void Tick(float DeltaTime) override;
//	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
//	//?????????????????
//	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
//	virtual void PostInitializeComponents() override;
//
//protected:
//	virtual void BeginPlay() override;
//
//	void MoveForward(float Value);
//	void MoveRight(float Value);
//	void Turn(float Value);
//	void LookUp(float Value);
//	void EquipButtonPressed();
//	void CrouchButtonPressed();
//	void AimButtonPressed();
//	void AimButtonReleased();
//
//private:
//	UPROPERTY(VisibleAnywhere, Category = Camera)
//	class USpringArmComponent* CameraBoom;
//
//	UPROPERTY(VisibleAnywhere, Category = Camera)
//	class UCameraComponent* FollowCamera;
//
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
//		class UWidgetComponent* OverheadWidget;
//
//	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)//??????????
//	class AWeapon* OverlappingWeapon;//??????????
//
//	UFUNCTION()
//	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);//??????????
//
//	UPROPERTY(VisibleAnywhere)
//		class UCombatComponent* Combat;//?????????????
//
//	UFUNCTION(Server, Reliable)
//		void ServerEquipButtonPressed();//?????????????
//
//public:	
//	void SetOverlappingWeapon(AWeapon* Weapon);//??????????
//	bool IsWeaponEquipped();
//	bool IsAiming();
//
//};
