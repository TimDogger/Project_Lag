// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ActorTransform.generated.h"

/**
 * 
 */
USTRUCT()
struct FActorTransform
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Actor;

	UPROPERTY()
	FTransform Transform;

	FActorTransform() = default;

	FActorTransform(AActor* const Actor, const FTransform Transform)
		: Actor(Actor),
		  Transform(Transform)
	{
	}
};
