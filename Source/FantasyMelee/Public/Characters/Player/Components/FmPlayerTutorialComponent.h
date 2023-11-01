// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "FmPlayerTutorialComponent.generated.h"

class AFmPlayerCharacter;

USTRUCT(BlueprintType)
struct FFmTutorialData : public FTableRowBase
{
	GENERATED_BODY();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag TagId;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Instructions;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FANTASYMELEE_API UFmPlayerTutorialComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static bool IsTutorialTag(const FGameplayTag& Tag);
	
	UFmPlayerTutorialComponent();

	virtual void BeginPlay() override;

	bool GrantTutorial(const FGameplayTag& TutorialTag, const AFmPlayerCharacter* Player) const;

private:
	UPROPERTY(EditDefaultsOnly, Category="FM Data", meta=(RowType="/Script/FantasyMelee.FmTutorialData"))
	TObjectPtr<UDataTable> TutorialDataTable;
	
	const FFmTutorialData* GetTutorialByTag(const FGameplayTag& Tag) const;
};
