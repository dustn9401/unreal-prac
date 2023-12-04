// Fill out your copyright notice in the Description page of Project Settings.


#include "UPFCharacterEquipmentComponent.h"

#include "AbilitySystemComponent.h"
#include "UnrealPortfolio.h"
#include "Item/UPFEquipmentItemData.h"
#include "Item/ItemInstance/Equipments/UPFEquipmentInstance.h"
#include "Net/UnrealNetwork.h"

void FUPFEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
}

void FUPFEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
}

void FUPFEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}

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
}

void UUPFCharacterEquipmentComponent::InitializeComponent()
{
	Super::InitializeComponent();

	CharacterMeshComponent = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();
}

void UUPFCharacterEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UUPFCharacterEquipmentComponent, EquipmentList);
}

bool UUPFCharacterEquipmentComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	return Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
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

	// 장비 목록에 추가
	FUPFAppliedEquipmentEntry& NewEntry = EquipmentList.Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentItemData = Data;
	NewEntry.EquipmentInstance = SpawnedItem;
	EquipmentList.MarkItemDirty(NewEntry);
}

void UUPFCharacterEquipmentComponent::UnEquipItem(FGameplayTag EquipmentType)
{
	for(auto It = EquipmentList.Entries.CreateIterator(); It; ++It)
	{
		FUPFAppliedEquipmentEntry& Entry = *It;
		if (Entry.EquipmentItemData->EquipmentType == EquipmentType)
		{
			if (UAbilitySystemComponent* ASC = GetOwner()->GetComponentByClass<UAbilitySystemComponent>())
			{
				Entry.GrantedData.TakeFromASC(ASC);
			}

			Entry.EquipmentInstance->DestroySelf();

			It.RemoveCurrent();
			EquipmentList.MarkArrayDirty();
		}
	}
}

void UUPFCharacterEquipmentComponent::ToggleHolsterWeapon()
{
	// 아무 장비도 착용하지 않음
	if (!CurrentWeaponType.IsValid()) return;

	UAnimInstance* AnimInstance = CharacterMeshComponent->GetAnimInstance();
	check(AnimInstance);

	AnimInstance->Montage_Play(HolsterMontage, 1.0f);
}

void UUPFCharacterEquipmentComponent::OnAnimNotifyHolster()
{
	UPF_LOG_COMPONENT(LogTemp, Log, TEXT("OnAnimNotifyHolster"));

	if (!CurrentWeaponType.IsValid()) return;
	const FUPFAppliedEquipmentEntry* TargetEntry = EquipmentList.Entries.FindByPredicate([&](const FUPFAppliedEquipmentEntry& X)
	{
		return X.EquipmentItemData->EquipmentType == CurrentWeaponType;
	});
	
	if (TargetEntry == nullptr) return;
	
	FEquipmentSocketData SocketData = SocketDatas[CurrentWeaponType];
	FName TargetSocket = IsHolstered ? SocketData.HandSocket : SocketData.HolsterSocket;
	TargetEntry->EquipmentInstance->MeshComp->AttachToComponent(CharacterMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, TargetSocket);
	
	IsHolstered = !IsHolstered;
}
