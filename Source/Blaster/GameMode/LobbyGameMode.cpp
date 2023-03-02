// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"


void ALobbyGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	//获取大厅的玩家人数
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	//人数大于2会传送到BlasterMap地图
	if (NumberOfPlayers == 2) {
		UWorld* World = GetWorld();
		if (World) {
			//允许无缝旅行,无缝旅行（）在后台加载并且不会断开客户端
			bUseSeamlessTravel = true;
			World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
		}
	}
}