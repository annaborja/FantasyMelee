// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FmCharacterMovementComponent.generated.h"

class AFmPlayerCharacter;
class AFmNpcCharacter;
class AFmCharacter;

UENUM(BlueprintType)
namespace EFmCustomMovementMode
{
	enum Type : uint8
	{
		None
	};
}

UCLASS()
class FANTASYMELEE_API UFmCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UFmCharacterMovementComponent();

	virtual void BeginPlay() override;
	virtual float GetMaxSpeed() const override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;
	
	void ToggleSprint(const bool bInWantsToSprint);
	bool TryMantle();

protected:
	UPROPERTY(EditDefaultsOnly, Category="FM Assets")
	TObjectPtr<UAnimMontage> MantlesMontage;
	
	UPROPERTY(EditAnywhere, Category="FM Debug")
	bool bDebugMantle = false;

	UPROPERTY(EditAnywhere, Category="FM Params|Mantle")
	uint8 NumMantleFrontTraceDivisions = 8;
	UPROPERTY(EditAnywhere, Category="FM Params|Mantle")
	float MantleMinDistanceOffset = 30.f;
	UPROPERTY(EditAnywhere, Category="FM Params|Mantle")
	float MantleMaxDistance = 200.f;
	UPROPERTY(EditAnywhere, Category="FM Params|Mantle")
	float MantleReachHeight = 50.f;
	UPROPERTY(EditAnywhere, Category="FM Params|Mantle")
	float MinMantleDepth = 30.f;
	/** The minimum valid angle between the world up vector and the surface impact normal. */
	UPROPERTY(EditAnywhere, Category="FM Params|Mantle")
	float MantleMinWallSteepnessAngle = 75.f;
	/** The maximum valid angle between the character's velocity forward vector and the inverse surface impact normal. */
	UPROPERTY(EditAnywhere, Category="FM Params|Mantle")
	float MantleMaxSurfaceAngle = 40.f;
	UPROPERTY(EditAnywhere, Category="FM Params|Mantle")
	float MantleMaxAlignmentAngle = 45.f;

	UPROPERTY(EditAnywhere, Category="FM Params|Sprint")
	float MaxSprintSpeed = 900.f;
	
	UPROPERTY(Transient)
	TObjectPtr<AFmCharacter> Character;
	UPROPERTY(Transient)
	TObjectPtr<AFmNpcCharacter> Npc;
	UPROPERTY(Transient)
	TObjectPtr<AFmPlayerCharacter> Player;
	
	bool bWantsToSprint = false;

	// TODO
	bool bHadAnimRootMotion = false;
	bool bTransitionFinished = false;
	bool Safe_bTransitionFinished;
	TSharedPtr<FRootMotionSource_MoveToForce> TransitionRMS;
	FString TransitionName;
	UPROPERTY(Transient) UAnimMontage* TransitionQueuedMontage;
	FName MantleMontageSectionName;
	float TransitionQueuedMontageSpeed;
	int TransitionRMS_ID;
	//
	
	bool IsCustomMovementModeActive(const EFmCustomMovementMode::Type InCustomMovementMode) const;
	FVector GetMantleStartLocation(const FHitResult& FrontHit, const FHitResult& SurfaceHit, const float CapsuleHalfHeight, const float CapsuleRadius, const bool bTallMantle) const;
};
