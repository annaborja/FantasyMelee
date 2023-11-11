// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/FmCharacterMovementComponent.h"
#include "FmPlayerMovementComponent.generated.h"

class AFmPlayerCharacter;

UCLASS()
class FANTASYMELEE_API UFmPlayerMovementComponent : public UFmCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UFmPlayerMovementComponent();

	virtual void BeginPlay() override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	void ToggleCrouch();
	void ToggleCustomPressedJump(const bool bInCustomPressedJump);
	
protected:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

private:
	UPROPERTY(EditAnywhere, Category="FM Params|Jump")
	float JumpCooldownTime = 0.1f;
	UPROPERTY(EditAnywhere, Category="FM Params|Jump")
	float JumpFallGravityScale = 4.f;
	
	float BaseGravityScale = 2.f;
	FTimerHandle JumpCooldownTimerHandle;
	bool bCustomPressedJump = false;

	UFUNCTION()
	void AllowJump();
	UFUNCTION()
	void OnJumpApexReached();
};
