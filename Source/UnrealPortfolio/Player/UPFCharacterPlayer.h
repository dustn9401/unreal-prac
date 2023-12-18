// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Ability/UPFAbilitySet.h"
#include "Character/UPFCharacterBase.h"
#include "Interface/UPFItemContainerInterface.h"
#include "UPFCharacterPlayer.generated.h"

struct FInputBindingHandle;
struct FUPFGrantedAbilitySetData;
struct FUPFAbilityTriggerData;

/**
 * 플레이어용 캐릭터
 */
UCLASS()
class UNREALPORTFOLIO_API AUPFCharacterPlayer : public AUPFCharacterBase, public IUPFItemContainerInterface
{
	GENERATED_BODY()

public:
	AUPFCharacterPlayer(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void ApplyCharacterControlData(const class UUPFCharacterControlData* Data);

// Camera Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;	// 카메라 지지대
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CharacterControl)
	TObjectPtr<class UUPFCharacterControlData> CharacterControlData;

// Ability Input
public:
	// 어빌리티 셋의 인풋을 바인드 하고, 바인드 고유 ID를 반환
	FGuid BindAbilitySetInput(const UUPFAbilitySet* AbilitySet);

	// 바인드 시 반환했던 고유 ID 에 해당하는 인풋을 제거한다.
	void RemoveAbilitySetBind(FGuid GrantGuid);

protected:
	TMultiMap<FGuid, int32> AbilityInputBindingHandles;

// Native Input
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CrouchAction;

	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);

// IUPFItemContainerInterface Impl
public:
	virtual void TakeItem(UUPFItemData* Data) override;
};
