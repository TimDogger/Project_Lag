// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PastTransform.generated.h"

USTRUCT()
struct FPastTransform
{
	GENERATED_BODY()
	
	float Time;

	FTransform Transform;

	FPastTransform() = default;

	FPastTransform(const float Time, const FTransform Transform)
		: Time(Time),
		  Transform(Transform)
	{
	}

	bool operator==(const FPastTransform& Other) const
	{
		return Time == Other.Time && Transform.Equals(Other.Transform);
	}
};

