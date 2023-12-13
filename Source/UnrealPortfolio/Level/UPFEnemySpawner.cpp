// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/UPFEnemySpawner.h"

#include "Ability/Attributes/UPFHPSet.h"
#include "Character/UPFCharacterNonPlayer.h"

// Sets default values
AUPFEnemySpawner::AUPFEnemySpawner()
{
	bReplicates = true;
}

// Called when the game starts or when spawned
void AUPFEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SpawnEnemy();
	}
}

void AUPFEnemySpawner::SpawnEnemy()
{
	if (!ensure(EnemyClass)) return;
	
	AUPFCharacterNonPlayer* SpawnedEnemy = GetWorld()->SpawnActor<AUPFCharacterNonPlayer>(EnemyClass, GetActorLocation(), GetActorRotation());
	SpawnedEnemy->OnDestroyed.AddDynamic(this, &AUPFEnemySpawner::OnSpawnActorDestroyed);
}

void AUPFEnemySpawner::OnSpawnActorDestroyed(AActor* DestroyedActor)
{
	UPF_LOG(LogTemp, Log, TEXT("Called"));
	DestroyedActor->OnDestroyed.RemoveDynamic(this, &AUPFEnemySpawner::OnSpawnActorDestroyed);
	
	if (HasAuthority())
	{
		SpawnEnemy();
	}
}
