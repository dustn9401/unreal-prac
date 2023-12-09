// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UPFWeaponStateComponent.h"

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Player/UPFCharacterPlayer.h"

// Sets default values for this component's properties
UUPFWeaponStateComponent::UUPFWeaponStateComponent()
{
	SetIsReplicatedByDefault(true);
}

void UUPFWeaponStateComponent::AddUnconfirmedServerSideHitMarkers(const FGameplayAbilityTargetDataHandle& InTargetData, const TArray<FHitResult>& FoundHits)
{
	APlayerController* OwnerPC = Cast<APlayerController>(GetOwner());
	if (!ensure(OwnerPC)) return;
	
	FUPFServerSideHitMarkerBatch& NewUnconfirmedHitMarker = UnconfirmedServerSideHitMarkers.Emplace_GetRef(InTargetData.UniqueId);
	
	for (const FHitResult& Hit : FoundHits)
	{
		FVector2D HitScreenLocation;
		if (UGameplayStatics::ProjectWorldToScreen(OwnerPC, Hit.Location, /*out*/ HitScreenLocation, /*bPlayerViewportRelative=*/ false))
		{
			FUPFScreenSpaceHitLocation& Entry = NewUnconfirmedHitMarker.Markers.AddDefaulted_GetRef();
			Entry.Location = HitScreenLocation;
			Entry.bShowAsSuccess = ShouldShowHitAsSuccess(Hit);

			// Determine the hit zone
			// if (const UPhysicalMaterialWithTags* PhysMatWithTags = Cast<const UPhysicalMaterialWithTags>(Hit.PhysMaterial.Get()))
			// {
			// 	for (const FGameplayTag MaterialTag : PhysMatWithTags->Tags)
			// 	{
			// 		if (MaterialTag.MatchesTag(TAG_Gameplay_Zone))
			// 		{
			// 			Entry.HitZone = MaterialTag;
			// 			break;
			// 		}
			// 	}
			// }
		}
	}
}

void UUPFWeaponStateComponent::UpdateDamageInstigatedTime(const FGameplayEffectContextHandle& EffectContext)
{
	if (ShouldUpdateDamageInstigatedTime(EffectContext))
	{
		ActuallyUpdateDamageInstigatedTime();
	}
}

double UUPFWeaponStateComponent::GetTimeSinceLastHitNotification() const
{
	UWorld* World = GetWorld();
	return World->TimeSince(LastWeaponDamageInstigatedTime);
}

bool UUPFWeaponStateComponent::ShouldShowHitAsSuccess(const FHitResult& Hit) const
{
	// todo
	return true;
}

bool UUPFWeaponStateComponent::ShouldUpdateDamageInstigatedTime(const FGameplayEffectContextHandle& EffectContext) const
{
	//@TODO: Implement me, for the purposes of this component we really only care about damage caused by a weapon
	// or projectile fired from a weapon, and should filter to that
	// (or perhaps see if the causer is also the source of our active reticle config)
	return EffectContext.GetEffectCauser() != nullptr;
}

void UUPFWeaponStateComponent::ActuallyUpdateDamageInstigatedTime()
{
	// If our LastWeaponDamageInstigatedTime was not very recent, clear our LastWeaponDamageScreenLocations array
	UWorld* World = GetWorld();
	if (World->GetTimeSeconds() - LastWeaponDamageInstigatedTime > 0.1)
	{
		LastWeaponDamageScreenLocations.Reset();
	}
	LastWeaponDamageInstigatedTime = World->GetTimeSeconds();
}

void UUPFWeaponStateComponent::ClientConfirmTargetData_Implementation(uint16 UniqueId, bool bSuccess, const TArray<uint8>& HitReplaces)
{
	for (int i = 0; i < UnconfirmedServerSideHitMarkers.Num(); i++)
	{
		FUPFServerSideHitMarkerBatch& Batch = UnconfirmedServerSideHitMarkers[i];
		if (Batch.UniqueId == UniqueId)
		{
			if (bSuccess && (HitReplaces.Num() != Batch.Markers.Num()))
			{
				UWorld* World = GetWorld();
				bool bFoundShowAsSuccessHit = false;

				int32 HitLocationIndex = 0;
				for (const FUPFScreenSpaceHitLocation& Entry : Batch.Markers)
				{
					if (!HitReplaces.Contains(HitLocationIndex) && Entry.bShowAsSuccess)
					{
						// Only need to do this once
						if (!bFoundShowAsSuccessHit)
						{
							ActuallyUpdateDamageInstigatedTime();
						}

						bFoundShowAsSuccessHit = true;

						LastWeaponDamageScreenLocations.Add(Entry);
					}
					++HitLocationIndex;
				}
			}

			UnconfirmedServerSideHitMarkers.RemoveAt(i);
			break;
		}
	}
}
