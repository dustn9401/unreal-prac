// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemInstance/UPFItemInstanceBase.h"
#include "UPFEquipmentInstance.generated.h"

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
	virtual void SetData(const UUPFItemData* InData) override;
	
	// 이 장비가 착용된 후 호출됨, 착용한 Pawn에 대한 접근은 Owner를 사용.
	void OnEquipped();
};
