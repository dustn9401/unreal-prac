// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Item/UPFItemData.h"
#include "UPFEquipmentItemData.generated.h"

enum EEquipmentSocketType : int;
class AUPFEquipmentInstance;
class UUPFAbilitySet;
class UUPFGameplayAbility;

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFEquipmentItemData : public UUPFItemData
{
	GENERATED_BODY()

public:
	// 이 아이템의 Skeletal Mesh
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
	
	// 이 장비를 착용 시 부여할 어빌리티 (can be null)
	UPROPERTY(EditAnywhere)
	TObjectPtr<UUPFAbilitySet> AbilitiesToGrant;

	// 이 장비의 타입
	UPROPERTY(EditAnywhere)
	FGameplayTag EquipmentType;
};
