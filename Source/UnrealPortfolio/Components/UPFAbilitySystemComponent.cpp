// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UPFAbilitySystemComponent.h"

#include "EnhancedInputComponent.h"
#include "UPFGameplayTags.h"

void UUPFAbilitySystemComponent::SetupPlayerInputComponent(UEnhancedInputComponent* InputComponent)
{
	for(const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.DynamicAbilityTags.HasTag(UPFGameplayTags::InputTag_Ability))
		{
			// InputComponent->BindAction()
		}	
	}
	
	if (GameplayEventTriggeredAbilities.Contains(UPFGameplayTags::InputTag_Ability))
	{
		for(const auto& SpecHandle : GameplayEventTriggeredAbilities[UPFGameplayTags::InputTag_Ability])
		{
			if (!SpecHandle.IsValid()) continue;
			// FindAbilitySpecFromHandle()
		}
	}
	
}
