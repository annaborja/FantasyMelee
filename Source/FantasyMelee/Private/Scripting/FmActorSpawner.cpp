// Copyright AB. All Rights Reserved.

#include "Scripting/FmActorSpawner.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/FmBlueprintFunctionLibrary.h"

AFmActorSpawner::AFmActorSpawner(): AActor()
{
	// TODO(P1): Optimize for performance.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetRootComponent());
	BoxComponent->SetBoxExtent(FVector(34.f, 34.f, 88.f));
	BoxComponent->SetMobility(EComponentMobility::Static);
	
	BoxComponent->SetCanEverAffectNavigation(false);
	BoxComponent->SetGenerateOverlapEvents(false);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardArrow"));
	ArrowComponent->SetupAttachment(BoxComponent);
}

void AFmActorSpawner::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsActive()) return;

	CurrentDeltaSeconds += DeltaTime;

	if (CurrentDeltaSeconds >= TargetDeltaSeconds)
	{
		const auto RecurringSpawnCount = FMath::RandRange(RecurringSpawnCountMin, RecurringSpawnCountMax);
		for (uint8 i = 1; i <= RecurringSpawnCount; i++)
		{
			SpawnActor();
		}
		
		CurrentDeltaSeconds = 0.f;
		TargetDeltaSeconds = FMath::RandRange(TargetDeltaSecondsMin, TargetDeltaSecondsMax);
	}
}

bool AFmActorSpawner::IsActive_Implementation() const
{
	return UFmBlueprintFunctionLibrary::IsEntityTagSpecSatisfied(this, TagRequirements) &&
		UFmBlueprintFunctionLibrary::IsEntityTagSpecSatisfied(this, TagProhibitions, true);
}

void AFmActorSpawner::BeginPlay()
{
	Super::BeginPlay();

	check(ActorClass);

	const auto InitialSpawnCount = FMath::RandRange(InitialSpawnCountMin, InitialSpawnCountMax);
	for (uint8 i = 1; i <= InitialSpawnCount; i++)
	{
		SpawnActor();
	}
}

void AFmActorSpawner::OnSpawnedActorDestroyed_Implementation(AActor* DestroyedActor)
{
	SpawnedActorCount--;
}

void AFmActorSpawner::SpawnActor()
{
	if (SpawnedActorCount >= MaxSpawnCount) return;
	
	if (const auto World = GetWorld())
	{
		const FTransform SpawnTransform(GetActorRotation(), GetActorLocation());
		
		const auto SpawnedActor = World->SpawnActorDeferred<AActor>(ActorClass, SpawnTransform, this, nullptr,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

		SetUpSpawnedActor(SpawnedActor);

		UGameplayStatics::FinishSpawningActor(SpawnedActor, SpawnTransform);
		
		SpawnedActor->OnDestroyed.AddDynamic(this, &ThisClass::OnSpawnedActorDestroyed);
		SpawnedActorCount++;
	}
}
