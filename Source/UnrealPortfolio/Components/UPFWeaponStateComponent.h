// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "UPFWeaponStateComponent.generated.h"

struct FGameplayEffectContextHandle;
struct FGameplayAbilityTargetDataHandle;
// Hit markers are shown for ranged weapon impacts in the reticle
// A 'successful' hit marker is shown for impacts that damaged an enemy
struct FUPFScreenSpaceHitLocation
{
	/** Hit location in viewport screenspace */
	FVector2D Location;	
	FGameplayTag HitZone;
	bool bShowAsSuccess = false;
};

struct FUPFServerSideHitMarkerBatch
{
	FUPFServerSideHitMarkerBatch() { }

	FUPFServerSideHitMarkerBatch(uint8 InUniqueId) :
		UniqueId(InUniqueId)
	{ }

	TArray<FUPFScreenSpaceHitLocation> Markers;

	uint8 UniqueId = 0;
};

/*
 * 플레이어 화면에 데미지 효과 적용을 위한 컴포넌트
 * 컨트롤러가 소유한다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALPORTFOLIO_API UUPFWeaponStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUPFWeaponStateComponent();

	UFUNCTION(Client, Reliable)
	void ClientConfirmTargetData(uint16 UniqueId, bool bSuccess, const TArray<uint8>& HitReplaces);

	void AddUnconfirmedServerSideHitMarkers(const FGameplayAbilityTargetDataHandle& InTargetData, const TArray<FHitResult>& FoundHits);

	/** Updates this player's last damage instigated time */
	void UpdateDamageInstigatedTime(const FGameplayEffectContextHandle& EffectContext);

	/** Gets the array of most recent locations this player instigated damage, in screen-space */
	void GetLastWeaponDamageScreenLocations(TArray<FUPFScreenSpaceHitLocation>& WeaponDamageScreenLocations) const
	{
		WeaponDamageScreenLocations = LastWeaponDamageScreenLocations;
	}

	/** Returns the elapsed time since the last (outgoing) damage hit notification occurred */
	double GetTimeSinceLastHitNotification() const;

	int32 GetUnconfirmedServerSideHitMarkerCount() const
	{
		return UnconfirmedServerSideHitMarkers.Num();
	}

protected:
	// This is called to filter hit results to determine whether they should be considered as a successful hit or not
	// The default behavior is to treat it as a success if being done to a team actor that belongs to a different team
	// to the owning controller's pawn
	virtual bool ShouldShowHitAsSuccess(const FHitResult& Hit) const;

	virtual bool ShouldUpdateDamageInstigatedTime(const FGameplayEffectContextHandle& EffectContext) const;

	void ActuallyUpdateDamageInstigatedTime();

private:
	/** Last time this controller instigated weapon damage */
	double LastWeaponDamageInstigatedTime = 0.0;

	/** Screen-space locations of our most recently instigated weapon damage (the confirmed hits) */
	TArray<FUPFScreenSpaceHitLocation> LastWeaponDamageScreenLocations;

	/** The unconfirmed hits */
	TArray<FUPFServerSideHitMarkerBatch> UnconfirmedServerSideHitMarkers;
};
