// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterEquipmentComponent.h"

#include "AbilitySystemComponent.h"
#include "UnrealPortfolio.h"
#include "UPFGameplayTags.h"
#include "Character/UPFCharacterBase.h"
#include "Item/UPFEquipmentItemData.h"
#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"
#include "Player/UPFCharacterPlayer.h"
#include "Utility/UPFActorUtility.h"

// Sets default values for this component's properties
UUPFCharacterEquipmentComponent::UUPFCharacterEquipmentComponent()
{
	bWantsInitializeComponent = true;

	const FEquipmentSocketData SocketDataRifle(FName(TEXT("hand_rSocket")), FName(TEXT("spine_03Socket")));
	SocketDatas.Add(UPFGameplayTags::Item_Equipment_Weapon_Range_Rifle, SocketDataRifle);

	const FEquipmentSocketData SocketDataPistol(FName(TEXT("hand_rSocket")), FName(TEXT("holsterSocket")));
	SocketDatas.Add(UPFGameplayTags::Item_Equipment_Weapon_Range_Pistol, SocketDataPistol);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> HolsterMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/UnrealPortfolio/Animation/AM_Holster.AM_Holster'"));
	if (HolsterMontageRef.Object)
	{
		HolsterMontage = HolsterMontageRef.Object;
	}
	
	SetIsReplicatedByDefault(true);

	bIsHolstered = true;
}

void UUPFCharacterEquipmentComponent::InitializeComponent()
{
	Super::InitializeComponent();

	CharacterMeshComponent = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();
}

void UUPFCharacterEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME(UUPFCharacterEquipmentComponent, bIsHolstered);
	// DOREPLIFETIME(UUPFCharacterEquipmentComponent, CurrentWeaponType);
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
	// 각 플레이어에는 장비 스폰 및 이 캐릭터에 장착시키고, AppliedEquipmentEntry 를 맵에 추가
	EquipItemInternal(Data);

	// 서버는 추가된 AppliedEquipmentEntry 를 사용해서 어빌리티를 지급한다.
	if (const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		GiveEquipmentAbility(ASCInterface, Data->EquipmentType);
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
	const bool IsUnarmed = !CurrentWeaponType.IsValid();	// 아무 장비도 착용하지 않았다면, 현재 선택된 장비 타입으로 지정하고 손에 쥐어준다.
	const bool IsCurrentWeaponType = CurrentWeaponType == Data->EquipmentType;
	const bool AttachToHand = (IsCurrentWeaponType && !bIsHolstered) || IsUnarmed;
	const FName AttachSocket = AttachToHand
								   ? SocketDatas[Data->EquipmentType].HandSocket
								   : SocketDatas[Data->EquipmentType].HolsterSocket;
	
	if (IsUnarmed)
	{
		bIsHolstered = false;
		CurrentWeaponType = Data->EquipmentType;
	}

	// 여기서 Owner 변경되어 Replicate됨
	if (!SpawnedItem->MeshComp->AttachToComponent(CharacterMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, AttachSocket))
	{
		UPF_LOG_COMPONENT(LogTemp, Error, TEXT("AttachToComponent Failed"));
	}

	// 장비 목록에 추가
	FUPFAppliedEquipmentEntry NewEntry;
	NewEntry.EquipmentItemData = Data;
	NewEntry.EquipmentInstance = SpawnedItem;

	Equipments.Emplace(Data->EquipmentType, NewEntry);

	ACharacter* EquipCharacter = Cast<ACharacter>(GetOwner());
	check(EquipCharacter);

	if (AttachToHand)
	{
		SpawnedItem->OnAttachedToHand(EquipCharacter);
	}
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
	if (!Equipments.Contains(EquipmentType)) return;

	const FUPFAppliedEquipmentEntry& RemovedEntry = Equipments[EquipmentType];
	
	ACharacter* EquipCharacter = Cast<ACharacter>(GetOwner());
	check(EquipCharacter);
	RemovedEntry.EquipmentInstance->OnDetachedFromHand(EquipCharacter);
	
	RemovedEntry.EquipmentInstance->DestroySelf();
	Equipments.Remove(EquipmentType);

	// 손에 들고있던 무기가 제거된 경우 처리
	if (EquipmentType == CurrentWeaponType)
	{
		CurrentWeaponType = FGameplayTag::EmptyTag;
		bIsHolstered = true;
	}
}

void UUPFCharacterEquipmentComponent::GiveEquipmentAbility(const IAbilitySystemInterface* ASCInterface, FGameplayTag EquipmentType)
{
	FUPFAppliedEquipmentEntry* EntryPtr = Equipments.Find(EquipmentType);
	if (!ensure(EntryPtr)) return;
	if (!ensure(EntryPtr->EquipmentItemData)) return;
	if (!EntryPtr->EquipmentItemData->AbilitiesToGrant) return;
	
	// 서버: 어빌리티 지급
	if (HasAuthority())
	{
		check(!EntryPtr->IsAbilityGranted());	// 어빌리티가 이미 지급된 상태면 중복 호출임
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
	FUPFAppliedEquipmentEntry* EntryPtr = Equipments.Find(EquipmentType);
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

	const FUPFAppliedEquipmentEntry* EntryPtr = Equipments.Find(CurrentWeaponType);
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

	FUPFAppliedEquipmentEntry* EntryPtr = Equipments.Find(CurrentWeaponType);
	if (!EntryPtr) return;

	// 부착할 캐릭터의 소켓를 구한다.
	FEquipmentSocketData SocketData = SocketDatas[CurrentWeaponType];
	FName TargetSocket = bIsHolstered ? SocketData.HandSocket : SocketData.HolsterSocket;
	AUPFEquipmentInstance* EquipmentInstance = EntryPtr->EquipmentInstance;

	// Socket 이동
	EquipmentInstance->MeshComp->AttachToComponent(CharacterMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, TargetSocket);
	
	bIsHolstered = !bIsHolstered;

	ACharacter* OwnerCharacter = CastChecked<ACharacter>(GetOwner());
	if (bIsHolstered)
	{
		EquipmentInstance->OnDetachedFromHand(OwnerCharacter);

		// 수납 중 어빌리티를 제거한다.
		if (const IAbilitySystemInterface* ASCInterface = UPFActorUtility::GetTypedOwnerRecursive<IAbilitySystemInterface>(GetOwner()))
		{
			TakeEquipmentAbility(ASCInterface, CurrentWeaponType);
		}
	}
	else
	{
		EquipmentInstance->OnAttachedToHand(OwnerCharacter);

		// 어빌리티를 다시 지급힌다.
		if (const IAbilitySystemInterface* ASCInterface = UPFActorUtility::GetTypedOwnerRecursive<IAbilitySystemInterface>(GetOwner()))
		{
			GiveEquipmentAbility(ASCInterface, CurrentWeaponType);
		}
	}
}
