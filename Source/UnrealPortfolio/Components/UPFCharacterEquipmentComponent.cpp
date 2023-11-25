// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterEquipmentComponent.h"
#include "Item/UPFEquipmentItemData.h"
#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"

// Sets default values for this component's properties
UUPFCharacterEquipmentComponent::UUPFCharacterEquipmentComponent()
{
	bWantsInitializeComponent = true;
	
	SocketNames.Add(EEquipmentSocketType::RightHand, FName(TEXT("hand_rSocket")));
	SocketNames.Add(EEquipmentSocketType::Back, FName(TEXT("spine_03Socket")));
	
	SetIsReplicated(true);
}

void UUPFCharacterEquipmentComponent::InitializeComponent()
{
	Super::InitializeComponent();

	CharacterMeshComponent = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();
}

void UUPFCharacterEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UUPFCharacterEquipmentComponent::EquipItem(const UUPFEquipmentItemData* Data)
{
	if (!ensure(SocketNames.Contains(Data->AttachSocket))) return;
	
	AUPFEquipmentInstance* SpawnedItem = GetWorld()->SpawnActor<AUPFEquipmentInstance>(Data->InstanceClass);
	if (!ensure(SpawnedItem)) return;

	SpawnedItem->MeshComp->AttachToComponent(CharacterMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, SocketNames[Data->AttachSocket]);
	// SpawnedItem->MeshComp->SetupAttachment(CharacterMeshComponent, SocketNames[Data->AttachSocket]);	// 생성자용

	Equipments.Add(Data->AttachSocket, SpawnedItem);
}

void UUPFCharacterEquipmentComponent::UnequipItem(EEquipmentSocketType Socket)
{
}
