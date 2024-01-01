// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Ability/Attributes/UPFAttributeSet.h"
#include "UPFAmmoSet.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangedDelegate, float /*MagazineAmmo*/, float /*ExtraAmmo*/)

/**
 * 
 */
UCLASS()
class UNREALPORTFOLIO_API UUPFAmmoSet : public UUPFAttributeSet
{
	GENERATED_BODY()


public:
	UUPFAmmoSet();
	
	ATTRIBUTE_ACCESSORS(UUPFAmmoSet, MagazineAmmo);
	ATTRIBUTE_ACCESSORS(UUPFAmmoSet, ExtraAmmo);
	ATTRIBUTE_ACCESSORS(UUPFAmmoSet, MagazineSize);
	ATTRIBUTE_ACCESSORS(UUPFAmmoSet, MaxExtraAmmo);

	// 서버 Delegate 호출 용도로 override
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	FOnAmmoChangedDelegate OnAmmoChanged;

private:
	// 현재 탄창에 남은 탄약 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_MagazineAmmo, meta=(AllowPrivateAccess=true))
	FGameplayAttributeData MagazineAmmo;

	// 여분 탄약 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_ExtraAmmo, meta=(AllowPrivateAccess=true))
	FGameplayAttributeData ExtraAmmo;
	
	// 탄창크기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FGameplayAttributeData MagazineSize;

	// 소지 가능한 최대 여분 탄약 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FGameplayAttributeData MaxExtraAmmo;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_MagazineAmmo(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ExtraAmmo(const FGameplayAttributeData& OldValue);

	void BroadcastAmmoChangedDelegate();
};
