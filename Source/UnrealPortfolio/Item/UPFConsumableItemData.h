// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/UPFItemData.h"
#include "UPFConsumableItemData.generated.h"

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFConsumableItemData : public UUPFItemData
{
	GENERATED_BODY()

public:
	// 아이템 Mesh
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> StaticMesh;
};
