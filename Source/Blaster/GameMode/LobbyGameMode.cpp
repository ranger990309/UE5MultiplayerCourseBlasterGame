// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"


void ALobbyGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	//��ȡ�������������
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	//��������2�ᴫ�͵�BlasterMap��ͼ
	if (NumberOfPlayers == 2) {
		UWorld* World = GetWorld();
		if (World) {
			//�����޷�����,�޷����У����ں�̨���ز��Ҳ���Ͽ��ͻ���
			bUseSeamlessTravel = true;
			World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
		}
	}
}