// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UPFEnemySpawner.generated.h"

class AUPFCharacterNonPlayer;
/*
 * 레벨에 배치하는 적군 생성기
 */
UCLASS()
class UNREALPORTFOLIO_API AUPFEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUPFEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AUPFCharacterNonPlayer> EnemyClass;

	UFUNCTION()
	void OnSpawnActorDestroyed(AActor* DestroyedActor);
};
