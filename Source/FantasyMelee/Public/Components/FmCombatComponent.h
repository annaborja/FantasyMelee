// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "FmCombatComponent.generated.h"

class UGameplayAbility;

USTRUCT()
struct FFmAbilityExecutionRecord
{
	GENERATED_BODY()

	FFmAbilityExecutionRecord() {}

	explicit FFmAbilityExecutionRecord(const FGameplayTag& InTagId, const double InTimestamp = FPlatformTime::Seconds()) :
		Timestamp(InTimestamp), TagId(InTagId) {}

	UPROPERTY(VisibleAnywhere)
	double Timestamp = FPlatformTime::Seconds();
	
	UPROPERTY(VisibleAnywhere)
	FGameplayTag TagId;
};

USTRUCT()
struct FFmHitBoxData : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditDefaultsOnly)
	FVector StartOffsetLocation;
	UPROPERTY(EditDefaultsOnly)
	FVector EndOffsetLocation;
	
	UPROPERTY(EditDefaultsOnly)
	float HalfSize = 0.f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FANTASYMELEE_API UFmCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static bool IsCombatAbility(const UGameplayAbility* Ability);
	static bool IsQueueableAbility(const UGameplayAbility* Ability);
	
	static void TriggerHitReactionAbility(const FHitResult& HitResult);
	
	UFmCombatComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ExecuteHeavyAttack() const;
	void ExecuteLightAttack() const;
	void ExecuteBlock() const;

	AActor* FindSoftTarget(const float TraceLength) const;
	FName GetHitReactionMontageSectionName(const FHitResult& HitResult) const;
	
	void AddActiveHitBox(const FDataTableRowHandle& HitBoxDataRowHandle);
	void RemoveActiveHitBox(const FDataTableRowHandle& HitBoxDataRowHandle);

	void SetCancellableAbility(const FGameplayAbilitySpecHandle& AbilitySpecHandle);
	void UnsetCancellableAbility(const FGameplayAbilitySpecHandle& AbilitySpecHandle);
	
private:
	UPROPERTY(VisibleAnywhere, Category="Combat|Abilities")
	TArray<FFmAbilityExecutionRecord> AbilityExecutionRecords;
	UPROPERTY(EditDefaultsOnly, Category="Combat|Hit Boxes")
	uint8 MaxNumAbilityExecutionRecords = 4;
	UPROPERTY(VisibleAnywhere, Category="Combat|Abilities")
	TObjectPtr<const UGameplayAbility> QueuedAbility;
	
	UPROPERTY(VisibleAnywhere, Category="Combat|Hit Boxes")
	TMap<FName, int32> ActiveHitBoxCountMap;
	UPROPERTY(VisibleAnywhere, Category="Combat|Hit Boxes")
	TArray<const AActor*> HitActors;
	UPROPERTY(EditDefaultsOnly, Category="Combat|Hit Boxes")
	TEnumAsByte<EDrawDebugTrace::Type> HitBoxDrawDebugTrace = EDrawDebugTrace::None;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat|Targeting")
	TEnumAsByte<EDrawDebugTrace::Type> SoftTargetDrawDebugTrace = EDrawDebugTrace::None;
	UPROPERTY(EditDefaultsOnly, Category="Combat|Targeting")
	bool bDebugSoftTarget = false;
	
	FGameplayAbilitySpecHandle CancellableAbilitySpecHandle;
	FGameplayAbilitySpecHandle PrimaryActiveAbilitySpecHandle;

	UFUNCTION()
	void OnAbilityEnded(UGameplayAbility* Ability);
	UFUNCTION()
	void OnAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& TagContainer);
	UFUNCTION()
	void OnAbilityStarted(UGameplayAbility* Ability);
	
	void CleanUpAbilityExecutionRecords(const uint8 BufferSize = 0);
	void CleanUpAbilitySpecificData(const UGameplayAbility* Ability);
	void HandleAbilityCancellation();
	void HandleAbilityQueue(const UGameplayAbility* FinishedAbility);

	void ApplyDamageEffect(const FHitResult& HitResult) const;
	void HandleActiveHitBoxes();
	void HandleActiveHitBox(const FName& HitBoxName, const UDataTable* HitBoxDataTable);
	void HandleHitResult(const FHitResult& HitResult);
};
