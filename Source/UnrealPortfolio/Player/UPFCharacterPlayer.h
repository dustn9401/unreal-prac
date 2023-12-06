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

	virtual void BeginDestroy() override;

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
	// 클라이언트 빙의 후 SetupPlayerInputComponent 함수에서 서버에 캐릭터의 기본 어빌리티를 요청하는 함수
	UFUNCTION(Server, Reliable)
	void ServerRPCGiveCharacterAbilitySet();
	
	// 서버에서 어빌리티 세트 부여 시 호출되는 함수, 인풋을 바인딩하고 핸들을 GrantKey 를 키로 맵에 저장한다.
	UFUNCTION(Client, Reliable)
	void ClientRPCBindAbilitySetInput(const UUPFAbilitySet* AbilitySet, int32 GrantKey);

	// 서버에서 어빌리티 세트 제거 시 호출되는 함수, GrantKey 에 대한 인풋 바인딩을 제거한다.
	UFUNCTION(Client, Reliable)
	void ClientRPCRemoveAbilitySetBind(int32 GrantKey);
	
	TMap<int32, TArray<int32>> AbilityInputBindingHandles;
	
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
