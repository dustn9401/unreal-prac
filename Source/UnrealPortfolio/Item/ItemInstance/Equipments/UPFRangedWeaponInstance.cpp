// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemInstance/Equipments/UPFRangedWeaponInstance.h"

#include "UPFGameplayTags.h"
#include "Character/UPFCharacterBase.h"
#include "Constants/UPFSocketNames.h"

void AUPFRangedWeaponInstance::SetData(const UUPFItemData* InData)
{
	Super::SetData(InData);

	// TODO: cache FireDelay
}

void AUPFRangedWeaponInstance::PostEquipped(USkeletalMeshComponent* AttachedMesh, const FName& AttachSocket)
{
	Super::PostEquipped(AttachedMesh, AttachSocket);

	OwnerMesh = AttachedMesh;
	AttachedSocketNameCache = AttachSocket;

	if (AttachSocket == UPFSocketNames::hand_rSocket)
	{
		LinkAnimLayer();
	}
}

void AUPFRangedWeaponInstance::PreUnEquipped()
{
	Super::PreUnEquipped();

	if (IsAnimLayerLinked)
	{
		UnLinkAnimLayer();
	}

	AttachedSocketNameCache = FName(NAME_None);
}

void AUPFRangedWeaponInstance::OnSocketChanged(const FName& NewSocketName)
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

void AUPFRangedWeaponInstance::LinkAnimLayer()
{
	if (!ensure(OwnerMesh)) return;
	if (!ensure(!IsAnimLayerLinked)) return;
	IsAnimLayerLinked = true;
	
	OwnerMesh->LinkAnimClassLayers(AnimLayer);
}

void AUPFRangedWeaponInstance::UnLinkAnimLayer()
{
	if (!ensure(OwnerMesh)) return;
	if (!ensure(IsAnimLayerLinked)) return;
	IsAnimLayerLinked = false;

	OwnerMesh->UnlinkAnimClassLayers(AnimLayer);
}

void AUPFRangedWeaponInstance::AddSpread()
{
	// Sample the heat up curve
	const float HeatPerShot = HeatToHeatPerShotCurve.GetRichCurveConst()->Eval(CurrentHeat);
	CurrentHeat = ClampHeat(CurrentHeat + HeatPerShot);

	// Map the heat to the spread angle
	CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);
}

void AUPFRangedWeaponInstance::ComputeSpreadRange(float& MinSpread, float& MaxSpread)
{
	HeatToSpreadCurve.GetRichCurveConst()->GetValueRange(/*out*/ MinSpread, /*out*/ MaxSpread);
}

void AUPFRangedWeaponInstance::ComputeHeatRange(float& MinHeat, float& MaxHeat)
{
	float Min1;
	float Max1;
	HeatToHeatPerShotCurve.GetRichCurveConst()->GetTimeRange(/*out*/ Min1, /*out*/ Max1);

	float Min2;
	float Max2;
	HeatToCoolDownPerSecondCurve.GetRichCurveConst()->GetTimeRange(/*out*/ Min2, /*out*/ Max2);

	float Min3;
	float Max3;
	HeatToSpreadCurve.GetRichCurveConst()->GetTimeRange(/*out*/ Min3, /*out*/ Max3);

	MinHeat = FMath::Min(FMath::Min(Min1, Min2), Min3);
	MaxHeat = FMath::Max(FMath::Max(Max1, Max2), Max3);
}

void AUPFRangedWeaponInstance::OnFire()
{
	K2_OnFire();
}

