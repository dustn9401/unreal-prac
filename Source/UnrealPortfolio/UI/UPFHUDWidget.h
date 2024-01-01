// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPFHUDWidget.generated.h"

class UUPFHPBarWidget;
/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UUPFHPBarWidget> HPBarWidget;
};
