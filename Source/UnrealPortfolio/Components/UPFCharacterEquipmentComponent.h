// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UPFCharacterEquipmentComponent.generated.h"


class AUPFEquipmentInstance;
class UUPFEquipmentItemData;

/*
 * 장비 아이템이 착용될 소켓 enum
 */
UENUM()
enum EEquipmentSocketType
{
	RightHand,
	Back
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALPORTFOLIO_API UUPFCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUPFCharacterEquipmentComponent();

	virtual void InitializeComponent() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 장비를 장착한다. Socket에 이미 장비가 있으면, 교체한다.
	void EquipItem(const UUPFEquipmentItemData* Data);

	// Socket에 장비가 있다면, 해제한다.
	void UnequipItem(EEquipmentSocketType Socket);

protected:
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CharacterMeshComponent;
	
	// 소켓 enum의 실제 name 맵
	UPROPERTY()
	TMap<EEquipmentSocketType, FName> SocketNames;
	
	// 현재 착용중인 장비 목록
	UPROPERTY()
	TMap<EEquipmentSocketType, AUPFEquipmentInstance*> Equipments;
};
