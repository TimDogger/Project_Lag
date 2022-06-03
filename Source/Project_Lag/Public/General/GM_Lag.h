// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Structs/ActorsPast.h"
#include "GM_Lag.generated.h"

/**
 Отвечает за выдачу игроку Pawn и механику лаг компенсации при стрельбе
 */
UCLASS()
class PROJECT_LAG_API AGM_Lag : public AGameMode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FActorsPast> ActorsPasts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APawn> PlayersPawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RecordTimeLength = 200.0f;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void RecordActors();

	// Отрисовка капсулы
	void DrawDebugCapsule(FVector Location, float HalfHeight, float Radius, FRotator Rotation, FLinearColor Color);

	// Получение Transform из прошлого на основе задержки
	void GetActorsPastTransform(FActorsPast ActorsPast, float Latency, bool& bValidTransform, FTransform& OutTransform);

	// Добавление актора в учет таймлайна
	int AddActorToPastTimeline(AActor* Actor);

	// Спавн Pawn'а Игрока
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnPlayerPawn(AController* NewPlayerController, FTransform Transform, APawn*& OutPawn);

	// Вернуть акторы на прошлые позиции с учетом задержки.
	// После вызова можно выполнять необходимые действия,
	// после чего вызвать ForwardAllActors чтобы вернуть акторы на места
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RewindAllActors(float ClientLatency);

	// Возвращает акторы на текущие позиции. Вызывать только после вызова RewindAllActors
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ForwardAllActors();
};
