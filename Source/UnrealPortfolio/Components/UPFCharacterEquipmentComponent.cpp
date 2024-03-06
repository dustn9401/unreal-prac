// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterEquipmentComponent.h"

#include "AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "Character/UPFCharacterBase.h"
#include "Constants/UPFSocketNames.h"
#include "Item/UPFEquipmentItemData.h"
#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"
#include "Item/ItemInstance/Equipments/UPFWeaponInstance.h"
#include "Net/UnrealNetwork.h"
#include "Player/UPFCharacterPlayer.h"
#include "Utility/UPFActorUtility.h"

void FUPFAppliedEquipmentArray::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
	for(const int32 Index : RemovedIndices)
	{
		FUPFAppliedEquipmentEntry& Entry = Items[Index];
		if (Entry.EquipmentInstance)
		{
			Entry.EquipmentInstance->DestroySelf();
		}
	}
}

void FUPFAppliedEquipmentArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	for(const int32 Index : AddedIndices)
	{
		FUPFAppliedEquipmentEntry& Entry = Items[Index];
		if (Entry.EquipmentInstance)
		{
			Entry.EquipmentInstance->SetData(Entry.EquipmentItemData);
			Entry.EquipmentInstance->PostEquipped();
			EquipmentComponent->UpdateWeaponState();
		}
	}
}

// Sets default values for this component's properties
UUPFCharacterEquipmentComponent::UUPFCharacterEquipmentComponent()
{
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);

	bIsHolstered = true;
}

void UUPFCharacterEquipmentComponent::InitializeComponent()
{
	Super::InitializeComponent();

	CharacterMeshComponent = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();

	AppliedEquipmentArray.EquipmentComponent = this;
}

void UUPFCharacterEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UUPFCharacterEquipmentComponent, AppliedEquipmentArray);
	DOREPLIFETIME(UUPFCharacterEquipmentComponent, CurrentWeaponType);
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
	if (HasAuthority())
	{
		EquipItemServerOnly(Data);
	}
	
	// 추가된 AppliedEquipmentEntry 를 사용해서 어빌리티를 지급한다.
	if (const IAbilitySystemInterface* ASCInterface = UPFActorUtility::GetTypedOwnerRecursive<IAbilitySystemInterface>(GetOwner()))
	{
		GiveEquipmentAbility(ASCInterface, Data->AbilitiesToGrant, Data->EquipmentType);
	}
}

void UUPFCharacterEquipmentComponent::EquipItemServerOnly(const UUPFEquipmentItemData* Data)
{
	const bool IsUnarmed = !CurrentWeaponType.IsValid(); // 아무 장비도 착용하지 않았다면, 현재 선택된 장비 타입으로 지정하고 손에 쥐어준다.
	const bool IsCurrentWeaponType = CurrentWeaponType == Data->EquipmentType;
	const bool AttachToHand = (IsCurrentWeaponType && !bIsHolstered) || IsUnarmed;
	
	const FName AttachSocket = AttachToHand
		                           ? UPFSocketNames::hand_rSocket
		                           : UPFSocketNames::spine_03Socket;

	if (IsUnarmed)
	{
		bIsHolstered = false;
		CurrentWeaponType = Data->EquipmentType;
	}
	
	// 아이템 액터 생성
	AUPFEquipmentInstance* SpawnedItem = SpawnAndAttachEquipment(Data, AttachSocket);

	// 장비 목록에 추가
	if (HasAuthority())
	{
		FUPFAppliedEquipmentEntry NewEntry;
		NewEntry.EquipmentItemData = Data;
		NewEntry.EquipmentInstance = SpawnedItem;
		
		AppliedEquipmentArray.Items.Emplace(NewEntry);
		AppliedEquipmentArray.MarkItemDirty(NewEntry);
	}
	else
	{
		FUPFAppliedEquipmentEntry* Entry = FindEquipment(Data->EquipmentType);
		if (!ensure(Entry)) return;

		Entry->EquipmentInstance = SpawnedItem;
	}
}

AUPFEquipmentInstance* UUPFCharacterEquipmentComponent::SpawnAndAttachEquipment(const UUPFEquipmentItemData* Data, FName AttachSocketName) const
{
	UPF_LOG_COMPONENT(LogTemp, Log, TEXT("Owner Name = %s"), *GetOwner()->GetName());
	AUPFEquipmentInstance* SpawnedItem = GetWorld()->SpawnActorDeferred<AUPFEquipmentInstance>(Data->InstanceClass, FTransform::Identity, GetOwner());

	// Mesh 적용 등 세팅
	SpawnedItem->SetData(Data);

	SpawnedItem->FinishSpawning(FTransform::Identity);

	SpawnedItem->MeshComp->AttachToComponent(CharacterMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, AttachSocketName);
	SpawnedItem->SetOwner(GetOwner());
	
	SpawnedItem->PostEquipped();

	return SpawnedItem;
}

void UUPFCharacterEquipmentComponent::UnEquipItem(FGameplayTag EquipmentType)
{
	// 어빌리티부터 제거
	if (const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		TakeEquipmentAbility(ASCInterface, EquipmentType);
	}

	// 그다음 이 캐릭터의 장비 제거
	if (HasAuthority())
	{
		UnEquipItemServerOnly(EquipmentType);
	}
}

