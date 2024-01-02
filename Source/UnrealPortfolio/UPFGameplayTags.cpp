#include "UPFGameplayTags.h"

namespace UPFGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_IsDead, "Ability.ActivateFail.IsDead", "Ability failed to activate because its owner is dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cooldown, "Ability.ActivateFail.Cooldown", "Ability failed to activate because it is on cool down.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cost, "Ability.ActivateFail.Cost", "Ability failed to activate because it did not pass the cost checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_TagsBlocked, "Ability.ActivateFail.TagsBlocked", "Ability failed to activate because tags are blocking it.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_TagsMissing, "Ability.ActivateFail.TagsMissing", "Ability failed to activate because tags are missing.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Networking, "Ability.ActivateFail.Networking", "Ability failed to activate because it did not pass the network checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_ActivationGroup, "Ability.ActivateFail.ActivationGroup", "Ability failed to activate because of its activation group.");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Active_Holster, "Ability.Active.Holster");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Active_RangedWeapon_ADS, "Ability.Active.RangedWeapon.ADS");

	// Items
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Equipment_Weapon_Melee, "Item.Equipment.Weapon.Melee", "근접 공격용 무기")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Equipment_Weapon_Range, "Item.Equipment.Weapon.Range", "원거리 공격용 무기")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Equipment_Weapon_Range_Rifle, "Item.Equipment.Weapon.Range.Rifle", "소총")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Equipment_Weapon_Range_Pistol, "Item.Equipment.Weapon.Range.Pistol", "권총")
}
