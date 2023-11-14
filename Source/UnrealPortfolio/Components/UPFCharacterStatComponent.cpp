// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UPFCharacterStatComponent.h"

#include "AbilitySystemGlobals.h"
#include "GameplayAbilitiesModule.h"

// Sets default values for this component's properties
UUPFCharacterStatComponent::UUPFCharacterStatComponent()
{
}

void UUPFCharacterStatComponent::InitializeAttributes(UAbilitySystemComponent* ASC, const FName GroupName, const int32 Level)
{
	if (!ensure(ASC)) return;
	
	StatSet = ASC->GetSet<UUPFCharacterStatSet>();
	if (!StatSet)
	{
		UE_LOG(LogTemp, Error, TEXT("AbilitySystemComponent.InitializeComponent() 가 호출된 이후에 이 함수를 호출하세요"));
		return;
	}

	// {
	// 	const TArray<float> MaxHPs = IGameplayAbilitiesModule::Get()
	// 	                             .GetAbilitySystemGlobals()
	// 	                             ->GetAttributeSetInitter()
	// 	                             ->GetAttributeSetValues(UUPFCharacterStatSet::StaticClass(), UUPFCharacterStatSet::GetMaxHPAttribute().GetUProperty(), GroupName);
	// 	TArray<FString> MaxHPStrs;
	// 	Algo::Transform(MaxHPs, MaxHPStrs, [](float x){return FString::Printf(TEXT("%f"), x);});
	// 	UE_LOG(LogTemp, Log, TEXT("MaxHPs: %s"), *FString::Join(MaxHPStrs, TEXT(" ")));
	// }
	

	IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals()->GetAttributeSetInitter()->InitAttributeSetDefaults(ASC, GroupName, Level, true);

	// UE_LOG(LogTemp, Log, TEXT("%s"), *StatSet->ToString());
}
