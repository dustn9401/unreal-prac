// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"

#include "Item/UPFEquipmentItemData.h"

AUPFEquipmentInstance::AUPFEquipmentInstance()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
}

void AUPFEquipmentInstance::OnEquipped()
{
	check(GetOwner());
}
