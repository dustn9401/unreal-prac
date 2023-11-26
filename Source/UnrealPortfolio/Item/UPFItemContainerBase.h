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
 */
UCLASS(Abstract)
class UNREALPORTFOLIO_API AUPFItemContainerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUPFItemContainerBase();

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UUPFItemData> ItemData;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AUPFItemInstanceBase> SpawnedItem;

public:
	// 특정 아이템으로 설정한다.
	void SetData(UUPFItemData* InItemData);
	
	FORCEINLINE TObjectPtr<UBoxComponent> GetBoxComponent() const
	{
		return BoxComponent;
	}

protected:
	UPROPERTY(VisibleAnywhere, Category=Box)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category=Effect)
	TObjectPtr<UParticleSystemComponent> Effect;	// 획득 시 출력할 이펙트

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	UFUNCTION()
	void OnEffectFinished(UParticleSystemComponent* ParticleSystem);
};
