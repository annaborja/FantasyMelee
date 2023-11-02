// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FmPlayerMovementComponent.generated.h"

class AFmPlayerCharacter;

UENUM(BlueprintType)
enum EFmCustomMovementMode : uint8
{
	None
};

UCLASS()
class FANTASYMELEE_API UFmPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UFmPlayerMovementComponent();

	virtual void BeginPlay() override;
	virtual float GetMaxSpeed() const override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	void ToggleCrouch();
	void ToggleCustomPressedJump(const bool bInCustomPressedJump);
	void ToggleSprint(const bool bInWantsToSprint);
	
protected:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

private:
	UPROPERTY(EditAnywhere, Category="FM Assets|Mantle")
	UAnimMontage* TallMantleMontage;
	UPROPERTY(EditAnywhere, Category="FM Assets|Mantle")
	UAnimMontage* TransitionTallMantleMontage;
	UPROPERTY(EditAnywhere, Category="FM Assets|Mantle")
	UAnimMontage* ShortMantleMontage;
	UPROPERTY(EditAnywhere, Category="FM Assets|Mantle")
	UAnimMontage* TransitionShortMantleMontage;

	UPROPERTY(EditAnywhere, Category="FM Debug")
	bool bDebugMantle = false;
	
	UPROPERTY(EditAnywhere, Category="FM Params|Sprint")
	float MaxSprintSpeed = 900.f;
	
	UPROPERTY(EditAnywhere, Category="FM Params|Jump")
	float JumpCooldownTime = 0.1f;
	UPROPERTY(EditAnywhere, Category="FM Params|Jump")
	float JumpFallGravityScale = 4.f;

	UPROPERTY(EditAnywhere, Category="FM Params|Mantle")
	float MantleMaxDistance = 200.f;
	UPROPERTY(EditAnywhere, Category="FM Params|Mantle")
	float MantleReachHeight = 50.f;
	UPROPERTY(EditAnywhere, Category="FM Params|Mantle")
	float MinMantleDepth = 30.f;
	UPROPERTY(EditAnywhere, Category="FM Params|Mantle")
	float MantleMinWallSteepnessAngle = 75.f;
	UPROPERTY(EditAnywhere, Category="FM Params|Mantle")
	float MantleMaxSurfaceAngle = 40.f;
	UPROPERTY(EditAnywhere, Category="FM Params|Mantle")
	float MantleMaxAlignmentAngle = 45.f;
	
	UPROPERTY(VisibleInstanceOnly, Category="FM Runtime")
	bool bCustomPressedJump = false;
	UPROPERTY(VisibleInstanceOnly, Category="FM Runtime")
	bool bWantsToSprint = false;
	
	UPROPERTY(Transient)
	TObjectPtr<AFmPlayerCharacter> Player;
	
	FTimerHandle JumpCooldownTimerHandle;
	
	float BaseGravityScale = 2.f;
	float BaseMaxWalkSpeed = 600.f;

	// TODO
	bool bHadAnimRootMotion = false;
	bool bTransitionFinished = false;
	bool Safe_bTransitionFinished;
	TSharedPtr<FRootMotionSource_MoveToForce> TransitionRMS;
	FString TransitionName;
	UPROPERTY(Transient) UAnimMontage* TransitionQueuedMontage;
	float TransitionQueuedMontageSpeed;
	int TransitionRMS_ID;
	//
	
	UFUNCTION()
	void AllowJump();
	UFUNCTION()
	void OnJumpApexReached();
	
	bool IsCustomMovementModeActive(const EFmCustomMovementMode InCustomMovementMode) const;

	bool TryMantle();
	FVector GetMantleStartLocation(const FHitResult& FrontHit, const FHitResult& SurfaceHit, const bool bTallMantle) const;
};
