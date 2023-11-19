// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"

#include "Item/UPFEquipmentItemData.h"

AUPFEquipmentInstance::AUPFEquipmentInstance()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
}

void AUPFEquipmentInstance::SetData(UUPFEquipmentItemData* InData)
{
	// Mesh 세팅
	USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(MeshComp);
	if (ensure(SkeletalMeshComponent))
	{
		USkeletalMesh* Mesh = InData->SkeletalMesh.IsPending()
										? InData->SkeletalMesh.LoadSynchronous()
										: InData->SkeletalMesh.Get();
		SkeletalMeshComponent->SetSkeletalMesh(Mesh);
	}
}
