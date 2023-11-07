// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFGameMode.h"

#include "UPFGameState.h"
#include "UPFPlayerState.h"

AUPFGameMode::AUPFGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Game/Blueprints/BP_UPFPlayerController.BP_UPFPlayerController_C"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Game/Blueprints/BP_UPFCharacterPlayer.BP_UPFCharacterPlayer_C"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}
	
	GameStateClass = AUPFGameState::StaticClass();
	PlayerStateClass = AUPFPlayerState::StaticClass();
}
