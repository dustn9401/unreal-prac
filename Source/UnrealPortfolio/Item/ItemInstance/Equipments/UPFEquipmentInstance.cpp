// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"

#include "Item/UPFEquipmentItemData.h"

AUPFEquipmentInstance::AUPFEquipmentInstance()
{
	RootComponent = MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
}

void AUPFEquipmentInstance::SetData(const UUPFItemData* InData)
{
	Super::SetData(InData);

	USkeletalMeshComponent* SMC = Cast<USkeletalMeshComponent>(MeshComp);
	check(SMC);

	const UUPFEquipmentItemData* EquipmentItemData = Cast<UUPFEquipmentItemData>(InData);
	check(EquipmentItemData);
	
	USkeletalMesh* EquipmentMesh = EquipmentItemData->SkeletalMesh.IsPending()
									? EquipmentItemData->SkeletalMesh.LoadSynchronous()
									: EquipmentItemData->SkeletalMesh.Get();
	SMC->SetSkeletalMesh(EquipmentMesh);
}

void AUPFEquipmentInstance::OnEquipped(AUPFCharacterBase* EquippedCharacter)
{
}

void AUPFEquipmentInstance::OnUnEquipped(AUPFCharacterBase* UnEquippedCharacter)
{
}
