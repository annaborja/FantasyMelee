// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "Utils/Structs.h"
#include "FmPlayerController.generated.h"

class AFmHud;
class AFmPlayerCharacter;
struct FFmTagSpec;
struct FGameplayTag;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

UCLASS()
class FANTASYMELEE_API AFmPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void Init() const;
	void OnTagSpecGrant(const FFmTagSpec& TagSpec) const;

	FORCEINLINE AFmHud* GetCustomHud() const { return CustomHud; }
	FORCEINLINE AFmPlayerCharacter* GetPlayer() const { return CustomPlayer; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> EmptyInputMappingContext;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> BaseInputMappingContext;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> CombatInputMappingContext;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> AttackHeavyInputAction;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> AttackLightInputAction;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> BlockInputAction;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> CrouchInputAction;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InteractInputAction;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> JumpInputAction;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> LookInputAction;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> MoveInputAction;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> PauseGameInputAction;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> SprintInputAction;

	UPROPERTY(Transient)
	TObjectPtr<AFmPlayerCharacter> CustomPlayer;
	UPROPERTY(Transient)
	TObjectPtr<AFmHud> CustomHud;

	void PauseGame(const FInputActionValue& InputActionValue);
	
	void Look(const FInputActionValue& InputActionValue);
	void Move(const FInputActionValue& InputActionValue);
	
	void ToggleCrouch(const FInputActionValue& InputActionValue);
	void Interact(const FInputActionValue& InputActionValue);
	void Jump(const FInputActionValue& InputActionValue);
	
	void AttackHeavy(const FInputActionValue& InputActionValue);
	void AttackLight(const FInputActionValue& InputActionValue);
	void Block(const FInputActionValue& InputActionValue);
	
	void StartSprint(const FInputActionValue& InputActionValue);
	void StopSprint(const FInputActionValue& InputActionValue);
	
	void ActivateInputMappingContext(const UInputMappingContext* InputMappingContext, const int32 Priority = 0, const bool bClearExisting = true) const;
};
