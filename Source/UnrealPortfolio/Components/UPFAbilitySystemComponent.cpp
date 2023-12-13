// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UPFAbilitySystemComponent.h"
#include "UPFGameplayTags.h"
#include "Player/UPFCharacterPlayer.h"

UUPFAbilitySystemComponent::UUPFAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UUPFAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for(const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.InputPressed)
		{
			UPF_LOG_COMPONENT(LogTemp, Log, TEXT("Pressed: %d"), AbilitySpec.InputID);
		}
	}
}

void UUPFAbilitySystemComponent::AbilityLocalInputPressing(int32 InputID)
{
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.InputID == InputID)
		{
			if (Spec.Ability)
			{
				Spec.InputPressed = true;
				if (Spec.IsActive()) break;
				TryActivateAbility(Spec.Handle);
			}
		}
	}
}
