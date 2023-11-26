// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/UPFItemContainerBase.h"
#include "Components/BoxComponent.h"
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

	// todo: EquipmentComponent의 아이템 스폰 코드를 적당한곳으로 옮겨서 같이 쓰기
}

void AUPFItemContainerBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                           const FHitResult& SweepHitResult)
{
	SetActorEnableCollision(false);
}

void AUPFItemContainerBase::OnEffectFinished(UParticleSystemComponent* ParticleSystem)
{
	Destroy();
}

