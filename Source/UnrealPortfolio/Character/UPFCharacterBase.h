// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "..\Interface\MeleeAttackAnimationInterface.h"
#include "AbilitySystemInterface.h"
#include "Ability/Attributes/UPFStatSet.h"
#include "Components/UPFAbilitySystemComponent.h"
#include "Components/WidgetComponent.h"
#include "DataAssets/UPFCharacterData.h"
#include "UPFCharacterBase.generated.h"

/**
 * 모든 캐릭터들의 base class
 */

class UUPFHPSet;
class UUPFStatSet;
class UTimelineComponent;
class UUPFCharacterEquipmentComponent;
class UUPFEquipmentItemData;

USTRUCT()
struct FReplicateTestStruct
{
	GENERATED_BODY()

public:
	FReplicateTestStruct(): Integer(0)
	{
	}

	UPROPERTY()
	int32 Integer;

	UPROPERTY()
	TArray<int32> Integers;
};

UCLASS(Abstract)
class UNREALPORTFOLIO_API AUPFCharacterBase : public ACharacter,
public IMeleeAttackAnimationInterface,
public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUPFCharacterBase(const FObjectInitializer& ObjectInitializer);
	
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;
	
// ACharacter overrides
protected:
	virtual bool CanCrouch() const override;

// Character Data
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UUPFCharacterData> CharacterData;

	void SetData_ServerOnly(UUPFCharacterData* InData);

	
// IAttackAnimationInterface Impl
public:
	virtual void OnMeleeAttackAnimationHit() override;

// IAbilitySystemInterface Impl
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

// Ability
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TObjectPtr<UUPFAbilitySystemComponent> AbilitySystemComponent;		// 이 캐릭터가 보유한 어빌리티를 관리할 컴포넌트

	
	// Stat
	virtual FName GetStatGroup() {return FName("Default");}

	

public:
	UUPFHPSet* GetHPSet() const;

// Widget
protected:
	// HP bar 표시용 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widget")
	TObjectPtr<UWidgetComponent> HPBarWidgetComp;

// Death
private:
	void OnHPZero();

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> DeadMontage;
	
	virtual void OnDeathStart() {}
	
	// 데미지를 받아 캐릭터의 체력이 0이 되었을 때 호출되는 함수. 여기서 반드시 FinishDeath 를 호출할 것
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnDeathStartCpp")
	void K2_OnDeathStart();
	
	UFUNCTION(BlueprintCallable)
	virtual void FinishDeath();

// Equipment
public:
	UPROPERTY()
	TObjectPtr<UUPFCharacterEquipmentComponent> EquipmentComponent;

// Aiming
public:
	UFUNCTION(BlueprintCallable)
	void OnAimingStart();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnAimingStartCpp")
	void K2_OnAimingStart();

	UFUNCTION(BlueprintCallable)
	void OnAimingEnd();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnAimingEndCpp")
	void K2_OnAimingEnd();
	
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bIsAiming : 1;

// test
public:
	UPROPERTY(Replicated)
	TObjectPtr<AActor> ReplicatedSubObject;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ReplicateTestSpawnActor;


	UPROPERTY(ReplicatedUsing=OnRep_TestStruct)
	FReplicateTestStruct TestStruct;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ModifyReplicationTestProps(bool IsStart);

	UFUNCTION()
	void OnRep_TestStruct();
};
