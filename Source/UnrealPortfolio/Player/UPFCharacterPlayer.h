// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
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

// Input Section
public:
	UFUNCTION(Client, Reliable)
	void ClientRPCBindAbilitySetInput(const UUPFAbilitySet* AbilitySet, int32 GrandIndex);

	UFUNCTION(Client, Reliable)
	void ClientRPCRemoveAbilitySetBind(int32 GrandIndex);

	UPROPERTY()
	TMap<int32, TArray<FInputBindingHandle>> AbilityInputBindingHandles;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CrouchAction;

	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);

// IUPFItemContainerInterface Impl
public:
	virtual void TakeItem(UUPFItemData* Data) override;
};
