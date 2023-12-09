// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterEquipmentComponent.h"

#include "AbilitySystemComponent.h"
#include "UnrealPortfolio.h"
#include "UPFAbilitySystemComponent.h"
#include "Character/UPFCharacterBase.h"
#include "Item/UPFEquipmentItemData.h"
#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UUPFCharacterEquipmentComponent::UUPFCharacterEquipmentComponent()
{
	bWantsInitializeComponent = true;

	const FEquipmentSocketData SocketDataRifle(FName(TEXT("hand_rSocket")), FName(TEXT("spine_03Socket")));
	SocketDatas.Add(Item_Equipment_Weapon_Range_Rifle, SocketDataRifle);

	const FEquipmentSocketData SocketDataPistol(FName(TEXT("hand_rSocket")), FName(TEXT("holsterSocket")));
	SocketDatas.Add(Item_Equipment_Weapon_Range_Pistol, SocketDataPistol);

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

void UUPFCharacterEquipmentComponent::ServerRPCEquipItem_Implementation(const UUPFEquipmentItemData* Data)
{
	UPF_LOG_COMPONENT(LogTemp, Log, TEXT("Called"));

	MulticastRPCEquipItem(Data);

	if (!ensure(Equipments.Contains(Data->EquipmentType)))
	{
		return;
	}

	// 어빌리티 부여
	if (Data->AbilitiesToGrant)
	{
		AUPFCharacterBase* OwnerCharacter = CastChecked<AUPFCharacterBase>(GetOwner());
		FUPFAppliedEquipmentEntry& Entry = Equipments[Data->EquipmentType];
		Data->AbilitiesToGrant->GiveToCharacter(OwnerCharacter, Entry.EquipmentInstance, &Entry.GrantedData);
	}
}

bool UUPFCharacterEquipmentComponent::ServerRPCEquipItem_Validate(const UUPFEquipmentItemData* Data)
{
	return true;
}

void UUPFCharacterEquipmentComponent::MulticastRPCEquipItem_Implementation(const UUPFEquipmentItemData* Data)
{
	EquipItem(Data);
}

void UUPFCharacterEquipmentComponent::EquipItem(const UUPFEquipmentItemData* Data)
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

	AUPFCharacterBase* EquipCharacter = Cast<AUPFCharacterBase>(GetOwner());
	check(EquipCharacter);
	
	SpawnedItem->OnEquipped(EquipCharacter);
}

void UUPFCharacterEquipmentComponent::ServerRPCUnEquipItem_Implementation(FGameplayTag EquipmentType)
{
	UUPFAbilitySystemComponent* ASC = GetOwner()->GetComponentByClass<UUPFAbilitySystemComponent>();
	check(ASC);

	if (!Equipments.Contains(EquipmentType)) return;

	AUPFCharacterBase* OwnerCharacter = CastChecked<AUPFCharacterBase>(GetOwner());
	Equipments[EquipmentType].GrantedData.TakeFromCharacter(OwnerCharacter);
	
	MulticastRPCUnEquipItem(EquipmentType);
}

void UUPFCharacterEquipmentComponent::MulticastRPCUnEquipItem_Implementation(FGameplayTag EquipmentType)
{
	UnEquipItem(EquipmentType);
}

void UUPFCharacterEquipmentComponent::UnEquipItem(FGameplayTag EquipmentType)
{
	if (!Equipments.Contains(EquipmentType)) return;

	const FUPFAppliedEquipmentEntry& RemovedEntry = Equipments[EquipmentType];
	
	AUPFCharacterBase* EquipCharacter = Cast<AUPFCharacterBase>(GetOwner());
	check(EquipCharacter);
	RemovedEntry.EquipmentInstance->OnUnEquipped(EquipCharacter);
	
	RemovedEntry.EquipmentInstance->DestroySelf();
	Equipments.Remove(EquipmentType);
}

void UUPFCharacterEquipmentComponent::ToggleHolsterWeapon()
{
	// 아무 장비도 착용하지 않음
	if (!CurrentWeaponType.IsValid()) return;
	
	if (!Equipments.Contains(CurrentWeaponType)) return;
	
	FEquipmentSocketData SocketData = SocketDatas[CurrentWeaponType];
	FName TargetSocket = bIsHolstered ? SocketData.HandSocket : SocketData.HolsterSocket;
	Equipments[CurrentWeaponType].EquipmentInstance->MeshComp->AttachToComponent(CharacterMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, TargetSocket);
	
	bIsHolstered = !bIsHolstered;
}
