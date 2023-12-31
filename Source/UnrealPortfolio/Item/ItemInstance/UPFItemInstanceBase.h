// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/UPFItemData.h"
#include "UPFItemInstanceBase.generated.h"

/*
 * 씬에 존재하는 아이템 Actor
 */
UCLASS(Abstract)
class UNREALPORTFOLIO_API AUPFItemInstanceBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUPFItemInstanceBase();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UMeshComponent> MeshComp;

	virtual void SetData(const UUPFItemData* InData) {}

	// 자기 자신을 파괴시킨다.
	virtual void DestroySelf();
};
