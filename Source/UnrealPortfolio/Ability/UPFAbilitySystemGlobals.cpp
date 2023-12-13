// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/UPFAbilitySystemGlobals.h"

#include "Attributes/FUPFAttributeSetInitter.h"

void UUPFAbilitySystemGlobals::AllocAttributeSetInitter()
{
	GlobalAttributeSetInitter = MakeShared<FUPFAttributeSetInitter>();
}
