// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Utils/Structs.h"
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
	
	UPROPERTY(EditDefaultsOnly)
	FFmEntityTagSpec TagRequirements;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FANTASYMELEE_API UFmPlayerTutorialComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFmPlayerTutorialComponent();

	virtual void BeginPlay() override;

	void OnTagSpecGrant(const AFmPlayerCharacter* Player);

private:
	UPROPERTY(EditDefaultsOnly, Category="FM Data", meta=(RowType="/Script/FantasyMelee.FmTutorialData"))
	TObjectPtr<UDataTable> TutorialDataTable;

	// TODO(P0): Load from saved data.
	UPROPERTY(SaveGame, EditAnywhere, Category="FM Runtime")
	FGameplayTagContainer CompletedTutorialTags;
	
	UPROPERTY(Transient, VisibleInstanceOnly, Category="FM Runtime")
	TArray<FFmTutorialData> IncompleteTutorials;
};
