// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UPFAbilitySystemComponent.h"
#include "UPFGameplayTags.h"

void UUPFAbilitySystemComponent::AbilityLocalInputPressed(int32 InputID)
{
	// UE_LOG(LogTemp, Log, TEXT("AbilityLocalInputPressed, %d"), InputID);
	Super::AbilityLocalInputPressed(InputID);
}

void UUPFAbilitySystemComponent::AbilityLocalInputReleased(int32 InputID)
{
	// UE_LOG(LogTemp, Log, TEXT("AbilityLocalInputReleased, %d"), InputID);
	Super::AbilityLocalInputReleased(InputID);
}
