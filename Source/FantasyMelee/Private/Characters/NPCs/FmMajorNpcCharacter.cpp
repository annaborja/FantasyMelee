// Copyright AB. All Rights Reserved.

#include "Characters/NPCs/FmMajorNpcCharacter.h"

#include "Characters/Player/FmPlayerController.h"
#include "Components/FmDialogueComponent.h"
#include "Components/FmInventoryComponent.h"
#include "Components/SphereComponent.h"
#include "FantasyMelee/FantasyMeleeGameModeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/Constants.h"
#include "Utils/FmBlueprintFunctionLibrary.h"

bool AFmMajorNpcCharacter::GrantTagSpec(AFantasyMeleeGameModeBase* GameMode, const FGameplayTag& NpcTagId, const FFmTagSpec& TagSpec, const bool bDebug)
{
	if (bDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s GrantTagSpec: %s (%d)"), *NpcTagId.ToString(), *TagSpec.Tag.ToString(), TagSpec.Count)
	}

	if (TagSpec.Tag.IsValid())
	{
		bool bSuccess = false;

		if (TagSpec.Count > 0)
		{
			if (UFmInventoryComponent::IsItemTag(TagSpec.Tag)) {
				// TODO(P0): Implement.
				// bSuccess = InventoryComponent->AddItem(TagSpec.Tag, TagSpec.Count);
			} else if (auto& CharacterTags = GameMode->GetNpcCharacterTags(NpcTagId); !CharacterTags.HasTagExact(TagSpec.Tag))
			{
				CharacterTags.AddTag(TagSpec.Tag);
				bSuccess = true;
			}
		} else if (TagSpec.Count < 0)
		{
			if (UFmInventoryComponent::IsItemTag(TagSpec.Tag)) {
				// TODO(P0): Implement.
				// bSuccess = InventoryComponent->RemoveItem(TagSpec.Tag, TagSpec.Count);
			} else if (auto& CharacterTags = GameMode->GetNpcCharacterTags(NpcTagId); CharacterTags.HasTagExact(TagSpec.Tag))
			{
				CharacterTags.RemoveTag(TagSpec.Tag);
				bSuccess = true;
			}
		}

		return bSuccess;
	}

	return false;
}

AFmMajorNpcCharacter::AFmMajorNpcCharacter()
{
	DialogueComponent = CreateDefaultSubobject<UFmDialogueComponent>(TEXT("DialogueComponent"));
	
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(GetRootComponent());
	InteractionSphere->SetSphereRadius(InteractionSphereRadius);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(TRACE_CHANNEL_INTERACTION, ECR_Block);
}

void AFmMajorNpcCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	check(TagId.IsValid());
	
	if (const auto GameMode = UFmBlueprintFunctionLibrary::GetGameMode<AFantasyMeleeGameModeBase>(GetWorld()))
	{
		if (const auto MajorNpcDataTable = GameMode->GetMajorNpcDataTable())
		{
			TArray<FFmMajorNpcData*> AllRows;
			MajorNpcDataTable->GetAllRows<FFmMajorNpcData>(TEXT("MajorNpcDataTable"), AllRows);

			if (const auto FoundRow = AllRows.FindByPredicate([this](const FFmMajorNpcData* Row)
			{
				return Row->Tag.MatchesTagExact(TagId);
			}); FoundRow != nullptr)
			{
				MajorNpcData = **FoundRow;
			}
		}
	}
}

void AFmMajorNpcCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const auto GameMode = Cast<AFantasyMeleeGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		TagContainer.AppendTags(GameMode->GetNpcCharacterTags(TagId));
	}
}

bool AFmMajorNpcCharacter::CanInteract(AFmPlayerCharacter* Player) const
{
	return DialogueComponent->HasAvailableDialogue(Player);
}

void AFmMajorNpcCharacter::Interact(AFmPlayerController* PlayerController, AFmHud* Hud)
{
	if (const auto Player = Cast<AFmPlayerCharacter>(PlayerController->GetCharacter()))
	{
		if (DialogueComponent->HasAvailableDialogue(Player))
		{
			DialogueComponent->StartDialogue(PlayerController, Hud);
		}
	}
}

FText AFmMajorNpcCharacter::GetInGameName() const
{
	return MajorNpcData.Name;
}

void AFmMajorNpcCharacter::ToggleSprint(const bool bSprint) const
{
	if (const auto CharacterMovement = GetCharacterMovement())
	{
		if (bSprint)
		{
			CharacterMovement->MaxWalkSpeed += 200.f;
		} else
		{
			CharacterMovement->MaxWalkSpeed -= 200.f;
		}
	}
}

// TODO(P0): Figure out how actors can be affected by secondary effects if tags are only added to the game mode map.
void AFmMajorNpcCharacter::GrantTagSpec(const FFmTagSpec& TagSpec)
{
	if (const auto GameMode = Cast<AFantasyMeleeGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		GrantTagSpec(GameMode, TagId, TagSpec, bDebugGrantTagSpec);
	}
}

void AFmMajorNpcCharacter::AdvanceDialogueStep(const FFmDialogueStepData& DialogueStepData, AFmPlayerController* PlayerController, const AFmHud* Hud) const
{
	DialogueComponent->AdvanceDialogueStep(DialogueStepData, PlayerController, Hud);
}

void AFmMajorNpcCharacter::SelectDialogueOption(const FFmDialogueOptionData& DialogueOptionData, AFmPlayerController* PlayerController, const AFmHud* Hud) const
{
	DialogueComponent->SelectDialogueOption(DialogueOptionData, PlayerController, Hud);
}
