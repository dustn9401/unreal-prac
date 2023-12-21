// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UPFAbilitySystemComponent.h"
#include "UPFGameplayTags.h"
#include "Player/UPFCharacterPlayer.h"

UUPFAbilitySystemComponent::UUPFAbilitySystemComponent()
{

}

void UUPFAbilitySystemComponent::AbilityInputTagTriggered(const FGameplayTag InputTag)
{
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			if (Spec.Ability)
			{
				Spec.InputPressed = true;
				if (Spec.IsActive())
				{
					AbilitySpecInputPressed(Spec);
					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());					
				}
				else
				{
					TryActivateAbility(Spec.Handle);
				}
			}
		}
	}
}

void UUPFAbilitySystemComponent::AbilityInputTagCompleted(const FGameplayTag InputTag)
{
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			Spec.InputPressed = false;
			if (Spec.Ability && Spec.IsActive())
			{
				AbilitySpecInputReleased(Spec);
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}
