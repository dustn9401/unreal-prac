// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/UPFCharacterControlData.h"

UUPFCharacterControlData::UUPFCharacterControlData(): bUseControllerRotationYaw(0), bOrientRotationToMovement(1), bUseControllerDesiredRotation(0), TargetArmLength(400.0f), bUsePawnControlRotation(1),
                                                      bInheritPitch(1), bInheritYaw(1),
                                                      bInheritRoll(1),
                                                      bDoCollisionTest(1)
{
	RotationRate = FRotator(0.0f, 0.0f, 720.0f);
	RelativeRotation = FRotator(0.0f, -50.0f, 0.0f);
}
