// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Characters/FmCharacter.h"
#include "GAS/FmAbilitySystemComponent.h"
#include "FmNpcCharacter.generated.h"

class UFmCharacterMovementComponent;
class AFmAiController;
class UBehaviorTree;
class UFmVitalAttributeSet;
class UFmPrimaryAttributeSet;
class UFmAbilitySystemComponent;

UENUM(BlueprintType)
namespace EFmNpcEmotionalState
{
	enum Type : uint8
	{
		Neutral,
		Scared
	};
}

UCLASS()
class FANTASYMELEE_API AFmNpcCharacter : public AFmCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	explicit AFmNpcCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	void Attack() const;
	
	UFUNCTION(BlueprintCallable)
	void JumpToLocation(const FVector& TargetLocation, const float Duration);
	UFUNCTION(BlueprintCallable)
	void Mantle();
	UFUNCTION(BlueprintCallable)
	void ToggleSprint(const bool bSprint) const;

	UFUNCTION(BlueprintCallable)
	void SetEmotionalState(const EFmNpcEmotionalState::Type InEmotionalState);

	void SetTargetMoveLocation(const FVector& Location);
	void UnsetTargetMoveLocation();
	FVector GetTargetMoveLocation() const { return TargetMoveLocation; }
	bool HasTargetMoveLocation() const { return bHasTargetMoveLocation; }

	FORCEINLINE EFmNpcEmotionalState::Type GetEmotionalState() const { return EmotionalState; }
	FORCEINLINE float GetFindTargetSphereRadius() const { return FindTargetSphereRadius; }
	FORCEINLINE bool ShouldDebugFindTargetSphere() const { return bDebugFindTargetSphere; }

protected:
	UPROPERTY(VisibleAnywhere, Category="GAS")
	TObjectPtr<UFmAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, Category="GAS|Attributes")
	TObjectPtr<UFmPrimaryAttributeSet> PrimaryAttributeSet;
	UPROPERTY(VisibleAnywhere, Category="GAS|Attributes")
	TObjectPtr<UFmVitalAttributeSet> VitalAttributeSet;

	UPROPERTY(VisibleInstanceOnly, Category="AI")
	TObjectPtr<AFmAiController> AiController;
	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FM Params")
	TEnumAsByte<EFmNpcEmotionalState::Type> EmotionalState = EFmNpcEmotionalState::Neutral;
	
	UPROPERTY(EditDefaultsOnly, Category="AI|Find Target")
	float FindTargetSphereRadius = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category="AI|Find Target")
	bool bDebugFindTargetSphere;

	FVector TargetMoveLocation;
	bool bHasTargetMoveLocation = false;
	
	UPROPERTY(Transient)
	TObjectPtr<UFmCharacterMovementComponent> CustomMovementComponent;
};
