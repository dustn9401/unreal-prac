// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UPFItemContainerInterface.generated.h"

class UUPFItemData;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUPFItemContainerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * ItemContainer와 상호작용 할 수 있는 Pawn에 구현하는 인터페이스
 */
class UNREALPORTFOLIO_API IUPFItemContainerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// ItemContainerBase에서 자신과 오버랩 된 액터를 대상으로 호출하는 함수
	virtual void TakeItem(UUPFItemData* Data) = 0;
};
