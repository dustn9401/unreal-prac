// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/UPFItemContainerBase.h"

#include "UnrealPortfolio.h"
#include "Components/BoxComponent.h"
#include "Engine/AssetManager.h"
#include "Interface/UPFItemContainerInterface.h"
#include "ItemInstance/Equipments/UPFEquipmentInstance.h"
#include "Particles/ParticleSystemComponent.h"
#include "Physics/UPFCollision.h"

TArray<FPrimaryAssetId> AUPFItemContainerBase::AssetIds;

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

void AUPFItemContainerBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AUPFItemContainerBase::OnOverlapBegin);
}

void AUPFItemContainerBase::BeginPlay()
{
	Super::BeginPlay();

	if (ItemDataPath)
	{
		// ItemData값이 미리 지정된 경우, 즉시 세팅한다.
		SetData(ItemDataPath.LoadSynchronous());
	}
	else
	{
		const UAssetManager& AM = UAssetManager::Get();

		if (AssetIds.Num()==0)
		{
			const FPrimaryAssetType AssetType(TEXT("UPFItemData"));
			if (!ensure(AM.GetPrimaryAssetIdList(AssetType, AssetIds))) return;
		}

		const int32 RIdx = FMath::RandRange(0, AssetIds.Num() - 1);
		if (!ensure(AssetIds.IsValidIndex(RIdx))) return;
	
		const FSoftObjectPtr AssetPtr(AM.GetPrimaryAssetPath(AssetIds[RIdx]));
		UObject* LoadedAsset = AssetPtr.LoadSynchronous();
		SetData(CastChecked<UUPFItemData>(LoadedAsset));
	}
}

void AUPFItemContainerBase::SetData(UUPFItemData* InItemData)
{
	ItemData = InItemData;

	SpawnedItem = GetWorld()->SpawnActorDeferred<AUPFItemInstanceBase>(InItemData->InstanceClass, FTransform::Identity, this);
	if (!ensure(SpawnedItem)) return;

	// Mesh 적용 등 세팅
	SpawnedItem->SetData(InItemData);

	SpawnedItem->AttachToComponent(BoxComponent, FAttachmentTransformRules::KeepRelativeTransform);

	SpawnedItem->FinishSpawning(FTransform::Identity);

	SetActorEnableCollision(true);
}

void AUPFItemContainerBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                           const FHitResult& SweepHitResult)
{
	// SetActorEnableCollision(false);
	
	if (ItemData == nullptr)
	{
		Destroy();
		return;
	}

	if (IUPFItemContainerInterface* ItemReceiver = Cast<IUPFItemContainerInterface>(OtherActor))
	{
		ItemReceiver->TakeItem(ItemData);
	}
}

void AUPFItemContainerBase::PlayEffectAndDestroySelf()
{
	if (IsValid(Effect))
	{
		Effect->Activate(true);
		Effect->OnSystemFinished.Clear();
		Effect->OnSystemFinished.AddDynamic(this, &AUPFItemContainerBase::OnEffectFinished);
	}
	else
	{
		Destroy();
	}
}

void AUPFItemContainerBase::OnEffectFinished(UParticleSystemComponent* ParticleSystem)
{
	Destroy();
}