AUPFWeaponInstance* UUPFCharacterEquipmentComponent::GetCurrentRangedWeaponInstance()
{
	if (!CurrentWeaponType.IsValid()) return nullptr;
	
	FUPFAppliedEquipmentEntry* Entry = FindEquipment(CurrentWeaponType);
	if (!ensure(Entry)) return nullptr;

	return CastChecked<AUPFWeaponInstance>(Entry->EquipmentInstance);
}

void UUPFCharacterEquipmentComponent::UnEquipItemServerOnly(FGameplayTag EquipmentType)
{
	for (auto EntryIt = AppliedEquipmentArray.Items.CreateIterator(); EntryIt; ++EntryIt)
	{
		FUPFAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.EquipmentItemData->EquipmentType != EquipmentType) continue;

		if (Entry.EquipmentInstance)
		{
			Entry.EquipmentInstance->DestroySelf();
		}

		EntryIt.RemoveCurrent();
		AppliedEquipmentArray.MarkArrayDirty();
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
		EntryPtr->GrantedData.TakeFromAbilityComp(ASCInterface);
	}

	// 로컬컨트롤러는 인풋 바인딩 제거
	if (AUPFCharacterPlayer* PlayerCharacter = UPFActorUtility::GetTypedOwnerRecursive<AUPFCharacterPlayer>(GetOwner());
		PlayerCharacter->IsLocallyControlled())
	{
		const FGuid* InputID = InputBindIDs.Find(EquipmentType);
		if (!ensure(InputID)) return;
		PlayerCharacter->RemoveAbilitySetBind(*InputID);
		InputBindIDs.Remove(EquipmentType);
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
	APawn* OwnerPawn = UPFActorUtility::GetTypedOwnerRecursive<APawn>(GetOwner());
	if (!ensure(OwnerPawn)) return;
	
	// 어빌리티로 인해 서버와 로컬 컨트롤러에서만 호출되는 함수
	const bool IsRoleOK = OwnerPawn->HasAuthority() || OwnerPawn->IsLocallyControlled();
	if (!ensure(IsRoleOK)) return;
	
	// 아무 장비도 착용하지 않음
	if (!CurrentWeaponType.IsValid()) return;

	FUPFAppliedEquipmentEntry* EntryPtr = FindEquipment(CurrentWeaponType);
	if (!ensure(EntryPtr)) return;
	
	// 수납상태를 변경하고 어빌리티 관련 처리 수행
	ToggleHolsterWeaponInternal();
		
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

	if (HasAuthority())
	{
		// 서버, 로컬 컨트롤러를 제외한 플레이어들에게 수납 상태 변경 Client RPC 호출
		for(const AUPFCharacterPlayer* CharacterPlayer : TActorRange<AUPFCharacterPlayer>(GetWorld()))
		{
			if (!CharacterPlayer) continue;
			if (CharacterPlayer == OwnerPawn) continue;	// 이 캐릭터의 주인에게는 보내지 않음
			if (CharacterPlayer->IsLocallyControlled()) continue;	// 서버 자신에게는 보내지 않음

			if (UUPFCharacterEquipmentComponent* OtherPlayerEquipmentComp = CharacterPlayer->GetComponentByClass<UUPFCharacterEquipmentComponent>())
			{
				OtherPlayerEquipmentComp->ClientRPCToggleHolsterWeapon(this);
			}
		}
	}
}

void UUPFCharacterEquipmentComponent::ToggleHolsterWeaponInternal()
{
	FUPFAppliedEquipmentEntry* EntryPtr = FindEquipment(CurrentWeaponType);
	if (!ensure(EntryPtr)) return;
	
	// 부착할 캐릭터의 소켓를 구한다.
	const FName TargetSocket = bIsHolstered ? UPFSocketNames::hand_rSocket : UPFSocketNames::spine_03Socket;
	AUPFEquipmentInstance* EquipmentInstance = EntryPtr->EquipmentInstance;

	// Socket 이동
	ensure(EquipmentInstance->MeshComp->AttachToComponent(CharacterMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, TargetSocket));
	EquipmentInstance->OnSocketChanged(TargetSocket);
	
	bIsHolstered = !bIsHolstered;
}

void UUPFCharacterEquipmentComponent::ClientRPCToggleHolsterWeapon_Implementation(UUPFCharacterEquipmentComponent* TargetEquipmentComp)
{
	UPF_LOG_COMPONENT(LogTemp, Log, TEXT("Called"));
	
	if (TargetEquipmentComp)
	{
		TargetEquipmentComp->ToggleHolsterWeaponInternal();
	}
}


FUPFAppliedEquipmentEntry* UUPFCharacterEquipmentComponent::FindEquipment(FGameplayTag WeaponType)
{
	return AppliedEquipmentArray.Items.FindByPredicate([&WeaponType](const FUPFAppliedEquipmentEntry& X)
	{
		return X.EquipmentItemData->EquipmentType == WeaponType;
	});
}

void UUPFCharacterEquipmentComponent::UpdateWeaponState()
{
	FUPFAppliedEquipmentEntry* CurrentWeaponEntry = FindEquipment(CurrentWeaponType);
	if (!CurrentWeaponEntry || !CurrentWeaponEntry->EquipmentInstance)
	{
		bIsHolstered = false;
		return;
	}

	AUPFEquipmentInstance* CurrentWeaponInstance = CurrentWeaponEntry->EquipmentInstance;
	bIsHolstered = CurrentWeaponInstance->GetAttachParentSocketName() != UPFSocketNames::hand_rSocket;
}
