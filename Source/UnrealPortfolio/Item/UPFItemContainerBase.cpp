// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/UPFItemContainerBase.h"
#include "Components/BoxComponent.h"
#include "ItemInstance/Equipments/UPFEquipmentInstance.h"
#include "Physics/UPFCollision.h"

// Sets default values
AUPFItemContainerBase::AUPFItemContainerBase()
{
	// Box Trigger
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetCollisionProfileName(CPROFILE_UPFTRIGGER);
	BoxComponent->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	RootComponent = BoxComponent;
	
	bReplicates = true;
}

void AUPFItemContainerBase::SetData(UUPFItemData* InItemData)
{
	ItemData = InItemData;

	SpawnedItem = GetWorld()->SpawnActorDeferred<AUPFItemInstanceBase>(InItemData->InstanceClass, FTransform::Identity);
	if (!ensure(SpawnedItem)) return;

	// Mesh 적용 등 세팅
	SpawnedItem->SetData(InItemData);

	SpawnedItem->FinishSpawning(FTransform::Identity);
}

void AUPFItemContainerBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                           const FHitResult& SweepHitResult)
{
	SetActorEnableCollision(false);
	
	if (ItemData == nullptr)
	{
		Destroy();
		return;
	}

	
}

void AUPFItemContainerBase::OnEffectFinished(UParticleSystemComponent* ParticleSystem)
{
	Destroy();
}

