// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemInstance/UPFItemInstanceBase.h"
#include "UPFEquipmentInstance.generated.h"

class AUPFCharacterBase;
class UUPFEquipmentItemData;
/**
 * 폰이 착용하는 아이템, UUPFEquipmentItemData 와 일대일 대응
 */
UCLASS()
class UNREALPORTFOLIO_API AUPFEquipmentInstance : public AUPFItemInstanceBase
{
	GENERATED_BODY()

public:
	AUPFEquipmentInstance();

public:
	// ItemData 를 받아서 오브젝트를 초기화 시킨다.
	virtual void SetData(const UUPFItemData* InData) override;

	// 메시에 Attach 된 후 호출
	virtual void PostEquipped(USkeletalMeshComponent* AttachedMesh, const FName& AttachSocket);

	// 메시에서 Detach 되기 전 호출
	virtual void PreUnEquipped();

	// 소켓이 변경될 때 외부에서 호출하는 함수
	virtual void OnSocketChanged(const FName& NewSocketName);

	// 이 장비 인스턴스가 현재 누군가에게 착용되어 있는 상태인지 여부
	uint8 bIsEquipped : 1;
};
