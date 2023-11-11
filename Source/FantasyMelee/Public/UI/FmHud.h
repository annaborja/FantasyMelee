// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/HUD.h"
#include "FmHud.generated.h"

class AFmMajorNpcCharacter;
class AFmPlayerController;
struct FFmDialogueOptionData;
struct FFmDialogueStepData;
struct FFmGrantedQuest;
struct FFmQuestData;
struct FFmQuestStepData;
struct FFmTutorialData;
struct FGameplayAttribute;
struct FGameplayTag;
class UFmActivatableWidget;
class UFmBaseAttributeSet;
class UFmDialogueOverlayWidget;
class UFmPersistentOverlayWidget;

UENUM(BlueprintType)
enum EFmQuestBroadcastType : uint8
{
	QuestGranted,
	QuestSucceeded,
	QuestFailed
};

USTRUCT(BlueprintType)
struct FFmMenuTabData : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Label;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFmHudAttributeValueSignature, const FGameplayTag&, Tag, const float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFmHudQuestSignature, const FFmQuestData&, Quest, const EFmQuestBroadcastType, QuestBroadcastType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFmHudQuestStepSignature, const FFmQuestStepData&, QuestStep);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFmHudTargetInteractableSignature, const AActor*, TargetInteractable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFmHudTutorialSignature, const FFmTutorialData&, Tutorial);

UCLASS()
class FANTASYMELEE_API AFmHud : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FFmHudAttributeValueSignature AttributeValueDelegate;
	UPROPERTY(BlueprintAssignable)
	FFmHudQuestSignature QuestDelegate;
	UPROPERTY(BlueprintAssignable)
	FFmHudQuestStepSignature QuestStepDelegate;
	UPROPERTY(BlueprintAssignable)
	FFmHudTargetInteractableSignature TargetInteractableDelegate;
	UPROPERTY(BlueprintAssignable)
	FFmHudTutorialSignature TutorialDelegate;
	
	UFUNCTION(BlueprintCallable)
	TMap<FGameplayTag, int32> GetPlayerInventory() const;
	UFUNCTION(BlueprintCallable)
	TMap<FGameplayTag, FFmGrantedQuest> GetPlayerQuestsMap() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	void AdvanceDialogueStep(const AFmMajorNpcCharacter* Npc, const FFmDialogueStepData& DialogueStep) const;
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	void SelectDialogueOption(const AFmMajorNpcCharacter* Npc, const FFmDialogueOptionData& DialogueOption) const;

	FORCEINLINE TSubclassOf<UFmActivatableWidget> GetDialogueOverlayClass() const { return DialogueOverlayClass; }
	FORCEINLINE TSubclassOf<UFmActivatableWidget> GetMenuSwitcherClass() const { return MenuSwitcherClass; }
	
	void Init(AFmPlayerController* InPlayerController);

	void BroadcastQuest(const FFmQuestData& Quest, const EFmQuestBroadcastType QuestBroadcastType);
	void BroadcastQuestStep(const FFmQuestStepData& QuestStep);
	void BroadcastTargetInteractable(const AActor* TargetInteractable) const;
	void BroadcastTutorial(const FFmTutorialData& Tutorial) const;
	
	void OpenMainMenu() const;
	
	void CloseDialogueFlow() const;
	void DisplayDialogueOptions(const AFmMajorNpcCharacter* Npc, const TArray<FFmDialogueOptionData>& DialogueOptions) const;
	void DisplayDialogueStep(const AFmMajorNpcCharacter* Npc, const FFmDialogueStepData& DialogueStep) const;
	void OpenDialogueFlow();
	
	UFUNCTION(BlueprintCallable)
	void OnNotificationFadeInEnd();
	UFUNCTION(BlueprintCallable)
	void OnNotificationFadeOutStart();

private:
	UPROPERTY(EditDefaultsOnly, Category="FM Classes")
	TSubclassOf<UFmActivatableWidget> DialogueOverlayClass;
	UPROPERTY(EditDefaultsOnly, Category="FM Classes")
	TSubclassOf<UFmActivatableWidget> MenuSwitcherClass;
	UPROPERTY(EditDefaultsOnly, Category="FM Classes")
	TSubclassOf<UFmPersistentOverlayWidget> PersistentOverlayClass;
	
	UPROPERTY(Transient)
	TObjectPtr<AFmPlayerController> PlayerController;
	UPROPERTY(Transient)
	TObjectPtr<UFmDialogueOverlayWidget> DialogueOverlayWidget;
	UPROPERTY(Transient)
	TObjectPtr<UFmPersistentOverlayWidget> PersistentOverlayWidget;

	TArray<TFunction<void ()>> NotificationBroadcastQueue;
	bool bNotificationActive = false;

	void BroadcastAttributeValue(const FGameplayTag& Tag, const FGameplayAttribute& Attribute, const UFmBaseAttributeSet* AttributeSet) const;

	void ProcessNotificationQueue();
	void QueueNotificationBroadcast(const TFunction<void ()>& Fn);
};
