// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemInstance/Equipments/UPFWeaponInstance.h"

#include "UnrealPortfolio.h"
#include "Constants/UPFSocketNames.h"

AUPFWeaponInstance::AUPFWeaponInstance()
{
	
}

void AUPFWeaponInstance::OnRep_Owner()
{
	Super::OnRep_Owner();

	OnOwnerUpdated();
}

void AUPFWeaponInstance::OnOwnerUpdated()
{
	AActor* OwnerActor = GetOwner();
	if (OwnerActor)
	{
		OwnerMesh = OwnerActor->GetComponentByClass<USkeletalMeshComponent>();
		LinkAnimLayerIfPossible();
		UPF_LOG(LogTemp, Log, TEXT("Owner Name: %s"), *OwnerActor->GetName());
	}
	else
	{
		UPF_LOG(LogTemp, Log, TEXT("Owner is NULL"));
		UnLinkAnimLayerIfPossible();
	}
}

void AUPFWeaponInstance::OnSocketChanged()
{
	Super::OnSocketChanged();

	const FName AttachedSocketName = GetAttachParentSocketName();
	
	if (AttachedSocketName == UPFSocketNames::hand_rSocket)
	{
		LinkAnimLayerIfPossible();
	}
	else
	{
		UnLinkAnimLayerIfPossible();
	}
}

void AUPFWeaponInstance::LinkAnimLayerIfPossible()
{
	if (!AnimLayer) return;
	if (!OwnerMesh) return;
	if (IsAnimLayerLinked) return;
	IsAnimLayerLinked = true;
	
	OwnerMesh->LinkAnimClassLayers(AnimLayer);
}

void AUPFWeaponInstance::UnLinkAnimLayerIfPossible()
{
	if (!AnimLayer) return;
	if (!OwnerMesh) return;
	if (!IsAnimLayerLinked) return;
	IsAnimLayerLinked = false;

	OwnerMesh->UnlinkAnimClassLayers(AnimLayer);
}