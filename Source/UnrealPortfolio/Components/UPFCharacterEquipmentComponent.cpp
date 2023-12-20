// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterEquipmentComponent.h"

#include "AbilitySystemComponent.h"
#include "Character/UPFCharacterBase.h"
#include "Constants/UPFSocketNames.h"
#include "Item/UPFEquipmentItemData.h"
#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"
#include "Player/UPFCharacterPlayer.h"
#include "Utility/UPFActorUtility.h"

// Sets default values for this component's properties
UUPFCharacterEquipmentComponent::UUPFCharacterEquipmentComponent()
{
	bWantsInitializeComponent = true;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> HolsterMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/UnrealPortfolio/Animation/AM_Holster.AM_Holster'"));
	if (HolsterMontageRef.Object)
	{
		HolsterMontage = HolsterMontageRef.Object;
	}

	SetIsReplicatedByDefault(true);

	bIsHolstered = true;

	// AddReplicatedSubObject 함수 사용 시 아래 세팅 필요
	bReplicateUsingRegisteredSubObjectList = true;
}

void UUPFCharacterEquipmentComponent::InitializeComponent()
{
	Super::InitializeComponent();

	CharacterMeshComponent = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();
}

AController* UUPFCharacterEquipmentComponent::GetOwnerController() const
{
	AActor* TestActor = GetOwner();
	while (TestActor)
	{
		if (AController* C = Cast<AController>(TestActor))
		{
			return C;
		}

		if (const APawn* Pawn = Cast<APawn>(TestActor))
		{
			return Pawn->GetController();
		}

		TestActor = TestActor->GetOwner();
	}

	return nullptr;
}

bool UUPFCharacterEquipmentComponent::HasAuthority() const
{
	const AActor* OwnerActor = GetOwner();
	return OwnerActor && OwnerActor->HasAuthority();
}

void UUPFCharacterEquipmentComponent::EquipItem(const UUPFEquipmentItemData* Data)
{
	if (!ensure(Data)) return;
	if (CurrentWeaponType.IsValid() && Data->EquipmentType == CurrentWeaponType)
	{
		FUPFAppliedEquipmentEntry* EntryPtr = FindEquipment(CurrentWeaponType);
		if (ensure(EntryPtr))
		{
			// 이미 똑같은 무기를 착용 중이면 리턴
			if (EntryPtr->EquipmentItemData == Data)
			{
				UPF_LOG_COMPONENT(LogTemp, Log, TEXT("Already Equipped!"));
				return;
			}
		}
	}
	
	// 장비 스폰 및 이 캐릭터에 장착시키고, 리스트에 추가
	EquipItemInternal(Data);

	// 추가된 AppliedEquipmentEntry 를 사용해서 어빌리티를 지급한다.
	if (const IAbilitySystemInterface* ASCInterface = UPFActorUtility::GetTypedOwnerRecursive<IAbilitySystemInterface>(GetOwner()))
	{
		GiveEquipmentAbility(ASCInterface, Data->AbilitiesToGrant, Data->EquipmentType);
	}
}

void UUPFCharacterEquipmentComponent::EquipItemInternal(const UUPFEquipmentItemData* Data)
{
	// 아이템 액터 생성
	AUPFEquipmentInstance* SpawnedItem = GetWorld()->SpawnActorDeferred<AUPFEquipmentInstance>(Data->InstanceClass, FTransform::Identity);
	if (!ensure(SpawnedItem)) return;

	// Mesh 적용 등 세팅
	SpawnedItem->SetData(Data);

	SpawnedItem->FinishSpawning(FTransform::Identity);


	// 소켓에 부착
	const bool IsUnarmed = !CurrentWeaponType.IsValid(); // 아무 장비도 착용하지 않았다면, 현재 선택된 장비 타입으로 지정하고 손에 쥐어준다.
	const bool IsCurrentWeaponType = CurrentWeaponType == Data->EquipmentType;
	const bool AttachToHand = (IsCurrentWeaponType && !bIsHolstered) || IsUnarmed;

	// todo: 데이터화 하기
	const FName AttachSocket = AttachToHand
		                           ? UPFSocketNames::hand_rSocket
		                           : UPFSocketNames::spine_03Socket;

	if (IsUnarmed)
	{
		bIsHolstered = false;
		CurrentWeaponType = Data->EquipmentType;
	}

	
	SpawnedItem->MeshComp->AttachToComponent(CharacterMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, AttachSocket);
	SpawnedItem->SetOwner(CharacterMeshComponent->GetOwner());
	SpawnedItem->PostEquipped(CharacterMeshComponent, AttachSocket);

	// 장비 목록에 추가
	FUPFAppliedEquipmentEntry NewEntry;
	NewEntry.EquipmentItemData = Data;
	NewEntry.EquipmentInstance = SpawnedItem;
	AppliedEquipments.Emplace(NewEntry);
}

void UUPFCharacterEquipmentComponent::UnEquipItem(FGameplayTag EquipmentType)
{
	// 어빌리티부터 제거
	if (const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		TakeEquipmentAbility(ASCInterface, EquipmentType);
	}

	// 그다음 이 캐릭터의 장비 제거
	UnEquipItemInternal(EquipmentType);
}

