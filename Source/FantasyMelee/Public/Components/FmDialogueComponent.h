// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/FmPlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "FmDialogueComponent.generated.h"

class UDialogueWave;
class AAIController;
class AFmMajorNpcCharacter;
class AFmPlayerCharacter;
class AFmNpcCharacter;
class AFmPlayerController;
class AFmHud;

USTRUCT(BlueprintType)
struct FFmDialogueData : public FTableRowBase
{
	GENERATED_BODY();
	
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec TagRequirements;
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec TagProhibitions;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FDataTableRowHandle> PotentialFirstDialogueSteps;
};

USTRUCT(BlueprintType)
struct FFmDialogueLine
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UDialogueWave> DialogueWave;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FFmMontageData MontageData;
};

USTRUCT(BlueprintType)
struct FFmDialogueStepData : public FTableRowBase
{
	GENERATED_BODY();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FFmDialogueLine> DialogueLines;
	
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec TagRequirements;
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec TagProhibitions;
	
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec TagGrants;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FDataTableRowHandle> PotentialDialogueOptions;
};

USTRUCT(BlueprintType)
struct FFmDialogueOptionData : public FTableRowBase
{
	GENERATED_BODY();
	
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec GameplayTagRequirements;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText PlayerDialogue;
	
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec GameplayTagGrants;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FDataTableRowHandle> PotentialNextDialogueSteps;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFmDialogueComponentDialogueEndedSignature, AAIController*, AiController);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FANTASYMELEE_API UFmDialogueComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FFmDialogueComponentDialogueEndedSignature DialogueEndedDelegate;
	
	UFmDialogueComponent();

	FFmDialogueData* GetAvailableDialogue(const AFmPlayerCharacter* Player) const;
	bool HasAvailableDialogue(const AFmPlayerCharacter* Player) const;
	
	void EndDialogue(AFmPlayerController* PlayerController, const AFmHud* Hud) const;
	void StartDialogue(AFmPlayerController* PlayerController, AFmHud* Hud);
	
	void AdvanceDialogueStep(const FFmDialogueStepData DialogueStepData, AFmPlayerController* PlayerController, const AFmHud* Hud) const;
	void SelectDialogueOption(const FFmDialogueOptionData DialogueOptionData, AFmPlayerController* PlayerController, const AFmHud* Hud) const;

protected:
	virtual void BeginPlay() override;

private:
	TObjectPtr<ACameraActor> DialogueCameraPlayerShoulder;
	TObjectPtr<AFmMajorNpcCharacter> Owner;
	
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float DialogueCameraBlendTime = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category="Dialogue")
	TObjectPtr<UDataTable> DialogueDataTable;
};
