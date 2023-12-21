// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UPFHPBarWidget.h"

#include "Ability/Attributes/UPFHPSet.h"

void UUPFHPBarWidget::SetData(const UUPFHPSet* StatSet)
{
	K2_UpdateHPBarInstant(StatSet->GetCurrentHP(), StatSet->GetMaxHP());
	StatSet->OnCurrentHPChanged.AddUObject(this, &UUPFHPBarWidget::K2_UpdateHPBarInstant);
	StatSet->OnMaxHPChanged.AddUObject(this, &UUPFHPBarWidget::K2_UpdateHPBarInstant);
}
