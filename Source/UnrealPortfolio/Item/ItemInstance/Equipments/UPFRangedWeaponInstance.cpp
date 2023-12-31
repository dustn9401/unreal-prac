// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemInstance/Equipments/UPFRangedWeaponInstance.h"

#include "UPFGameplayTags.h"
#include "Character/UPFCharacterBase.h"
#include "Constants/UPFSocketNames.h"
#include "Interface/CameraShakeHandler.h"
#include "Net/UnrealNetwork.h"
#include "Utility/UPFActorUtility.h"

AUPFRangedWeaponInstance::AUPFRangedWeaponInstance()
{
	static ConstructorHelpers::FClassFinder<UCameraShakeBase> CameraShakeRef(TEXT("/Game/UnrealPortfolio/Blueprints/CameraShakes/CS_Weapon_Fire.CS_Weapon_Fire_C"));
	if (CameraShakeRef.Class)
	{
		CameraShakeClass = CameraShakeRef.Class;
	}
}

void AUPFRangedWeaponInstance::SetData(const UUPFItemData* InData)
{
	Super::SetData(InData);

	// TODO: cache FireDelay
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

void AUPFRangedWeaponInstance::OnFire(const FGameplayAbilityTargetDataHandle& InData)
{
	if (CameraShakeClass)
	{
		if (ICameraShakeHandler* CameraShakeHandler = UPFActorUtility::GetTypedOwnerRecursive<ICameraShakeHandler>(this))
		{
			CameraShakeHandler->PlayCameraShake(CameraShakeClass);
		}
	}
	
	K2_OnFire(InData);
}
