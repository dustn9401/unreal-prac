// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ItemInstance/UPFItemInstanceBase.h"
#include "UPFItemContainerBase.generated.h"

class UUPFItemData;
/*
 * 아이템 상자 등 아이템을 담고 있는 오브젝트의 베이스 클래스
 * 기본적으로 아이템 획득 시 사라지는 1회용이다.
 */
UCLASS(Abstract)
class UNREALPORTFOLIO_API AUPFItemContainerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUPFItemContainerBase();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UUPFItemData> ItemDataPath;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UUPFItemData> ItemData;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AUPFItemInstanceBase> SpawnedItem;

public:
	// 특정 아이템으로 설정한다.
	UFUNCTION(BlueprintCallable)
	void SetData(UUPFItemData* InItemData);
	
	FORCEINLINE TObjectPtr<UBoxComponent> GetBoxComponent() const
	{
		return BoxComponent;
	}

protected:
	static TArray<FPrimaryAssetId> AssetIds;
	
	UPROPERTY(VisibleAnywhere, Category=Box)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category=Effect)
	TObjectPtr<UParticleSystemComponent> Effect;	// 획득 시 출력할 이펙트

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	// 이펙트 출력 후 스스로를 파괴시킨다.
	UFUNCTION()
	void PlayEffectAndDestroySelf();

	UFUNCTION()
	void OnEffectFinished(UParticleSystemComponent* ParticleSystem);
};
