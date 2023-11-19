// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Item/UPFItemData.h"
#include "UPFEquipmentItemData.generated.h"

class UUPFAbilitySet;
class UUPFGameplayAbility;
UNREALPORTFOLIO_API		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Weapon_Melee);
UNREALPORTFOLIO_API		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Equipment_Weapon_Range);

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFEquipmentItemData : public UUPFItemData
{
	GENERATED_BODY()

public:
	// 아이템 Mesh
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
	
	// 이 장비를 착용 시 부여할 어빌리티 (can be null)
	UPROPERTY(EditAnywhere)
	TObjectPtr<UUPFAbilitySet> AbilitiesToGrant;

	// 이 장비의 스텟 Set 그룹 이름
	UPROPERTY(EditAnywhere)
	FName AttributeSetGroupName;
};
