// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterEquipmentComponent.h"

#include "AbilitySystemComponent.h"
#include "UnrealPortfolio.h"
#include "UPFGameplayTags.h"
#include "Character/UPFCharacterBase.h"
#include "Constants/UPFSocketNames.h"
#include "Item/UPFEquipmentItemData.h"
#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"
#include "Net/UnrealNetwork.h"
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

	// 이게 true여야 AddReplicatedSubObject 로 추가한게 동작함
	bReplicateUsingRegisteredSubObjectList = true;
}

void UUPFCharacterEquipmentComponent::InitializeComponent()
{
	Super::InitializeComponent();

	CharacterMeshComponent = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();

	AppliedEquipments.SetOwnerComp(this);
}

void UUPFCharacterEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UUPFCharacterEquipmentComponent, AppliedEquipments);
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
	// 장비 스폰 및 이 캐릭터에 장착시키고, 리스트에 추가
	AUPFEquipmentInstance* SpawnedItem = EquipItemInternal(Data);

	// Replicated SubObject 추가
	if (SpawnedItem != nullptr)
	{
		AddReplicatedSubObject(SpawnedItem);
	}

	// 추가된 AppliedEquipmentEntry 를 사용해서 어빌리티를 지급한다.
	if (const IAbilitySystemInterface* ASCInterface = UPFActorUtility::GetTypedOwnerRecursive<IAbilitySystemInterface>(GetOwner()))
	{
		GiveEquipmentAbility(ASCInterface, Data->EquipmentType);
	}
}

AUPFEquipmentInstance* UUPFCharacterEquipmentComponent::EquipItemInternal(const UUPFEquipmentItemData* Data)
{
	// 아이템 액터 생성
	AUPFEquipmentInstance* SpawnedItem = GetWorld()->SpawnActorDeferred<AUPFEquipmentInstance>(Data->InstanceClass, FTransform::Identity);
	if (!ensure(SpawnedItem)) return nullptr;

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
	SpawnedItem->PostEquipped(CharacterMeshComponent, AttachSocket);

	// 장비 목록에 추가
	FUPFAppliedEquipmentEntry NewEntry;
	NewEntry.EquipmentItemData = Data;
	NewEntry.EquipmentInstance = SpawnedItem;
	AppliedEquipments.Items.Emplace(NewEntry);
	AppliedEquipments.MarkItemDirty(NewEntry);

	return SpawnedItem;
}

void UUPFCharacterEquipmentComponent::UnEquipItem(FGameplayTag EquipmentType)
{
	// 어빌리티부터 제거
	if (const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		TakeEquipmentAbility(ASCInterface, EquipmentType);
	}

	FUPFAppliedEquipmentEntry* RemovedEntry = FindEquipment(EquipmentType);
	if (RemovedEntry)
	{
		// Replicated SubObject 제거
		if (RemovedEntry->EquipmentInstance)
		{
			RemoveReplicatedSubObject(RemovedEntry->EquipmentInstance);
		}
	}

	// 그다음 이 캐릭터의 장비 제거
	UnEquipItemInternal(EquipmentType);
}

void UUPFCharacterEquipmentComponent::UnEquipItemInternal(FGameplayTag EquipmentType)
{
	for (auto EntryIt = AppliedEquipments.Items.CreateIterator(); EntryIt; ++EntryIt)
	{
		FUPFAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.EquipmentItemData->EquipmentType != EquipmentType) continue;

		if (Entry.EquipmentInstance)
		{
			Entry.EquipmentInstance->DestroySelf();
		}

		EntryIt.RemoveCurrent();
		AppliedEquipments.MarkArrayDirty();
	}

	// todo: 손에 들고있던 무기가 제거된 경우 처리
	// if (EquipmentType == CurrentWeaponType)
	// {
	// 	CurrentWeaponType = FGameplayTag::EmptyTag;
	// 	bIsHolstered = true;
	// }
}

void UUPFCharacterEquipmentComponent::GiveEquipmentAbility(const IAbilitySystemInterface* ASCInterface, FGameplayTag EquipmentType)
{
	FUPFAppliedEquipmentEntry* EntryPtr = FindEquipment(EquipmentType);
	if (!ensure(EntryPtr)) return;
	if (!ensure(EntryPtr->EquipmentItemData)) return;
	if (!EntryPtr->EquipmentItemData->AbilitiesToGrant) return;

	// 서버: 어빌리티 지급
	if (HasAuthority())
	{
		check(!EntryPtr->IsAbilityGranted()); // 어빌리티가 이미 지급된 상태면 중복 호출임
		EntryPtr->EquipmentItemData->AbilitiesToGrant->GiveToAbilityComp(ASCInterface, EntryPtr->EquipmentInstance, &EntryPtr->GrantedData);
	}

	// 로컬컨트롤러는 인풋 바인딩
	if (AUPFCharacterPlayer* PlayerCharacter = UPFActorUtility::GetTypedOwnerRecursive<AUPFCharacterPlayer>(GetOwner());
		PlayerCharacter->IsLocallyControlled())
	{
		EntryPtr->InputBindID = PlayerCharacter->BindAbilitySetInput(EntryPtr->EquipmentItemData->AbilitiesToGrant);
	}
}

void UUPFCharacterEquipmentComponent::TakeEquipmentAbility(const IAbilitySystemInterface* ASCInterface, FGameplayTag EquipmentType)
{
	FUPFAppliedEquipmentEntry* EntryPtr = FindEquipment(EquipmentType);
	if (!ensure(EntryPtr)) return;

	// 서버: 어빌리티 제거
	if (HasAuthority())
	{
		if (!EntryPtr->IsAbilityGranted()) return;
		EntryPtr->GrantedData.TakeFromCharacter(ASCInterface);
	}

	// 로컬컨트롤러는 인풋 바인딩 제거
	if (AUPFCharacterPlayer* PlayerCharacter = UPFActorUtility::GetTypedOwnerRecursive<AUPFCharacterPlayer>(GetOwner());
		PlayerCharacter->IsLocallyControlled())
	{
		PlayerCharacter->RemoveAbilitySetBind(EntryPtr->InputBindID);
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
			GiveEquipmentAbility(ASCInterface, CurrentWeaponType);
		}
	}
}

FUPFAppliedEquipmentEntry* UUPFCharacterEquipmentComponent::FindEquipment(FGameplayTag WeaponType)
{
	return AppliedEquipments.Items.FindByPredicate([&WeaponType](const FUPFAppliedEquipmentEntry& X)
	{
		return X.EquipmentItemData->EquipmentType == WeaponType;
	});
}
