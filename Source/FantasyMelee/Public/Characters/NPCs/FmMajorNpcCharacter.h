// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/NPCs/FmNpcCharacter.h"
#include "Engine/DataTable.h"
#include "Interfaces/FmInteractable.h"
#include "Interfaces/FmNameable.h"
#include "Interfaces/FmTagIdable.h"
#include "Interfaces/FmTagSpecGrantable.h"
#include "FmMajorNpcCharacter.generated.h"

class AFantasyMeleeGameModeBase;
struct FFmDialogueOptionData;
struct FFmDialogueStepData;
class USphereComponent;
class UFmDialogueComponent;

USTRUCT()
struct FFmMajorNpcData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag Tag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly);
	FText Name;
};

UCLASS()
class FANTASYMELEE_API AFmMajorNpcCharacter : public AFmNpcCharacter, public IGameplayTagAssetInterface,
	public IFmInteractable, public IFmNameable, public IFmTagIdable, public IFmTagSpecGrantable
{
	GENERATED_BODY()

public:
	static bool GrantTagSpec(AFantasyMeleeGameModeBase* GameMode, const FGameplayTag& NpcTagId, const FFmTagSpec& TagSpec, const bool bDebug = false);
	
	AFmMajorNpcCharacter();
	
	virtual void BeginPlay() override;

	virtual FGameplayTag GetTagId() const override { return TagId; }

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	virtual bool CanInteract(AFmPlayerCharacter* Player) const override;
	virtual void Interact(AFmPlayerController* PlayerController, AFmHud* Hud) override;

	virtual FText GetInGameName() const override;

	UFUNCTION(BlueprintCallable)
	virtual void GrantTagSpec(const FFmTagSpec& TagSpec) override;

	void AdvanceDialogueStep(const FFmDialogueStepData& DialogueStepData, AFmPlayerController* PlayerController, const AFmHud* Hud) const;
	void SelectDialogueOption(const FFmDialogueOptionData& DialogueOptionData, AFmPlayerController* PlayerController, const AFmHud* Hud) const;

	FORCEINLINE UFmDialogueComponent* GetDialogueComponent() const { return DialogueComponent; }

private:
	UPROPERTY(VisibleAnywhere, Category="Interaction")
	TObjectPtr<UFmDialogueComponent> DialogueComponent;
	UPROPERTY(VisibleAnywhere, Category="Interaction")
	TObjectPtr<USphereComponent> InteractionSphere;
	UPROPERTY(VisibleAnywhere, Category="Interaction")
	float InteractionSphereRadius = 100.f;
	
	UPROPERTY(EditAnywhere, Category="FM Debug")
	bool bDebugGrantTagSpec = false;
	
	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTag TagId;

	FFmMajorNpcData MajorNpcData;

	void OnTagSpecGrant(const FFmTagSpec& TagSpec) const;
};
