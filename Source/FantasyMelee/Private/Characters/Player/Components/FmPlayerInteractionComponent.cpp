// Copyright AB. All Rights Reserved.

#include "Characters/Player/Components/FmPlayerInteractionComponent.h"

#include "Characters/Player/FmPlayerCharacter.h"
#include "Characters/Player/FmPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/FmInteractable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/FmHud.h"
#include "Utils/Constants.h"

UFmPlayerInteractionComponent::UFmPlayerInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFmPlayerInteractionComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Owner) return;
	
	const auto Capsule = Owner->GetCapsuleComponent();
	const auto Origin = Owner->GetActorLocation();
	TArray<FHitResult> OutHits;
				
	UKismetSystemLibrary::BoxTraceMulti(this,
		Origin, Origin + Owner->GetActorForwardVector() * TraceLength,
		FVector(10, Capsule->GetUnscaledCapsuleRadius(), Capsule->GetUnscaledCapsuleHalfHeight()),
		Owner->GetActorRotation(), UEngineTypes::ConvertToTraceType(TRACE_CHANNEL_INTERACTION), false,
		TArray<AActor*> { Owner }, EDrawDebugTrace::None, OutHits, true);

	const auto InteractableHitResult = OutHits.FindByPredicate([this](const FHitResult& HitResult)
	{
		const auto Interactable = Cast<IFmInteractable>(HitResult.GetActor());

		return Interactable != nullptr && Interactable->CanInteract(Owner);
	});

	const AActor* NewTargetInteractable = nullptr;

	if (InteractableHitResult)
	{
		NewTargetInteractable = InteractableHitResult->GetActor();
	}

	if (NewTargetInteractable != TargetInteractable)
	{
		if (const auto Hud = Owner->GetCustomHud())
		{
			Hud->BroadcastTargetInteractable(NewTargetInteractable);
		}
	}

	TargetInteractable = NewTargetInteractable;
}

void UFmPlayerInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = CastChecked<AFmPlayerCharacter>(GetOwner());
}

