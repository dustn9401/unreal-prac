// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UPFPlayerController.h"

AUPFPlayerController::AUPFPlayerController()
{

}

void AUPFPlayerController::Respawn()
{
	ServerRestartPlayer();
}

void AUPFPlayerController::QuitLevel()
{
	// todo
}

void AUPFPlayerController::PlayCameraShake(const TSubclassOf<UCameraShakeBase> CameraShakeClass)
{
	if (!ensure(PlayerCameraManager)) return;
	if (!IsLocalPlayerController()) return;

	PlayerCameraManager->StartCameraShake(CameraShakeClass, 1.0f, ECameraShakePlaySpace::CameraLocal, FRotator::ZeroRotator);
}