void UUPFCharacterEquipmentComponent::UnEquipItemInternal(FGameplayTag EquipmentType)
{
	for (auto EntryIt = AppliedEquipments.CreateIterator(); EntryIt; ++EntryIt)
	{
		FUPFAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.EquipmentItemData->EquipmentType != EquipmentType) continue;

		if (Entry.EquipmentInstance)
		{
			Entry.EquipmentInstance->DestroySelf();
		}

		EntryIt.RemoveCurrent();
	}

	// todo: 손에 들고있던 무기가 제거된 경우 처리
	// if (EquipmentType == CurrentWeaponType)
	// {
	// 	CurrentWeaponType = FGameplayTag::EmptyTag;
	// 	bIsHolstered = true;
	// }
}

void UUPFCharacterEquipmentComponent::GiveEquipmentAbility(const IAbilitySystemInterface* ASCInterface, const UUPFAbilitySet* AbilitySet, FGameplayTag EquipmentType)
{
	if (!ensure(AbilitySet)) return;

	// 서버: 어빌리티 지급
	if (HasAuthority())
	{
		FUPFAppliedEquipmentEntry* EntryPtr = FindEquipment(EquipmentType);
		check(EntryPtr && !EntryPtr->IsAbilityGranted()); // 어빌리티가 이미 지급된 상태면 중복 호출임
		AbilitySet->GiveToAbilityComp(ASCInterface, EntryPtr->EquipmentInstance, &EntryPtr->GrantedData);
	}

	// 로컬컨트롤러는 인풋 바인딩
	if (AUPFCharacterPlayer* PlayerCharacter = UPFActorUtility::GetTypedOwnerRecursive<AUPFCharacterPlayer>(GetOwner());
		PlayerCharacter->IsLocallyControlled())
	{
		const FGuid InputBindID = PlayerCharacter->BindAbilitySetInput(AbilitySet);
		InputBindIDs.Add(EquipmentType, InputBindID);
	}
}

void UUPFCharacterEquipmentComponent::TakeEquipmentAbility(const IAbilitySystemInterface* ASCInterface, FGameplayTag EquipmentType)
{
	// 서버: 어빌리티 제거
	if (HasAuthority())
	{
		FUPFAppliedEquipmentEntry* EntryPtr = FindEquipment(EquipmentType);
		if (!ensure(EntryPtr)) return;
		if (!EntryPtr->IsAbilityGranted()) return;
		EntryPtr->GrantedData.TakeFromCharacter(ASCInterface);
	}

	// 로컬컨트롤러는 인풋 바인딩 제거
	if (AUPFCharacterPlayer* PlayerCharacter = UPFActorUtility::GetTypedOwnerRecursive<AUPFCharacterPlayer>(GetOwner());
		PlayerCharacter->IsLocallyControlled())
	{
		const FGuid* InputID = InputBindIDs.Find(EquipmentType);
		if (!ensure(InputID)) return;
		PlayerCharacter->RemoveAbilitySetBind(*InputID);
	}
}

bool UUPFCharacterEquipmentComponent::CanToggleHolster()
{
	// 아무 장비도 착용하지 않음
	if (!CurrentWeaponType.IsValid())
	{
		return false;
	}

	const FUPFAppliedEquipmentEntry* EntryPtr = FindEquipment(CurrentWeaponType);
	if (!ensure(EntryPtr))
	{
		// 여기로 오면 안됨
		return false;
	}

	return true;
}

void UUPFCharacterEquipmentComponent::ToggleHolsterWeapon()
{
	// 아무 장비도 착용하지 않음
	if (!CurrentWeaponType.IsValid()) return;

	FUPFAppliedEquipmentEntry* EntryPtr = FindEquipment(CurrentWeaponType);
	if (!EntryPtr) return;

	// 부착할 캐릭터의 소켓를 구한다.
	const FName TargetSocket = bIsHolstered ? UPFSocketNames::hand_rSocket : UPFSocketNames::spine_03Socket;
	AUPFEquipmentInstance* EquipmentInstance = EntryPtr->EquipmentInstance;

	// Socket 이동
	EquipmentInstance->MeshComp->AttachToComponent(CharacterMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, TargetSocket);
	EquipmentInstance->OnSocketChanged(TargetSocket);

	bIsHolstered = !bIsHolstered;
	if (bIsHolstered)
	{
		// 수납 중 어빌리티를 제거한다.
		if (const IAbilitySystemInterface* ASCInterface = UPFActorUtility::GetTypedOwnerRecursive<IAbilitySystemInterface>(GetOwner()))
		{
			TakeEquipmentAbility(ASCInterface, CurrentWeaponType);
		}
	}
	else
	{
		// 어빌리티를 다시 지급힌다.
		if (const IAbilitySystemInterface* ASCInterface = UPFActorUtility::GetTypedOwnerRecursive<IAbilitySystemInterface>(GetOwner()))
		{
			GiveEquipmentAbility(ASCInterface, EntryPtr->EquipmentItemData->AbilitiesToGrant, CurrentWeaponType);
		}
	}
}

FUPFAppliedEquipmentEntry* UUPFCharacterEquipmentComponent::FindEquipment(FGameplayTag WeaponType)
{
	return AppliedEquipments.FindByPredicate([&WeaponType](const FUPFAppliedEquipmentEntry& X)
	{
		return X.EquipmentItemData->EquipmentType == WeaponType;
	});
}
