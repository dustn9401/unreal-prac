// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "UPFCharacterEquipmentComponent.generated.h"


class AUPFEquipmentInstance;
class UUPFEquipmentItemData;

/*
 * 장비의 캐릭터 장착을 위한 데이터
 */
USTRUCT()
struct FEquipmentSocketData
{
	GENERATED_BODY()

public:
	// 무기를 쥘 소켓
	UPROPERTY(EditAnywhere)
	FName HandSocket;

	// 무기를 수납할 소켓
	UPROPERTY(EditAnywhere)
	FName HolsterSocket;
};

/*
 * 캐릭터의 장비 아이템을 관리하는 컴포넌트
 */
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
	// 장비를 장착한다. 이미 같은 종류의 장비가 있으면, 교체한다.
	void EquipOrSwitchItem(const UUPFEquipmentItemData* Data);

	// 무기를 손에 들거나 수납한다.
	void ToggleHolsterWeapon();

	// 수납 애니메이션 콜백
	UFUNCTION(BlueprintCallable)
	void OnAnimNotifyHolster();

protected:
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CharacterMeshComponent;
	
	// 장비 타입 별 Socket 데이터
	UPROPERTY()
	TMap<FGameplayTag, FEquipmentSocketData> SocketDatas;
	
	// 현재 보유중인 장비 목록
	UPROPERTY(VisibleAnywhere)
	TMap<FGameplayTag, AUPFEquipmentInstance*> Equipments;

	// 현재 선택된 무기 타입 (근접무기/라이플/권총 등)
	FGameplayTag CurrentWeaponType;

	// 무기를 수납중인지 여부
	bool IsHolstered = true;

public:
	FORCEINLINE bool GetIsHolstered() const
	{
		return IsHolstered;
	}

	// Animation
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAnimMontage> HolsterMontage;
};
