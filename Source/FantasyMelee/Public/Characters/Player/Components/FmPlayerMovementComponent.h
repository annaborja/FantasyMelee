// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FmPlayerMovementComponent.generated.h"

class AFmPlayerCharacter;

UENUM(BlueprintType)
enum EFmCustomMovementMode : uint8
{
	None,
	Slide
};

UCLASS()
class FANTASYMELEE_API UFmPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UFmPlayerMovementComponent();

	virtual void BeginPlay() override;
	virtual bool CanCrouchInCurrentState() const override;
	virtual bool IsMovingOnGround() const override;

	bool IsCustomMovementModeActive(const EFmCustomMovementMode InCustomMovementMode) const;

	void ToggleCrouch();
	void ToggleSprint(const bool bInWantsToSprint);

protected:
	virtual void InitializeComponent() override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<AFmPlayerCharacter> PlayerOwner;
	
	UPROPERTY(EditAnywhere, Category="FM Params")
	float BaseGravityScale = 2.f;
	UPROPERTY(EditAnywhere, Category="FM Params")
	float JumpFallGravityScale = 4.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Movement|Sprint")
	float SprintSpeedMultiplier = 1.5f;
	
	UPROPERTY(EditAnywhere, Category="FM Params")
	float JumpCooldownTime = 0.1f;

	float BaseMaxWalkSpeed;
	bool bWantsToSprint = false;

	FTimerHandle CanJumpResetTimerHandle;

	UFUNCTION()
	void EnableJump();
	UFUNCTION()
	void OnJumpApexReached();
};
