// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"
#include "UPFWeaponInstance.generated.h"

/**
 * Damage 를 주는 어빌리티가 있는 장비 아이템 종류
 */
UCLASS()
class UNREALPORTFOLIO_API AUPFWeaponInstance : public AUPFEquipmentInstance
{
	GENERATED_BODY()

public:
	AUPFWeaponInstance();

protected:
	virtual void OnRep_Owner() override;

public:
	virtual void OnOwnerUpdated() override;

public:
	// 소켓이 변경될 때 외부에서 호출하는 함수
	virtual void OnSocketChanged() override;


protected:
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> OwnerMesh;
	
	/* 
	 * ALI_WeaponLayer 구현한 애니메이션 인스턴스, 장비 착용 시 추가되고, 해제 시 다시 제거된다.
	 * 무기 착용 시 캐릭터 기본 애니메이션이 달라지는 경우 사용
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAnimInstance> AnimLayer;

	// OwnerMesh에 AnimLayer 를 등록 
	void LinkAnimLayerIfPossible();

	// OwnerMesh에서 AnimLayer 를 해제
	void UnLinkAnimLayerIfPossible();
	
	bool IsAnimLayerLinked = false;
};
