// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/UPFGameplayAbilityWorldSubsystem.h"

#include "AbilitySystemGlobals.h"

void UUPFGameplayAbilityWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();

	UE_LOG(LogTemp, Log, TEXT("IsInitialized: %d"), UAbilitySystemGlobals::Get().IsAbilitySystemGlobalsInitialized());
	UAbilitySystemGlobals::Get().InitGlobalData();
}
