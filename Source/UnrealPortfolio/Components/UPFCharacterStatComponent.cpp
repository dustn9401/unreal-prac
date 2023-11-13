// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UPFCharacterStatComponent.h"

// Sets default values for this component's properties
UUPFCharacterStatComponent::UUPFCharacterStatComponent()
{
	bWantsInitializeComponent = true;

	BaseStat = CreateDefaultSubobject<UUPFCharacterStatSet>(TEXT("BaseStat"));
}

void UUPFCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	
}
