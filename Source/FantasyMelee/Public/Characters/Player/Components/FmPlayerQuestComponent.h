// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "Tags/InGameTags.h"
#include "Utils/Structs.h"
#include "FmPlayerQuestComponent.generated.h"

class AFmPlayerCharacter;

UENUM(BlueprintType)
enum class EFmQuestType : uint8
{
	MainQuest,
	SideQuest
};

UENUM(BlueprintType)
enum class EFmQuestOutcome : uint8
{
	None,
	Success,
	Failure
};

USTRUCT(BlueprintType)
struct FFmQuestData : public FTableRowBase
{
	GENERATED_BODY();
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag TagId;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EFmQuestType QuestType = EFmQuestType::MainQuest;
	
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec SuccessTagGrants;
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec FailureTagGrants;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FGameplayTag> PotentialFirstQuestStepTags;
};

USTRUCT(BlueprintType)
struct FFmQuestStepData : public FTableRowBase
{
	GENERATED_BODY();
	
	UPROPERTY(EditAnywhere)
	FGameplayTag TagId;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Instructions;
	
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec TagRequirements;
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec TagProhibitions;
	
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec SuccessTagRequirements;
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec SuccessTagProhibitions;
	
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec FailureTagRequirements = FFmEntityTagSpec(TArray { FFmTagSpec(TAG_InGame_Never) });
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec FailureTagProhibitions;
	
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec SuccessTagGrants;
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec FailureTagGrants;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FGameplayTag> PotentialNextQuestStepTags;
};

USTRUCT(BlueprintType)
struct FFmGrantedQuestStep
{
	GENERATED_BODY()

	FFmGrantedQuestStep() {}
	explicit FFmGrantedQuestStep(const uint8 InOrder) : Order(InOrder) {}
	explicit FFmGrantedQuestStep(const FFmQuestStepData& InQuestStepData, const uint8 InOrder) :
		QuestStepData(InQuestStepData), Order(InOrder) {}
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FFmQuestStepData QuestStepData;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	uint8 Order = 0;
};

USTRUCT(BlueprintType)
struct FFmGrantedQuest
{
	GENERATED_BODY()

	FFmGrantedQuest() {}
	explicit FFmGrantedQuest(const FFmQuestData& InQuestData) : QuestData(InQuestData) {}
	explicit FFmGrantedQuest(const FFmQuestData& InQuestData, const TArray<FFmGrantedQuestStep>& InGrantedQuestSteps, const EFmQuestOutcome InOutcome) :
		QuestData(InQuestData), GrantedQuestSteps(InGrantedQuestSteps), Outcome(InOutcome) {}

	UPROPERTY(BlueprintReadOnly)
	FFmQuestData QuestData;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<FFmGrantedQuestStep> GrantedQuestSteps;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	EFmQuestOutcome Outcome = EFmQuestOutcome::None;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FANTASYMELEE_API UFmPlayerQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static bool IsQuestTag(const FGameplayTag& Tag);
	static bool IsQuestStepTag(const FGameplayTag& Tag);
	
	UFmPlayerQuestComponent();

	virtual void BeginPlay() override;

	bool GrantQuest(const FGameplayTag& QuestTag, const AFmPlayerCharacter* Player);
	void OnTagSpecGrant(const AFmPlayerCharacter* Player);
	
	FORCEINLINE TMap<FGameplayTag, FFmGrantedQuest> GetGrantedQuestsMap() const { return GrantedQuestsMap; }

private:
	UPROPERTY(EditDefaultsOnly, Category="FM Data", meta=(RowType="/Script/FantasyMelee.FmQuestData"))
	TObjectPtr<UDataTable> QuestDataTable;
	UPROPERTY(EditDefaultsOnly, Category="FM Data", meta=(RowType="/Script/FantasyMelee.FmQuestStepData"))
	TObjectPtr<UDataTable> QuestStepDataTable;
	
	UPROPERTY(EditDefaultsOnly, Category="FM Debug")
	bool bDebugQuestGrants = false;
	
	// TODO(P0): Load from saved data.
	UPROPERTY(SaveGame, EditAnywhere, Category="FM Runtime")
	TMap<FGameplayTag, FFmGrantedQuest> GrantedQuestsMap;

	const FFmQuestData* GetQuestByTag(const FGameplayTag& Tag) const;
	const FFmQuestStepData* GetQuestStepByTag(const FGameplayTag& Tag) const;

	bool GrantQuestStepFromPotentialTags(FFmGrantedQuest* GrantedQuest, const TArray<FGameplayTag>& PotentialQuestStepTags, const AFmPlayerCharacter* Player) const;
	void SucceedQuest(FFmGrantedQuest& GrantedQuest, const AFmPlayerCharacter* Player) const;
	void SucceedQuestStep(FFmGrantedQuest& GrantedQuest, const FFmQuestStepData& QuestStep, const AFmPlayerCharacter* Player) const;
	void FailQuestStep(FFmGrantedQuest& GrantedQuest, const FFmQuestStepData& QuestStep, const AFmPlayerCharacter* Player) const;
};
