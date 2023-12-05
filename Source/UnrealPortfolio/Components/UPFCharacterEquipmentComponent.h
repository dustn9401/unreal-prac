// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Ability/UPFAbilitySet.h"
#include "Components/ActorComponent.h"
#include "UPFCharacterEquipmentComponent.generated.h"


class UUPFCharacterEquipmentComponent;
class AUPFEquipmentInstance;
class UUPFEquipmentItemData;

/*
 * 장비의 캐릭터 장착을 위한 데이터
 */
USTRUCT(BlueprintType)
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
 * 캐릭터가 착용충인 하나의 장비 아이템에 대한 데이터
 */
USTRUCT()
struct FUPFAppliedEquipmentEntry
{
	GENERATED_BODY()

private:
	friend UUPFCharacterEquipmentComponent;
	
	UPROPERTY()
	TObjectPtr<const UUPFEquipmentItemData> EquipmentItemData;

	// 각 플레이어가 관리
	UPROPERTY(NotReplicated)
	TObjectPtr<AUPFEquipmentInstance> EquipmentInstance;

	// 부여된 어빌리티 및 이펙트 데이터
	UPROPERTY(NotReplicated)
	FUPFGrantedAbilitySetData GrantedData;
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
	// 장비 장착 시킬때 호출하는 함수.
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCEquipItem(const UUPFEquipmentItemData* Data);

	// 모두에게 이 캐릭터에게 장비 장착 명령
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCEquipItem(const UUPFEquipmentItemData* Data);

	// 특정 타입의 장비를 해제한다.
	UFUNCTION(Server, Reliable)
	void ServerRPCUnEquipItem(FGameplayTag EquipmentType);

	// 모두에게 이 캐릭터의 장비 해제 명령
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCUnEquipItem(FGameplayTag EquipmentType);

	// 무기를 손에 들거나 수납한다.
	UFUNCTION(BlueprintCallable)
	void ToggleHolsterWeapon();

protected:
	// Data에 대한 장비 액터를 생성하고 Equipments 맵에 추가한다.
	void EquipItem(const UUPFEquipmentItemData* Data);

	// 특정 타입의 장비가 착용되어 있다면 제거한다.
	void UnEquipItem(FGameplayTag EquipmentType);
	
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CharacterMeshComponent;
	
	// 장비 타입 별 Socket 데이터
	UPROPERTY()
	TMap<FGameplayTag, FEquipmentSocketData> SocketDatas;

	// 현재 착용중인 장비 목록, EquipmentType 과 데이터 pair
	UPROPERTY()
	TMap<FGameplayTag, FUPFAppliedEquipmentEntry> Equipments;

	// 현재 선택된 무기 타입 (근접무기/라이플/권총 등)
	FGameplayTag CurrentWeaponType;

	// 무기를 수납중인지 여부
	uint8 bIsHolstered : 1;

public:
	FORCEINLINE bool GetIsHolstered() const
	{
		return bIsHolstered;
	}

	// Animation
protected:
	// 무기 수납 / 착용 시 공통으로 사용되는 몽타주, Holster Ability에 의해 Replicate 로 재생됨
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAnimMontage> HolsterMontage;
};
