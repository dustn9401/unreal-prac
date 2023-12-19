// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UPFAbilitySystemComponent.h"
#include "UPFGameplayTags.h"
#include "Player/UPFCharacterPlayer.h"

UUPFAbilitySystemComponent::UUPFAbilitySystemComponent()
{

}

void UUPFAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag InputTag)
{
	// AbilityLocalInputPressed 함수 내용에서 태그로 어빌리티를 찾도록 수정한 함수
 
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
					if (Spec.Ability->bReplicateInputDirectly && IsOwnerActorAuthoritative() == false)
					{
						ServerSetInputPressed(Spec.Handle);
					}

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

void UUPFAbilitySystemComponent::AbilityInputTagPressing(const FGameplayTag InputTag)
{
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
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

void UUPFAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			Spec.InputPressed = false;
			if (Spec.Ability && Spec.IsActive())
			{
				if (Spec.Ability->bReplicateInputDirectly && IsOwnerActorAuthoritative() == false)
				{
					ServerSetInputReleased(Spec.Handle);
				}

				AbilitySpecInputReleased(Spec);
				
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}

