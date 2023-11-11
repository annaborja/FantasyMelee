// Copyright AB. All Rights Reserved.

#include "Characters/NPCs/FmNpcCharacter.h"

#include "BehaviorTree/BehaviorTree.h"
#include "AI/FmAiController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/FmCharacterMovementComponent.h"
#include "Components/FmCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/FmAbilitySystemComponent.h"
#include "GAS/Attributes/FmPrimaryAttributeSet.h"
#include "GAS/Attributes/FmVitalAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "Tags/TempTags.h"
#include "Utils/Constants.h"
#include "Utils/FmBlueprintFunctionLibrary.h"
#include "Utils/Macros.h"

AFmNpcCharacter::AFmNpcCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFmCharacterMovementComponent>(CharacterMovementComponentName))
{
	AIControllerClass = AFmAiController::StaticClass();
	CombatComponent = CreateDefaultSubobject<UFmCombatComponent>(TEXT("CombatComponent"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	if (const auto CharacterMovement = GetCharacterMovement())
	{
		CharacterMovement->bUseControllerDesiredRotation = true;
	}
	
	if (const auto CapsuleComponent = GetCapsuleComponent())
	{
		CapsuleComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		CapsuleComponent->SetCollisionResponseToChannel(TRACE_CHANNEL_WEAPON, ECR_Block);
	}

	if (const auto Mesh = GetMesh())
	{
		Mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(OBJECT_CHANNEL_PLAYER, ECR_Ignore);
	}
	
	AbilitySystemComponent = CreateDefaultSubobject<UFmAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	
	PrimaryAttributeSet = CreateDefaultSubobject<UFmPrimaryAttributeSet>(TEXT("PrimaryAttributeSet"));
	VitalAttributeSet = CreateDefaultSubobject<UFmVitalAttributeSet>(TEXT("VitalAttributeSet"));
}

void AFmNpcCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent) {
		AbilitySystemComponent->Init(this, this);
	}
	
	CustomMovementComponent = CastChecked<UFmCharacterMovementComponent>(GetCharacterMovement());
}

void AFmNpcCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority()) return;
	
	AiController = Cast<AFmAiController>(NewController);

	if (AiController && BehaviorTree)
	{
		if (const auto BlackboardComponent = AiController->GetBlackboardComponent())
		{
			if (const auto BlackboardAsset = BehaviorTree->BlackboardAsset)
			{
				BlackboardComponent->InitializeBlackboard(*BlackboardAsset);
				const auto Result = AiController->RunBehaviorTree(BehaviorTree);
			}
		}
	}
}

void AFmNpcCharacter::Attack() const
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilityByTag(TAG_Action_Combat_Attack_Unarmed_Jab);
	}
}

void AFmNpcCharacter::JumpToLocation(const FVector& TargetLocation, const float Duration)
{
	if (CustomMovementComponent)
	{
		const auto Velocity = UFmBlueprintFunctionLibrary::CalculateVelocity(GetActorLocation(), TargetLocation, Duration, CustomMovementComponent->GravityScale);
		SCREEN_LOG(Velocity.ToString(), 4.f)

		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation));
		LaunchCharacter(Velocity, true, true);
	}
}

void AFmNpcCharacter::Mantle()
{
	if (!CustomMovementComponent) return;
	
	CustomMovementComponent->TryMantle();
}

void AFmNpcCharacter::ToggleSprint(const bool bSprint) const
{
	if (!CustomMovementComponent) return;

	CustomMovementComponent->ToggleSprint(bSprint);
}

void AFmNpcCharacter::SetEmotionalState(const EFmNpcEmotionalState::Type InEmotionalState)
{
	EmotionalState = InEmotionalState;
}

void AFmNpcCharacter::SetTargetMoveLocation(const FVector& Location)
{
	TargetMoveLocation = Location;
	bHasTargetMoveLocation = true;
}

void AFmNpcCharacter::UnsetTargetMoveLocation()
{
	TargetMoveLocation = FVector();
	bHasTargetMoveLocation = false;
}
