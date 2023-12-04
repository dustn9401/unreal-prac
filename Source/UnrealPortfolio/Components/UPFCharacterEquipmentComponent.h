// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Ability/UPFAbilitySet.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "UPFCharacterEquipmentComponent.generated.h"


class UUPFCharacterEquipmentComponent;
class AUPFEquipmentInstance;
class UUPFEquipmentItemData;
struct FUPFEquipmentList;

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
struct FUPFAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

private:
	friend FUPFEquipmentList;
	friend UUPFCharacterEquipmentComponent;
	
	UPROPERTY()
	TObjectPtr<const UUPFEquipmentItemData> EquipmentItemData;

	UPROPERTY()
	TObjectPtr<AUPFEquipmentInstance> EquipmentInstance;

	// 서버만 가지고 있는 부여된 어빌리티 및 이펙트 데이터
	UPROPERTY(NotReplicated)
	FUPFGrantedAbilitySetData GrantedData;
};

USTRUCT(BlueprintType)
struct FUPFEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FUPFEquipmentList() {}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FUPFAppliedEquipmentEntry, FUPFEquipmentList>(Entries, DeltaParms, *this);
	}

private:
	friend UUPFCharacterEquipmentComponent;
	
	UPROPERTY()
	TArray<FUPFAppliedEquipmentEntry> Entries;
};

template<>
struct TStructOpsTypeTraits<FUPFEquipmentList> : public TStructOpsTypeTraitsBase2<FUPFEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
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

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:
	// 장비를 장착한다.
	void EquipItem(const UUPFEquipmentItemData* Data);

	// 특정 타입의 장비를 해제한다. 
	void UnEquipItem(FGameplayTag EquipmentType);

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

	// 현재 착용중인 장비 목록
	UPROPERTY(Replicated)
	FUPFEquipmentList EquipmentList;

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
	// 무기 수납 / 착용 시 공통으로 사용되는 몽타주, Holster Ability에 의해 Replicate 로 재생됨
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAnimMontage> HolsterMontage;
};
