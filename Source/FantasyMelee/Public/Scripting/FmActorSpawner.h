// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Utils/Structs.h"
#include "FmActorSpawner.generated.h"

class UArrowComponent;
class UBoxComponent;

UCLASS()
class FANTASYMELEE_API AFmActorSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AFmActorSpawner();
	
	virtual void Tick(float DeltaTime) override;

protected:
	/**
	 * Override this function in BP if setting `TagRequirements` and `TagProhibitions` isn't sufficient.
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool IsActive() const;
	UFUNCTION(BlueprintImplementableEvent)
	void SetUpSpawnedActor(AActor* SpawnedActor);

	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UArrowComponent> ArrowComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditAnywhere, Category="FM Params")
	TSubclassOf<AActor> ActorClass;
	UPROPERTY(EditAnywhere, Category="FM Params")
	FVector BoxExtent = FVector(34.f, 34.f, 88.f);
	
	UPROPERTY(EditAnywhere, Category="FM Params")
	FFmEntityTagSpec TagRequirements;
	UPROPERTY(EditAnywhere, Category="FM Params")
	FFmEntityTagSpec TagProhibitions;
	
	UPROPERTY(EditAnywhere, Category="FM Params|Spawning")
	uint8 InitialSpawnCountMin = 0;
	UPROPERTY(EditAnywhere, Category="FM Params|Spawning")
	uint8 InitialSpawnCountMax = 0;
	
	UPROPERTY(EditAnywhere, Category="FM Params|Spawning")
	uint8 RecurringSpawnCountMin = 1;
	UPROPERTY(EditAnywhere, Category="FM Params|Spawning")
	uint8 RecurringSpawnCountMax = 1;
	
	UPROPERTY(EditAnywhere, Category="FM Params|Spawning")
	uint8 MaxSpawnCount = 100;
	
	UPROPERTY(EditAnywhere, Category="FM Params|Spawning")
	float TargetDeltaSecondsMin = 1.f;
	UPROPERTY(EditAnywhere, Category="FM Params|Spawning")
	float TargetDeltaSecondsMax = 5.f;
	
	UPROPERTY(VisibleInstanceOnly, Category="FM Runtime")
	float CurrentDeltaSeconds = 0.f;
	UPROPERTY(VisibleInstanceOnly, Category="FM Runtime")
	uint8 SpawnedActorCount = 0;
	UPROPERTY(VisibleInstanceOnly, Category="FM Runtime")
	float TargetDeltaSeconds = TargetDeltaSecondsMin;

	UFUNCTION()
	void DecrementSpawnedActorCount(AActor* DestroyedActor);

	void SpawnActor();
};
