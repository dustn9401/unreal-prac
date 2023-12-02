// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFGameMode.h"
#include "UPFGameState.h"
#include "GameFramework/HUD.h"
#include "Player/UPFPlayerState.h"

AUPFGameMode::AUPFGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Game/UnrealPortfolio/Blueprints/BP_UPFPlayerController.BP_UPFPlayerController_C"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
	
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Game/UnrealPortfolio/Blueprints/BP_UPFCharacterPlayer.BP_UPFCharacterPlayer_C"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<AHUD> HUDClassRef(TEXT("/Game/UnrealPortfolio/UI/BP_UPFHUD.BP_UPFHUD_C"));
	if (HUDClassRef.Class)
	{
		HUDClass = HUDClassRef.Class;
	}
	
	GameStateClass = AUPFGameState::StaticClass();
	PlayerStateClass = AUPFPlayerState::StaticClass();
}
