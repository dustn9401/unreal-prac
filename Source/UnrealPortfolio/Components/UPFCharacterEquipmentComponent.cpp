// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterEquipmentComponent.h"

#include "AbilitySystemComponent.h"
#include "UnrealPortfolio.h"
#include "UPFAbilitySystemComponent.h"
#include "UPFGameplayTags.h"
#include "Character/UPFCharacterBase.h"
#include "Item/UPFEquipmentItemData.h"
#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"
#include "Net/UnrealNetwork.h"

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

void UUPFCharacterEquipmentComponent::EquipItem(const UUPFEquipmentItemData* Data)
{
	// 각 플레이어에는 장비 스폰 및 이 캐릭터에 장착시키고, AppliedEquipmentEntry 를 맵에 추가
	MulticastRPCEquipItem(Data);

	// 서버는 추가된 AppliedEquipmentEntry 를 사용해서 어빌리티를 지급한다.
	ServerRPCGiveEquipmentAbility(Data->EquipmentType);
}

void UUPFCharacterEquipmentComponent::MulticastRPCEquipItem_Implementation(const UUPFEquipmentItemData* Data)
{
	InternalMulticastEquipItem(Data);
}

void UUPFCharacterEquipmentComponent::InternalMulticastEquipItem(const UUPFEquipmentItemData* Data)
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
	ServerRPCTakeEquipmentAbility(EquipmentType);

	// 그다음 각 플레이어에게 이 캐릭터의 장비 제거 신호
	MulticastRPCUnEquipItem(EquipmentType);
}

void UUPFCharacterEquipmentComponent::MulticastRPCUnEquipItem_Implementation(FGameplayTag EquipmentType)
{
	InternalMulticastUnEquipItem(EquipmentType);
}

void UUPFCharacterEquipmentComponent::InternalMulticastUnEquipItem(FGameplayTag EquipmentType)
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
		// todo: 다른 장비로 교체해주기
		CurrentWeaponType = FGameplayTag::EmptyTag;
		bIsHolstered = true;
	}
}

void UUPFCharacterEquipmentComponent::ServerRPCGiveEquipmentAbility_Implementation(FGameplayTag EquipmentType)
{
	FUPFAppliedEquipmentEntry* EntryPtr = Equipments.Find(EquipmentType);
	if (!ensure(EntryPtr)) return;
	if (!ensure(!EntryPtr->IsAbilityGranted())) return; // 이미 어빌리티가 지급된 상태임
	if (!EntryPtr->EquipmentItemData->AbilitiesToGrant) return;
	
	AUPFCharacterBase* OwnerCharacter = CastChecked<AUPFCharacterBase>(GetOwner());
	EntryPtr->EquipmentItemData->AbilitiesToGrant->GiveToCharacter(OwnerCharacter, EntryPtr->EquipmentInstance, &EntryPtr->GrantedData);
}

bool UUPFCharacterEquipmentComponent::ServerRPCGiveEquipmentAbility_Validate(FGameplayTag EquipmentType)
{
	return true;
}

void UUPFCharacterEquipmentComponent::ServerRPCTakeEquipmentAbility_Implementation(FGameplayTag EquipmentType)
{
	FUPFAppliedEquipmentEntry* EntryPtr = Equipments.Find(EquipmentType);
	if (!ensure(EntryPtr)) return;
	if (!EntryPtr->IsAbilityGranted()) return;

	AUPFCharacterBase* OwnerCharacter = CastChecked<AUPFCharacterBase>(GetOwner());
	EntryPtr->GrantedData.TakeFromCharacter(OwnerCharacter);
}

bool UUPFCharacterEquipmentComponent::ServerRPCTakeEquipmentAbility_Validate(FGameplayTag EquipmentType)
{
	return true;
}

bool UUPFCharacterEquipmentComponent::CanToggleHolster()
{
	// 아무 장비도 착용하지 않음
	if (!CurrentWeaponType.IsValid())
	{
		return false;
	}

	FUPFAppliedEquipmentEntry* EntryPtr = Equipments.Find(CurrentWeaponType);
	if (!ensure(EntryPtr))
	{
		return false;	// 여기로 오면 안됨
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
		if (GetOwner()->HasAuthority())
		{
			ServerRPCTakeEquipmentAbility(CurrentWeaponType);
		}
	}
	else
	{
		EquipmentInstance->OnAttachedToHand(OwnerCharacter);

		// 어빌리티를 다시 지급힌다.
		if (GetOwner()->HasAuthority())
		{
			ServerRPCGiveEquipmentAbility(CurrentWeaponType);
		}
	}
}
