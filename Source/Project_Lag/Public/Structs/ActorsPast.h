// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PastTransform.h"
#include "Containers/CircularQueue.h"
#include "Containers/RingBuffer.h"
#include "ActorsPast.generated.h"


USTRUCT(BlueprintType)
struct FActorsPast
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Actor;

	TRingBuffer<FPastTransform> PastTransforms;

	FActorsPast() = default;

	bool operator==(const FActorsPast& Other) const
	{
		return Actor == Other.Actor;
	}

	explicit FActorsPast(AActor* const Actor)
		: Actor(Actor)
	{
		PastTransforms = TRingBuffer<FPastTransform>();
	}
};
