// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UPFHUDWidget.h"

#include "UPFHPBarWidget.h"
#include "Character/UPFCharacterBase.h"

void UUPFHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HPBarWidget = Cast<UUPFHPBarWidget>(GetWidgetFromName(TEXT("WBP_UPFHPBar")));
	if (ensure(HPBarWidget))
	{
		const AUPFCharacterBase* MyCharacter = Cast<AUPFCharacterBase>(GetOwningPlayerPawn());
		if (ensure(MyCharacter))
		{
			HPBarWidget->SetData(MyCharacter->GetHPSet());
		}
	}
}
