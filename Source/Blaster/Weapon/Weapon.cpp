
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;//这代表着可以复制,可以一些客户端和服务器的数据一起复制

	//创建武器,放在根目录
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);//?????????????????

	//????????????????????????
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//唯独对player(ECC_Pawn)跑过去会忽略碰撞
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//??????????????????????关闭碰撞
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//创建个球形范围,就是不知道干嘛用的????????????????
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//创建E-pick up这几个字,放在武器蓝图根部
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())//这里的作用是指只有服务端(存疑,我还以HasAuthority()客户端服务端都可以)可进入,AreaSphere的设置就打开
	{//,QueryAndPhysics是可用于空间查询（光线投射、扫描、重叠）和模拟（刚体、约束）。
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//query=查询
		//碰撞的是Player的话就重叠
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		//?????????????????????????加入动态委托,走进武器触发圈
		//OnComponentBeginOverlap当某些内容开始与此组件重叠时调用的事件，例如，玩家走进触发器。
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		//??????????????????????????加入动态委托,走出武器触发圈
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
	if (PickupWidget)//其余几个离武器远的看不见武器上的E-pickup
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

	DOREPLIFETIME(AWeapon, WeaponState);//????这可能是将武器状态分享给客户端和服务端
}

//Player走进武器范围圈
void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);//获取BlasterCharacter代码
	if (BlasterCharacter)
	{
		//显示E-pickup,只有本地控制的角色重叠武器后会显示E-pickup字
		BlasterCharacter->SetOverlappingWeapon(this);
	}
}

//Player走出武器范围圈
void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
}

// 这方法可以让外面修改武器状态
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

//武器被其他人装备,那靠近别人的装备就不要显示E-Pickup,关闭武器圈
void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		break;
	}
}

//显示E-pick up这几个字
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
//	bReplicates = true;//这代表着可以复制,可以一些客户端和服务器的数据一起复制
//
//	//创建武器,放在根目录
//	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
//	//WeaponMesh->SetupAttachment(RootComponent);
//	SetRootComponent(WeaponMesh);//?????????????????
//
//	//????????????????????????
//	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
//	//唯独对player(ECC_Pawn)跑过去会忽略碰撞
//	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
//	//??????????????????????关闭碰撞
//	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//
//	//创建个球形范围,就是不知道干嘛用的????????????????
//	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
//	AreaSphere->SetupAttachment(RootComponent);
//	AreaSphere->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
//	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//
//	//创建E-pick up这几个字,放在武器蓝图根部
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
//	//如果操控的是ROLE_Authority人物的话,AreaSphere的设置就打开
//	if (HasAuthority()) {
//		//QueryAndPhysics是可用于空间查询（光线投射、扫描、重叠）和模拟（刚体、约束）。
//		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//		//碰撞的是Player的话就重叠
//		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
//
//		//?????????????????????????
//		//OnComponentBeginOverlap当某些内容开始与此组件重叠时调用的事件，例如，玩家走进触发器。
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
//	DOREPLIFETIME(AWeapon, WeaponState);//????这可能是将武器状态分享给客户端和服务端
//}
//
//
////????????????????????????????????????
//void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	//?????????????????????????????????按逻辑来说,玩家接近武器识别圈,武器头上就会出现名字
//	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);//获取BlasterCharacter代码
//	if (BlasterCharacter)
//	{
//		//PickupWidget->SetVisibility(true);
//		BlasterCharacter->SetOverlappingWeapon(this);
//	}
//}
//
////????????????????????????????????????
//void AWeapon::OnSphereEndOverlap(//Player走出武器范围圈
//	UPrimitiveComponent* OverlappedComponent,
//	AActor* OtherActor,
//	UPrimitiveComponent* OtherComp,
//	int32 OtherBodyIndex
//) {
//	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);//获取BlasterCharacter代码
//	if (BlasterCharacter)
//	{
//		//PickupWidget->SetVisibility(true);
//		BlasterCharacter->SetOverlappingWeapon(nullptr);
//	}
//}
//
///// <summary>
///// 这方法可以让外面修改武器状态
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
////武器被其他人装备,那靠近别人的装备就不要显示E-Pickup,关闭武器圈
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
////显示E-pick up这几个字
//void AWeapon::ShowPickupWidget(bool bShowWidget)
//{
//	if (PickupWidget) {
//		PickupWidget->SetVisibility(bShowWidget);
//	}
//}
