// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Characters/FmCharacter.h"
#include "GAS/FmAbilitySystemComponent.h"
#include "FmNpcCharacter.generated.h"

class AFmAiController;
class UBehaviorTree;
class UFmVitalAttributeSet;
class UFmPrimaryAttributeSet;
class UFmAbilitySystemComponent;

UCLASS()
class FANTASYMELEE_API AFmNpcCharacter : public AFmCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AFmNpcCharacter();

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	void Attack() const;

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
	
	UPROPERTY(EditDefaultsOnly, Category="AI|Find Target")
	float FindTargetSphereRadius = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category="AI|Find Target")
	bool bDebugFindTargetSphere;
};
