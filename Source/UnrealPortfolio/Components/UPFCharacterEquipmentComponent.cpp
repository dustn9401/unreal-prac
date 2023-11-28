// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterEquipmentComponent.h"

#include "UnrealPortfolio.h"
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

	static ConstructorHelpers::FObjectFinder<UAnimMontage> HolsterMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/UnrealPortfolio/Animation/AM_Holster.AM_Holster'"));
	if (HolsterMontageRef.Object)
	{
		HolsterMontage = HolsterMontageRef.Object;
	}
	
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
	
	SpawnedItem->FinishSpawning(FTransform::Identity);
	
	// 소켓에 부착
	const bool IsUnarmed = !CurrentWeaponType.IsValid();	// 아무 장비도 착용하지 않았다면, 현재 선택된 장비 타입으로 지정하고 손에 쥐어준다.
	const bool IsCurrentWeaponType = CurrentWeaponType == Data->EquipmentType;
	const bool AttachToHand = (IsCurrentWeaponType && !IsHolstered) || IsUnarmed;
	const FName AttachSocket = AttachToHand
		                           ? SocketDatas[Data->EquipmentType].HandSocket
		                           : SocketDatas[Data->EquipmentType].HolsterSocket;
	
	if (IsUnarmed)
	{
		IsHolstered = false;
		CurrentWeaponType = Data->EquipmentType;
	}
	
	SpawnedItem->MeshComp->AttachToComponent(CharacterMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, AttachSocket);

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

}

void UUPFCharacterEquipmentComponent::OnAnimNotifyHolster()
{
	UPF_LOG_COMPONENT(LogTemp, Log, TEXT("OnAnimNotifyHolster"));

	if (!CurrentWeaponType.IsValid()) return;
	if (!Equipments.Contains(CurrentWeaponType)) return;
	
	FEquipmentSocketData SocketData = SocketDatas[CurrentWeaponType];
	FName TargetSocket = IsHolstered ? SocketData.HandSocket : SocketData.HolsterSocket;
	Equipments[CurrentWeaponType]->MeshComp->AttachToComponent(CharacterMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, TargetSocket);
	
	IsHolstered = !IsHolstered;
}
