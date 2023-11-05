// Copyright AB. All Rights Reserved.

#include "Characters/FmCharacter.h"

#include "Components/FmInventoryComponent.h"
#include "MotionWarpingComponent.h"

AFmCharacter::AFmCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	InventoryComponent = CreateDefaultSubobject<UFmInventoryComponent>(TEXT("InventoryComponent"));
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}

void AFmCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(CombatComponent);
	check(HitReactionMontageData.IsValid());
}

void AFmCharacter::ToggleInjured(const bool bInIsInjured)
{
	bIsInjured = bInIsInjured;
}
