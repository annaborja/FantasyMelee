// Copyright AB. All Rights Reserved.

#include "Characters/Player/FmPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "FmEnhancedInputComponent.h"
#include "Characters/Player/FmPlayerCharacter.h"
#include "Characters/Player/Components/FmPlayerCombatComponent.h"
#include "Characters/Player/Components/FmPlayerInteractionComponent.h"
#include "Characters/Player/Components/FmPlayerMovementComponent.h"
#include "Interfaces/FmInteractable.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/InGameTags.h"
#include "UI/FmHud.h"

void AFmPlayerController::Init() const
{
	ActivateInputMappingContext(BaseInputMappingContext);
}

void AFmPlayerController::OnTagSpecGrant(const FFmTagSpec& TagSpec) const
{
	if (TagSpec.Tag.MatchesTagExact(TAG_InGame_BlockInput))
	{
		if (TagSpec.Count > 0)
		{
			ActivateInputMappingContext(EmptyInputMappingContext);
		} else if (TagSpec.Count < 0)
		{
			ActivateInputMappingContext(BaseInputMappingContext);
		}
	}
}

void AFmPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(EmptyInputMappingContext);
	check(BaseInputMappingContext);
	check(CombatInputMappingContext);

	check(AttackHeavyInputAction);
	check(AttackLightInputAction);
	check(BlockInputAction);
	check(CrouchInputAction);
	check(InteractInputAction);
	check(JumpInputAction);
	check(LookInputAction);
	check(MoveInputAction);
	check(PauseGameInputAction);
	check(SprintInputAction);

	CustomPlayer = CastChecked<AFmPlayerCharacter>(GetCharacter());

	CustomHud = CastChecked<AFmHud>(GetHUD());
	CustomHud->Init(this);
}

void AFmPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	const auto EnhancedInputComponent = CastChecked<UFmEnhancedInputComponent>(InputComponent);
	
	EnhancedInputComponent->BindAction(PauseGameInputAction, ETriggerEvent::Completed, this, &ThisClass::PauseGame);
	
	EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	
	EnhancedInputComponent->BindAction(CrouchInputAction, ETriggerEvent::Started, this, &ThisClass::ToggleCrouch);
	EnhancedInputComponent->BindAction(InteractInputAction, ETriggerEvent::Started, this, &ThisClass::Interact);
	EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Started, this, &ThisClass::Jump);
	
	EnhancedInputComponent->BindAction(AttackHeavyInputAction, ETriggerEvent::Started, this, &ThisClass::AttackHeavy);
	EnhancedInputComponent->BindAction(AttackLightInputAction, ETriggerEvent::Started, this, &ThisClass::AttackLight);
	EnhancedInputComponent->BindAction(BlockInputAction, ETriggerEvent::Started, this, &ThisClass::Block);
	
	EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Started, this, &ThisClass::StartSprint);
	EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Completed, this, &ThisClass::StopSprint);
}

void AFmPlayerController::PauseGame(const FInputActionValue& InputActionValue)
{
	if (CustomHud) CustomHud->OpenMainMenu();
	
	UGameplayStatics::SetGamePaused(this, true);
}

void AFmPlayerController::Look(const FInputActionValue& InputActionValue)
{
	const auto InputActionVector = InputActionValue.Get<FVector2D>();

	AddPitchInput(InputActionVector.Y);
	AddYawInput(InputActionVector.X);
}

void AFmPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const auto InputActionVector = InputActionValue.Get<FVector2D>();
	const FRotationMatrix RotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f));

	if (CustomPlayer)
	{
		CustomPlayer->AddMovementInput(RotationMatrix.GetUnitAxis(EAxis::X), InputActionVector.Y);
		CustomPlayer->AddMovementInput(RotationMatrix.GetUnitAxis(EAxis::Y), InputActionVector.X);
	}
}

void AFmPlayerController::ToggleCrouch(const FInputActionValue& InputActionValue)
{
	if (!CustomPlayer) return;
	
	if (const auto PlayerMovementComponent = CustomPlayer->GetPlayerMovementComponent())
	{
		PlayerMovementComponent->ToggleCrouch();
	}
}

void AFmPlayerController::Interact(const FInputActionValue& InputActionValue)
{
	if (CustomPlayer && CustomHud)
	{
		if (const auto PlayerInteractionComponent = CustomPlayer->GetPlayerInteractionComponent())
		{
			if (const auto TargetInteractable = Cast<IFmInteractable>(PlayerInteractionComponent->GetTargetInteractable()))
			{
				TargetInteractable->Interact(this, CustomHud);
			}
		}
	}
}

void AFmPlayerController::Jump(const FInputActionValue& InputActionValue)
{
	if (!CustomPlayer) return;
	
	CustomPlayer->Jump();
}

void AFmPlayerController::AttackHeavy(const FInputActionValue& InputActionValue)
{
	if (!CustomPlayer) return;
	
	if (const auto PlayerCombatComponent = CustomPlayer->GetPlayerCombatComponent())
	{
		PlayerCombatComponent->ExecuteHeavyAttack();
	}
}

void AFmPlayerController::AttackLight(const FInputActionValue& InputActionValue)
{
	if (!CustomPlayer) return;
	
	if (const auto PlayerCombatComponent = CustomPlayer->GetPlayerCombatComponent())
	{
		PlayerCombatComponent->ExecuteLightAttack();
	}
}

void AFmPlayerController::Block(const FInputActionValue& InputActionValue)
{
	if (!CustomPlayer) return;
	
	if (const auto PlayerCombatComponent = CustomPlayer->GetPlayerCombatComponent())
	{
		PlayerCombatComponent->ExecuteBlock();
	}
}

void AFmPlayerController::StartSprint(const FInputActionValue& InputActionValue)
{
	if (!CustomPlayer) return;
	
	if (const auto PlayerMovementComponent = CustomPlayer->GetPlayerMovementComponent())
	{
		PlayerMovementComponent->ToggleSprint(true);
	}
}

void AFmPlayerController::StopSprint(const FInputActionValue& InputActionValue)
{
	if (!CustomPlayer) return;
	
	if (const auto PlayerMovementComponent = CustomPlayer->GetPlayerMovementComponent())
	{
		PlayerMovementComponent->ToggleSprint(false);
	}
}

void AFmPlayerController::ActivateInputMappingContext(const UInputMappingContext* InputMappingContext, const int32 Priority, const bool bClearExisting) const
{
	if (const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (bClearExisting) Subsystem->ClearAllMappings();
		
		Subsystem->AddMappingContext(InputMappingContext, Priority);
	}
}
