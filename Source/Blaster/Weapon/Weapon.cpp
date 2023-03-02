
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;//������ſ��Ը���,����һЩ�ͻ��˺ͷ�����������һ����

	//��������,���ڸ�Ŀ¼
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);//?????????????????

	//????????????????????????
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//Ψ����player(ECC_Pawn)�ܹ�ȥ�������ײ
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//??????????????????????�ر���ײ
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//���������η�Χ,���ǲ�֪�������õ�????????????????
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//����E-pick up�⼸����,����������ͼ����
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())//�����������ָֻ�з����(����,�һ���HasAuthority()�ͻ��˷���˶�����)�ɽ���,AreaSphere�����þʹ�
	{//,QueryAndPhysics�ǿ����ڿռ��ѯ������Ͷ�䡢ɨ�衢�ص�����ģ�⣨���塢Լ������
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//query=��ѯ
		//��ײ����Player�Ļ����ص�
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		//?????????????????????????���붯̬ί��,�߽���������Ȧ
		//OnComponentBeginOverlap��ĳЩ���ݿ�ʼ�������ص�ʱ���õ��¼������磬����߽���������
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		//??????????????????????????���붯̬ί��,�߳���������Ȧ
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
	if (PickupWidget)//���༸��������Զ�Ŀ����������ϵ�E-pickup
	{
		PickupWidget->SetVisibility(false);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);//????������ǽ�����״̬������ͻ��˺ͷ����
}

//Player�߽�������ΧȦ
void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);//��ȡBlasterCharacter����
	if (BlasterCharacter)
	{
		//��ʾE-pickup,ֻ�б��ؿ��ƵĽ�ɫ�ص����������ʾE-pickup��
		BlasterCharacter->SetOverlappingWeapon(this);
	}
}

//Player�߳�������ΧȦ
void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
}

// �ⷽ�������������޸�����״̬
void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

//������������װ��,�ǿ������˵�װ���Ͳ�Ҫ��ʾE-Pickup,�ر�����Ȧ
void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		break;
	}
}

//��ʾE-pick up�⼸����
void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}


















// Fill out your copyright notice in the Description page of Project Settings.

//
//#include "Weapon.h"
//#include "Components/SphereComponent.h"
//#include "Components/WidgetComponent.h"
//#include "Blaster/Character/BlasterCharacter.h"
//#include "Components/SphereComponent.h"
//#include "Net/UnrealNetwork.h"
//
//AWeapon::AWeapon()
//{
//	PrimaryActorTick.bCanEverTick = false;
//	bReplicates = true;//������ſ��Ը���,����һЩ�ͻ��˺ͷ�����������һ����
//
//	//��������,���ڸ�Ŀ¼
//	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
//	//WeaponMesh->SetupAttachment(RootComponent);
//	SetRootComponent(WeaponMesh);//?????????????????
//
//	//????????????????????????
//	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
//	//Ψ����player(ECC_Pawn)�ܹ�ȥ�������ײ
//	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
//	//??????????????????????�ر���ײ
//	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//
//	//���������η�Χ,���ǲ�֪�������õ�????????????????
//	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
//	AreaSphere->SetupAttachment(RootComponent);
//	AreaSphere->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
//	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//
//	//����E-pick up�⼸����,����������ͼ����
//	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
//	PickupWidget->SetupAttachment(RootComponent);
//
//	//bReplicates = true;
//
//	//WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
//
//	//SetRootComponent(WeaponMesh);
//
//	//WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
//	//WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
//	//WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//	//AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
//	//AreaSphere->SetupAttachment(RootComponent);
//	//AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
//	//AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//
//	//PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
//	//PickupWidget->SetupAttachment(RootComponent);
//}
//
//void AWeapon::BeginPlay()
//{
//	Super::BeginPlay();
//
//	//����ٿص���ROLE_Authority����Ļ�,AreaSphere�����þʹ�
//	if (HasAuthority()) {
//		//QueryAndPhysics�ǿ����ڿռ��ѯ������Ͷ�䡢ɨ�衢�ص�����ģ�⣨���塢Լ������
//		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//		//��ײ����Player�Ļ����ص�
//		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
//
//		//?????????????????????????
//		//OnComponentBeginOverlap��ĳЩ���ݿ�ʼ�������ص�ʱ���õ��¼������磬����߽���������
//		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
//		//??????????????????????????
//		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
//	}
//	if (PickupWidget)
//	{
//		PickupWidget->SetVisibility(false);
//	}
//
//
//
//	//Super::BeginPlay();
//	//if (HasAuthority())
//	//{
//	//	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//	//	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
//	//	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
//	//}
//	//if (PickupWidget)
//	//{
//	//	PickupWidget->SetVisibility(false);
//	//}
//	
//}
//
//void AWeapon::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//
//void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	DOREPLIFETIME(AWeapon, WeaponState);//????������ǽ�����״̬������ͻ��˺ͷ����
//}
//
//
////????????????????????????????????????
//void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	//?????????????????????????????????���߼���˵,��ҽӽ�����ʶ��Ȧ,����ͷ�Ͼͻ��������
//	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);//��ȡBlasterCharacter����
//	if (BlasterCharacter)
//	{
//		//PickupWidget->SetVisibility(true);
//		BlasterCharacter->SetOverlappingWeapon(this);
//	}
//}
//
////????????????????????????????????????
//void AWeapon::OnSphereEndOverlap(//Player�߳�������ΧȦ
//	UPrimitiveComponent* OverlappedComponent,
//	AActor* OtherActor,
//	UPrimitiveComponent* OtherComp,
//	int32 OtherBodyIndex
//) {
//	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);//��ȡBlasterCharacter����
//	if (BlasterCharacter)
//	{
//		//PickupWidget->SetVisibility(true);
//		BlasterCharacter->SetOverlappingWeapon(nullptr);
//	}
//}
//
///// <summary>
///// �ⷽ�������������޸�����״̬
///// </summary>
///// <param name="State"></param>
//void AWeapon::SetWeaponState(EWeaponState State)
//{
//	WeaponState = State;
//	switch (WeaponState)
//	{
//	case EWeaponState::EWS_Equipped:
//		ShowPickupWidget(false);
//		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//		break;
//	}
//}
//
////������������װ��,�ǿ������˵�װ���Ͳ�Ҫ��ʾE-Pickup,�ر�����Ȧ
//void AWeapon::OnRep_WeaponState()
//{
//	switch (WeaponState)
//	{
//	case EWeaponState::EWS_Equipped:
//		ShowPickupWidget(false);
//		break;
//	}
//}
//
//
////��ʾE-pick up�⼸����
//void AWeapon::ShowPickupWidget(bool bShowWidget)
//{
//	if (PickupWidget) {
//		PickupWidget->SetVisibility(bShowWidget);
//	}
//}
