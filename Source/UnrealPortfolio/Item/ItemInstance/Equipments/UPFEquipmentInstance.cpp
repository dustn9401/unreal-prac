// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"

#include "UnrealPortfolio.h"
#include "Item/UPFEquipmentItemData.h"

AUPFEquipmentInstance::AUPFEquipmentInstance()
{
	RootComponent = MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	bReplicates = true;
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

	SetActorRelativeTransform(FTransform::Identity);
}

void AUPFEquipmentInstance::PostEquipped()
{
	bIsEquipped = true;
}

void AUPFEquipmentInstance::PreUnEquipped()
{
	bIsEquipped = false;
}

void AUPFEquipmentInstance::OnSocketChanged(const FName& NewSocketName)
{
}

