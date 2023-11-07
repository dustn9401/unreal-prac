// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterPlayer.h"

AUPFCharacterPlayer::AUPFCharacterPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void AUPFCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void AUPFCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
