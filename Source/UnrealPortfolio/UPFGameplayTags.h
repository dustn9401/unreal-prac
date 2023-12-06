﻿#pragma once

#include "NativeGameplayTags.h"

namespace UPFGameplayTags
{
	UNREALPORTFOLIO_API		UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability);
	UNREALPORTFOLIO_API		UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Melee);

	UNREALPORTFOLIO_API		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	UNREALPORTFOLIO_API		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	UNREALPORTFOLIO_API		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	UNREALPORTFOLIO_API		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	UNREALPORTFOLIO_API		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	UNREALPORTFOLIO_API		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
	UNREALPORTFOLIO_API		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);
	
	UNREALPORTFOLIO_API		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Active_Holster);
}
