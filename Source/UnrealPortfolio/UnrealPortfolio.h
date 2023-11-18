// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define LOG_LOCAL_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetLocalRole()))
#define LOG_REMOTE_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetRemoteRole()))
#define LOG_NET_MODE_INFO (GetNetMode() == ENetMode::NM_Client ? *FString::Printf(TEXT("CLIENT_%d"), GPlayInEditorID) : GetNetMode() == ENetMode::NM_Standalone ? TEXT("STANDALONE") : TEXT("SERVER"))

#define LOG_SUB_LOCAL_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetLocalRole()))
#define LOG_SUB_REMOTE_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetRemoteRole()))

#define LOG_ATTRIBUTE_LOCAL_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwningActor()->GetLocalRole()))
#define LOG_ATTRIBUTE_REMOTE_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwningActor()->GetRemoteRole()))
#define LOG_NET_MODE_INFO_ATTRIBUTE (GetOwningActor()->GetNetMode() == ENetMode::NM_Client ? *FString::Printf(TEXT("CLIENT_%d"), GPlayInEditorID) : GetOwningActor()->GetNetMode() == ENetMode::NM_Standalone ? TEXT("STANDALONE") : TEXT("SERVER"))

#define UPF_LINE_LOG(Format, ...) UE_LOG(LogTemp, Log, TEXT("%s:%d %s"), *FString(__FUNCTION__), __LINE__, *FString::Printf(Format, ##__VA_ARGS__))
#define UPF_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), LOG_NET_MODE_INFO, LOG_LOCAL_ROLE_INFO, LOG_REMOTE_ROLE_INFO, *FString(__FUNCTION__), *FString::Printf(Format, ##__VA_ARGS__))
#define UPF_LOG_COMPONENT(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), LOG_NET_MODE_INFO, LOG_SUB_LOCAL_ROLE_INFO, LOG_SUB_REMOTE_ROLE_INFO, *FString(__FUNCTION__), *FString::Printf(Format, ##__VA_ARGS__))
#define UPF_LOG_ATTRIBUTE(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), LOG_NET_MODE_INFO_ATTRIBUTE, LOG_ATTRIBUTE_LOCAL_ROLE_INFO, LOG_ATTRIBUTE_REMOTE_ROLE_INFO, *FString(__FUNCTION__), *FString::Printf(Format, ##__VA_ARGS__))

UENUM(BlueprintType)
enum EUPFGameplayAbilityInputBinds : int
{
	None,
	Ability1				UMETA(DisplayName = "Ability1 (LMB)"),
	Ability2				UMETA(DisplayName = "Ability2 (RMB)"),
	Ability3				UMETA(DisplayName = "Ability3 (Q)"),
	Ability4				UMETA(DisplayName = "Ability4 (E)"),
	Ability5				UMETA(DisplayName = "Ability5 (R)"),
	Ability6				UMETA(DisplayName = "Ability6 (Shift)"),
	Ability7				UMETA(DisplayName = "Ability7 (Space)"),
	Ability8				UMETA(DisplayName = "Ability8 (B)"),
	Ability9				UMETA(DisplayName = "Ability9 (T)"),
};