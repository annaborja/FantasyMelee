// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "Characters/FmCharacter.h"
#include "Interfaces/FmTagSpecGrantable.h"
#include "FmPlayerCharacter.generated.h"

class AFmHud;
class AFmPlayerController;
class UCameraComponent;
class UFmPlayerCombatComponent;
class UFmPlayerInteractionComponent;
class UFmPlayerMovementComponent;
class UFmPlayerQuestComponent;
class UFmPlayerTutorialComponent;
class USpringArmComponent;

UCLASS()
class FANTASYMELEE_API AFmPlayerCharacter : public AFmCharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface, public IFmTagSpecGrantable
{
	GENERATED_BODY()

public:
	explicit AFmPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;
	virtual void Jump() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	virtual void GrantTagSpec(const FFmTagSpec& TagSpec) override;

	void EnableJump();
	AFmHud* GetCustomHud() const;
	
	FORCEINLINE AFmPlayerController* GetCustomController() const { return CustomPlayerController; }
	FORCEINLINE UFmPlayerCombatComponent* GetPlayerCombatComponent() const { return PlayerCombatComponent; }
	FORCEINLINE UFmPlayerInteractionComponent* GetPlayerInteractionComponent() const { return InteractionComponent; }
	FORCEINLINE UFmPlayerQuestComponent* GetPlayerQuestComponent() const { return QuestComponent; }
	FORCEINLINE UFmPlayerMovementComponent* GetPlayerMovementComponent() const { return PlayerMovementComponent; }
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FollowCamera;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> FollowCameraSpringArm;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFmPlayerCombatComponent> PlayerCombatComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFmPlayerInteractionComponent> InteractionComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFmPlayerQuestComponent> QuestComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFmPlayerTutorialComponent> TutorialComponent;
	
	UPROPERTY(EditAnywhere, Category="FM Debug")
	bool bDebugGrantTagSpec = false;
	
	// TODO(P0): Load from saved data.
	UPROPERTY(SaveGame, EditAnywhere, Category="FM Runtime")
	FGameplayTagContainer CharacterTags;
	UPROPERTY(VisibleInstanceOnly, Category="FM Runtime")
	bool bCanJump = true;

	UPROPERTY(Transient)
	TObjectPtr<AFmPlayerController> CustomPlayerController;
	UPROPERTY(Transient)
	TObjectPtr<UFmPlayerMovementComponent> PlayerMovementComponent;

	void OnTagSpecGrant(const FFmTagSpec& TagSpec);
};
