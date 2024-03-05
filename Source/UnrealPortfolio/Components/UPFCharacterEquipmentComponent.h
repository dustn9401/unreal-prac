// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Ability/UPFAbilitySet.h"
#include "Components/ActorComponent.h"
#include "Item/UPFEquipmentItemData.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "UPFCharacterEquipmentComponent.generated.h"


class UUPFEquipmentInstanceWrapper;
class AUPFWeaponInstance;
class UUPFCharacterEquipmentComponent;
class AUPFEquipmentInstance;
class UUPFEquipmentItemData;
struct FUPFAppliedEquipmentEntry;
struct FUPFAppliedEquipmentArray;

/*
 * 캐릭터가 착용충인 하나의 장비 아이템에 대한 데이터
 */
USTRUCT()
struct FUPFAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FUPFAppliedEquipmentEntry() {}

private:
	friend UUPFCharacterEquipmentComponent;
	friend FUPFAppliedEquipmentArray;
	
	UPROPERTY()
	TObjectPtr<const UUPFEquipmentItemData> EquipmentItemData;
	
	UPROPERTY()
	TObjectPtr<AUPFEquipmentInstance> EquipmentInstance;

	// 부여된 어빌리티 및 이펙트 데이터, 서버만 데이터가 채워져 있다.
	UPROPERTY(NotReplicated)
	FUPFGrantedAbilitySetData GrantedData;

public:
	// 캐릭터가 장비를 보유 중이어도, 수납된 상태인 경우 등 어빌리티가 지급되지 않은 상태일 수도 있음.  그걸 확인하는 용도의 함수
	bool IsAbilityGranted() const
	{
		return !GrantedData.IsEmpty();
	}
};

USTRUCT()
struct FUPFAppliedEquipmentArray : public FFastArraySerializer
{
	GENERATED_BODY()

private:
	friend UUPFCharacterEquipmentComponent;
	friend FUPFAppliedEquipmentEntry;
	
	UPROPERTY()
	TArray<FUPFAppliedEquipmentEntry> Items;

	UPROPERTY()
	TObjectPtr<UUPFCharacterEquipmentComponent> EquipmentComponent;

public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FUPFAppliedEquipmentEntry, FUPFAppliedEquipmentArray>(Items, DeltaParms, *this);
	}

	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
};

template<>
struct TStructOpsTypeTraits< FUPFAppliedEquipmentArray > : public TStructOpsTypeTraitsBase2< FUPFAppliedEquipmentArray >
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};

/*
 * 캐릭터의 장비 아이템을 관리하는 컴포넌트
 * 하는일: 장비 아이템의 데이터 관리, 장비 아이템 액터 관리, 캐릭터 메시에 장비 아이템 탈부착
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

protected:
	AController* GetOwnerController() const;
	bool HasAuthority() const;

// 장비의 장착 / 해제
public:
	// 장비 장착 시킬때 호출하는 함수.
	void EquipItem(const UUPFEquipmentItemData* Data);
	
	// 특정 타입의 장비를 해제한다.
	void UnEquipItem(FGameplayTag EquipmentType);

	// can be null
	AUPFWeaponInstance* GetCurrentRangedWeaponInstance();
	
protected:
	friend FUPFAppliedEquipmentArray;
	
	// 캐릭터에게 장비 스폰 및 장착
	void EquipItemServerOnly(const UUPFEquipmentItemData* Data);

	AUPFEquipmentInstance* SpawnAndAttachEquipment(const UUPFEquipmentItemData* Data, FName AttachSocketName) const;

	// 캐릭터의 장비 해제 및 파괴
	void UnEquipItemServerOnly(FGameplayTag EquipmentType);

	// 현재 보유중인 EquipmentType 장비의 어빌리티 셋을 캐릭터에 부여한다.
	void GiveEquipmentAbility(const IAbilitySystemInterface* ASCInterface, const UUPFAbilitySet* AbilitySet, FGameplayTag EquipmentType);

	// 현재 보유중인 EquipmentType 장비의 어빌리티 셋을 캐릭터에서 제거한다.
	void TakeEquipmentAbility(const IAbilitySystemInterface* ASCInterface, FGameplayTag EquipmentType);

	// 로컬 컨트롤로만 사용하는 변수, 장비로 인해 바인딩 된 인풋 ID 저장용
	TMap<FGameplayTag, FGuid> InputBindIDs;

// Holster
public:
	// 현재 캐릭터가 Holster 액션을 수행 가능한 상태인지 여부
	bool CanToggleHolster();
	
	// 무기를 손에 들거나 수납한다.
	UFUNCTION(BlueprintCallable)
	void ToggleHolsterWeapon();

protected:
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CharacterMeshComponent;
	
	// 현재 착용중인 장비 목록
	UPROPERTY(Replicated)
	FUPFAppliedEquipmentArray AppliedEquipmentArray;

	// 현재 선택된 무기 타입 (근접무기/라이플/권총 등)
	UPROPERTY(Replicated)
	FGameplayTag CurrentWeaponType;

	// 무기를 수납중인지 여부
	UPROPERTY()
	uint8 bIsHolstered : 1;

	void ToggleHolsterWeaponInternal();

	UFUNCTION(Server, Reliable)
	void ServerRPCToggleHolsterWeapon(UUPFCharacterEquipmentComponent* TargetEquipmentComp);
	
	FUPFAppliedEquipmentEntry* FindEquipment(FGameplayTag WeaponType);

	void UpdateWeaponState();

public:
	FORCEINLINE bool GetIsHolstered() const
	{
		return bIsHolstered;
	}
};
