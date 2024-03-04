// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UPFCharacterNonPlayer.h"

#include "AI/UPFAIController.h"
#include "AbilitySystemBlueprintLibrary.h"

AUPFCharacterNonPlayer::AUPFCharacterNonPlayer()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AUPFAIController::StaticClass();
}

FName AUPFCharacterNonPlayer::GetStatGroup()
{
	return FName(TEXT("Enemy"));
}

void AUPFCharacterNonPlayer::OnDeathStart()
{
	Super::OnDeathStart();

	if (AUPFAIController* AIController = Cast<AUPFAIController>(GetController()))
	{
		AIController->StopAI();
	}
}

float AUPFCharacterNonPlayer::GetAIPatrolRadius()
{
	return 800.0f;
}

float AUPFCharacterNonPlayer::GetAIDetectRange()
{
	return 400.0f;
}

float AUPFCharacterNonPlayer::GetAIAttackRange()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ensure(ASC)) return 0.0f;
	
	return ASC->GetNumericAttribute(UUPFStatSet::GetAttackRangeAttribute());
}

float AUPFCharacterNonPlayer::GetAITurnSpeed()
{
	return 0.0f;
}

void AUPFCharacterNonPlayer::ActivateAbilityByAI(FGameplayTag AbilityTag)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ensure(ASC)) return;

	for(FGameplayAbilitySpec& AbilitySpec : ASC->GetActivatableAbilities())
	{
		// todo
	}
	
	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AbilityTag, Payload);

	if (ASC->HasMatchingGameplayTag(AbilityTag))
	{
		ASC->OnAbilityEnded
	}
}

void AUPFCharacterNonPlayer::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}
