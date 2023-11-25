// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterEquipmentComponent.h"
#include "Item/UPFEquipmentItemData.h"
#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"

// Sets default values for this component's properties
UUPFCharacterEquipmentComponent::UUPFCharacterEquipmentComponent()
{
	bWantsInitializeComponent = true;

	const FEquipmentSocketData SocketDataRifle(FName(TEXT("hand_rSocket")), FName(TEXT("spine_03Socket")));
	SocketDatas.Add(Item_Equipment_Weapon_Range_Rifle, SocketDataRifle);

	const FEquipmentSocketData SocketDataPistol(FName(TEXT("hand_rSocket")), FName(TEXT("holsterSocket")));
	SocketDatas.Add(Item_Equipment_Weapon_Range_Pistol, SocketDataPistol);
	
	SetIsReplicated(true);
}

void UUPFCharacterEquipmentComponent::InitializeComponent()
{
	Super::InitializeComponent();

	CharacterMeshComponent = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();
}

void UUPFCharacterEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UUPFCharacterEquipmentComponent::EquipOrSwitchItem(const UUPFEquipmentItemData* Data)
{
	// 착용중인 장비 제거
	if (Equipments.Contains(Data->EquipmentType))
	{
		Equipments[Data->EquipmentType]->Destroy();
		Equipments.Remove(Data->EquipmentType);
	}
	
	AUPFEquipmentInstance* SpawnedItem = GetWorld()->SpawnActorDeferred<AUPFEquipmentInstance>(Data->InstanceClass, FTransform::Identity);
	if (!ensure(SpawnedItem)) return;

	// Mesh 적용 등 세팅
	SpawnedItem->SetData(Data);
	
	// 소켓에 부착
	const bool IsCurrentWeaponType = CurrentWeaponType == Data->EquipmentType;
	const FName AttachSocket = IsHolstered || !IsCurrentWeaponType
		                           ? SocketDatas[Data->EquipmentType].HolsterSocket
		                           : SocketDatas[Data->EquipmentType].HandSocket;
	SpawnedItem->MeshComp->AttachToComponent(CharacterMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, AttachSocket);
	
	SpawnedItem->FinishSpawning(FTransform::Identity);

	Equipments.Add(Data->EquipmentType, SpawnedItem);
}

void UUPFCharacterEquipmentComponent::ToggleHolsterWeapon()
{
	// 아무 장비도 착용하지 않음
	if (!CurrentWeaponType.IsValid()) return;
	if (!Equipments.Contains(CurrentWeaponType)) return;

	UAnimInstance* AnimInstance = CharacterMeshComponent->GetAnimInstance();
	check(AnimInstance);

	AnimInstance->Montage_Play(HolsterMontage, 1.0f);

	// todo: AnimNotifyReload 제작, 구현
	IsHolstered = !IsHolstered;
}