// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemInstance/Equipments/UPFWeaponInstance.h"

#include "Constants/UPFSocketNames.h"

AUPFWeaponInstance::AUPFWeaponInstance()
{
	bReplicates = true;
}

void AUPFWeaponInstance::PostEquipped(USkeletalMeshComponent* AttachedMesh, const FName& AttachSocket)
{
	Super::PostEquipped(AttachedMesh, AttachSocket);

	OwnerMesh = AttachedMesh;
	AttachedSocketNameCache = AttachSocket;

	if (AttachSocket == UPFSocketNames::hand_rSocket)
	{
		LinkAnimLayer();
	}
}

void AUPFWeaponInstance::PreUnEquipped()
{
	Super::PreUnEquipped();

	if (IsAnimLayerLinked)
	{
		UnLinkAnimLayer();
	}

	AttachedSocketNameCache = FName(NAME_None);
}

void AUPFWeaponInstance::OnSocketChanged(const FName& NewSocketName)
{
	Super::OnSocketChanged(NewSocketName);
	
	if (AttachedSocketNameCache == NewSocketName) return;

	// hand -> none or back
	if (AttachedSocketNameCache == UPFSocketNames::hand_rSocket)
	{
		UnLinkAnimLayer();
	}
	// none or back -> hand
	else if (NewSocketName == UPFSocketNames::hand_rSocket)
	{
		LinkAnimLayer();
	}

	AttachedSocketNameCache = NewSocketName;
}

void AUPFWeaponInstance::LinkAnimLayer()
{
	if (!AnimLayer) return;
	if (!ensure(OwnerMesh)) return;
	if (!ensure(!IsAnimLayerLinked)) return;
	IsAnimLayerLinked = true;
	
	OwnerMesh->LinkAnimClassLayers(AnimLayer);
}

void AUPFWeaponInstance::UnLinkAnimLayer()
{
	if (!AnimLayer) return;
	if (!ensure(OwnerMesh)) return;
	if (!ensure(IsAnimLayerLinked)) return;
	IsAnimLayerLinked = false;

	OwnerMesh->UnlinkAnimClassLayers(AnimLayer);
}