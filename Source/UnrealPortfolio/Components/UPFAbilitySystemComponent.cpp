// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UPFAbilitySystemComponent.h"
#include "UPFGameplayTags.h"
#include "Player/UPFCharacterPlayer.h"

UUPFAbilitySystemComponent::UUPFAbilitySystemComponent()
{
}

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

// void UUPFAbilitySystemComponent::OnGiveAbilitySet(const UUPFAbilitySet* AbilitySet)
// {
// 	AUPFCharacterPlayer* CharacterPlayer = Cast<AUPFCharacterPlayer>(GetOwner());
// 	if (!IsValid(CharacterPlayer)) return;
// 	if (!CharacterPlayer->IsLocallyControlled()) return;
//
// 	CharacterPlayer->BindAbilityInput(AbilitySet);
// }
//
// void UUPFAbilitySystemComponent::OnRemoveAbilitySet(const UUPFAbilitySet* AbilitySet)
// {
// 	AUPFCharacterPlayer* CharacterPlayer = Cast<AUPFCharacterPlayer>(GetOwner());
// 	if (!IsValid(CharacterPlayer)) return;
// 	if (!CharacterPlayer->IsLocallyControlled()) return;
//
// 	CharacterPlayer->UnBindAbilityInput(AbilitySet);
// }
