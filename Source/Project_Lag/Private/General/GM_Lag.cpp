// Fill out your copyright notice in the Description page of Project Settings.


#include "General/GM_Lag.h"

#include "ToolBuilderUtil.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Project_Lag/LagCharacter_Base.h"
#include "Structs/ActorsPast.h"

DEFINE_LOG_CATEGORY_STATIC(LogCorrection, Warning, All);

void AGM_Lag::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutActors);

	if (OutActors.Num() > 0)
	{
		APawn* OutPawn;
		SpawnPlayerPawn(NewPlayer, OutActors[0]->GetTransform(), OutPawn);
		if (OutPawn)
		{
			AddActorToPastTimeline(OutPawn);
		}
	}
}

void AGM_Lag::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALagCharacter_Base::StaticClass(), Actors);

	if (Actors.Num() > 0)
	{
		for (const auto Actor : Actors)
		{
			AddActorToPastTimeline(Actor);
		}
	}
}

void AGM_Lag::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	RecordActors();
}

void AGM_Lag::RecordActors()
{
	const float Time = UKismetSystemLibrary::GetGameTimeInSeconds(GetWorld());

	UE_LOG(LogCorrection, Warning, TEXT("Time: %f"), Time);
	for (auto& ActorsPast : ActorsPasts)
	{
		ActorsPast.PastTransforms.AddFront(FPastTransform(Time, ActorsPast.Actor->GetTransform()));
		const int Diff = ActorsPast.PastTransforms.First().Time - ActorsPast.PastTransforms.Last().Time;
		if (Diff > RecordTimeLength)
		{
			ActorsPast.PastTransforms.Pop();
		}
	}
}

void AGM_Lag::GetActorsPastTransform(FActorsPast ActorsPast, float Latency, bool& bValidTransform,
                                     FTransform& OutTransform)
{
	const float ActualTime = ActorsPast.PastTransforms[0].Time;
	const float TargetTime = ActualTime - Latency;
	bValidTransform = false;

	UE_LOG(LogCorrection, Warning, TEXT("ActualTime  %f,  TargetTime %f,    RewindTime %f"), ActualTime, TargetTime,
	       Latency);

	// If ActualTime == TargetTime then the Server has fired. Do no compensation, return
	if (ActualTime == TargetTime) return;

	for (int i = 1; i < ActorsPast.PastTransforms.Num(); ++i)
	{
		const int FirstIndex = i;
		const int SecondIndex = i - 1;
		if (TargetTime > ActorsPast.PastTransforms[FirstIndex].Time)
		{
			const FTransform FirstTransform = ActorsPast.PastTransforms[FirstIndex].Transform;
			const FTransform SecondTransform = ActorsPast.PastTransforms[SecondIndex].Transform;

			const float FirstTime = ActorsPast.PastTransforms[FirstIndex].Time;
			const float SecondTime = ActorsPast.PastTransforms[SecondIndex].Time;

			const float Alpha = UKismetMathLibrary::MapRangeClamped(TargetTime, FirstTime, SecondTime, 0.0f, 1.0f);

			UE_LOG(LogCorrection, Warning, TEXT("FirstTime %f,    SecondTime %f    Alpha %f"), FirstTime, SecondTime,
			       Alpha);

			OutTransform = UKismetMathLibrary::TLerp(FirstTransform, SecondTransform, Alpha);
			bValidTransform = true;

			const auto Capsule = ActorsPast.Actor->FindComponentByClass<UCapsuleComponent>();

			DrawDebugCapsule(FirstTransform.GetLocation(), Capsule->GetScaledCapsuleHalfHeight(),
			                 Capsule->GetScaledCapsuleRadius(), FirstTransform.Rotator(), FLinearColor::Blue);

			DrawDebugCapsule(OutTransform.GetLocation(), Capsule->GetScaledCapsuleHalfHeight(),
			                 Capsule->GetScaledCapsuleRadius(), OutTransform.Rotator(),
			                 FLinearColor::Yellow);

			DrawDebugCapsule(SecondTransform.GetLocation(), Capsule->GetScaledCapsuleHalfHeight(),
			                 Capsule->GetScaledCapsuleRadius(), SecondTransform.Rotator(), FLinearColor::Red);

			return;
		}
	}
}

int AGM_Lag::AddActorToPastTimeline(AActor* Actor)
{
	if (Actor) return ActorsPasts.AddUnique(FActorsPast(Actor));
	return -1;
}

void AGM_Lag::RewindAllActors(const float ClientLatency)
{
	for (const auto CurrentActor : ActorsPasts)
	{
		bool bValid;
		FTransform PastTransform;
		GetActorsPastTransform(CurrentActor, ClientLatency, bValid, PastTransform);
		if (bValid && PastTransform.IsValid())
		{
			CurrentActor.Actor->SetActorTransform(PastTransform, false, nullptr, ETeleportType::TeleportPhysics);
		}
		else
		{
			UE_LOG(LogCorrection, Warning, TEXT("No valid Transform for %s"), *CurrentActor.Actor->GetName());
		}
	}
}

void AGM_Lag::ForwardAllActors()
{
	for (auto& CurrentActor : ActorsPasts)
	{
		// Первый трансформ в PastTransforms это текущее положение у актора
		CurrentActor.Actor->SetActorTransform(CurrentActor.PastTransforms[0].Transform);
	}
}

void AGM_Lag::DrawDebugCapsule(FVector Location, float HalfHeight, float Radius,
                               FRotator Rotation, FLinearColor Color)
{
	UKismetSystemLibrary::DrawDebugCapsule(GetWorld(), Location, HalfHeight, Radius, Rotation, Color,
	                                       5.0f,
	                                       1.0f);
}
