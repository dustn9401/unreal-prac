#pragma once

namespace UPFActorUtility
{
	template <typename T>
	static T* GetTypedOwnerRecursive(AActor* InActor)
	{
		while (InActor)
		{
			if (T* TActor = Cast<T>(InActor))
			{
				return TActor;
			}

			InActor = InActor->GetOwner();
		}

		return nullptr;
	}
}
