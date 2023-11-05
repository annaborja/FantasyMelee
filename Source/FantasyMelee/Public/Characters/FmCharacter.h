// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/FmCombatable.h"
#include "Interfaces/FmInventoriable.h"
#include "Interfaces/FmMotionWarpable.h"
#include "Utils/Structs.h"
#include "FmCharacter.generated.h"

UCLASS()
class FANTASYMELEE_API AFmCharacter : public ACharacter, public IFmCombatable, public IFmInventoriable, public IFmMotionWarpable
{
	GENERATED_BODY()

public:
	AFmCharacter();
	explicit AFmCharacter(const FObjectInitializer& ObjectInitializer) : AFmCharacter() {}
	
	virtual void BeginPlay() override;

	virtual UFmCombatComponent* GetCombatComponent() const override { return CombatComponent; }
	virtual FFmMontageData GetHitReactionMontageData() const override { return HitReactionMontageData; }

	virtual UFmInventoryComponent* GetInventoryComponent() const override { return InventoryComponent; };

	virtual UMotionWarpingComponent* GetMotionWarpingComponent() const override { return MotionWarpingComponent; }

	UFUNCTION(BlueprintCallable)
	void ToggleInjured(const bool bInIsInjured);

	FORCEINLINE bool IsInjured() const { return bIsInjured; }

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFmInventoryComponent> InventoryComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="FM Assets")
	FFmMontageData HitReactionMontageData;
	
	UPROPERTY(EditAnywhere, Category="FM Params")
	bool bIsInjured = false;
	
	UPROPERTY(Transient)
	TObjectPtr<UFmCombatComponent> CombatComponent;
};
