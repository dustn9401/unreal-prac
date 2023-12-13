// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFAbilitySet.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "Character/UPFCharacterBase.h"
#include "Components/UPFAbilitySystemComponent.h"
#include "Player/UPFCharacterPlayer.h"

void FUPFGrantedAbilitySetData::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilityHandles.Add(Handle);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FUPFGrantedAbilityData::AddAbilitySpecHandle() Handle Not Valid, %s"), *Handle.ToString());
	}
}

void FUPFGrantedAbilitySetData::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		EffectHandles.Add(Handle);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FUPFGrantedAbilityData::AddGameplayEffectHandle() Handle Not Valid, %s"), *Handle.ToString());
	}
}

void FUPFGrantedAbilitySetData::TakeFromCharacter(AUPFCharacterBase* Character)
{
	check(Character);
	
	if (!Character->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("!Character->HasAuthority()"));
		return;
	}
	
	if (AUPFCharacterPlayer* CharacterPlayer = Cast<AUPFCharacterPlayer>(Character))
	{
		CharacterPlayer->ClientRPCRemoveAbilitySetBind(Index);
	}

	UAbilitySystemComponent* AbilityComp = Character->GetAbilitySystemComponent();
	for(const FGameplayAbilitySpecHandle& Handle : AbilityHandles)
	{
		if (Handle.IsValid())
		{
			AbilityComp->ClearAbility(Handle);
		}
	}

	for(const FActiveGameplayEffectHandle& Handle : EffectHandles)
	{
		if (Handle.IsValid())
		{
			AbilityComp->RemoveActiveGameplayEffect(Handle);
		}
	}

	AbilityHandles.Reset();
	EffectHandles.Reset();
}

void UUPFAbilitySet::GiveToCharacter(AUPFCharacterBase* Character, UObject* SrcObj, FUPFGrantedAbilitySetData* OutGrantData) const
{
	UAbilitySystemComponent* AbilityComp = Character->GetAbilitySystemComponent();
	
	for(const FUPFAbilityTriggerData& Data : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec(Data.Ability, 1, Data.InputID, SrcObj);
		FGameplayAbilitySpecHandle AbilityHandle = AbilityComp->GiveAbility(AbilitySpec);
		
		if (OutGrantData)
		{
			OutGrantData->AddAbilitySpecHandle(AbilityHandle);
		}
	}

	for(const TSubclassOf<UGameplayEffect> EffectClass : Effects)
	{
		const UGameplayEffect* EffectInst = EffectClass->GetDefaultObject<UGameplayEffect>();
		FGameplayEffectContextHandle EffectContextHandle = AbilityComp->MakeEffectContext();
		const FActiveGameplayEffectHandle EffectHandle = AbilityComp->ApplyGameplayEffectToSelf(EffectInst, 1.0f, EffectContextHandle);

		if (OutGrantData)
		{
			OutGrantData->AddGameplayEffectHandle(EffectHandle);
		}
	}

	if (AUPFCharacterPlayer* CharacterPlayer = Cast<AUPFCharacterPlayer>(Character))
	{
		CharacterPlayer->ClientRPCBindAbilitySetInput(this, OutGrantData ? OutGrantData->Index : INDEX_NONE);
	}
}
