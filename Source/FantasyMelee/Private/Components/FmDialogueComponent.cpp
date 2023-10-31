// Copyright AB. All Rights Reserved.

#include "Components/FmDialogueComponent.h"

#include "AIController.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Characters/NPCs/FmMajorNpcCharacter.h"
#include "Characters/NPCs/FmNpcCharacter.h"
#include "Characters/Player/FmPlayerCharacter.h"
#include "Characters/Player/FmPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/FmHud.h"
#include "Utils/FmBlueprintFunctionLibrary.h"

UFmDialogueComponent::UFmDialogueComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// TODO(P0): Cache result (update whenever tags change).
FFmDialogueData* UFmDialogueComponent::GetAvailableDialogue(const AFmPlayerCharacter* Player) const
{
	if (!DialogueDataTable) return nullptr;
	
	TArray<FFmDialogueData*> OutRows;

	DialogueDataTable->GetAllRows<FFmDialogueData>(TEXT("Get all dialogue data rows"), OutRows);
	
	FGameplayTagContainer PlayerGameplayTags;
	Player->GetOwnedGameplayTags(PlayerGameplayTags);

	if (FFmDialogueData** DialogueDataPtr = OutRows.FindByPredicate([PlayerGameplayTags](const FFmDialogueData* DialogueData)
	{
		for (const auto& TagSpec : DialogueData->GameplayTagRequirements.PlayerTagSpecs)
		{
			if (!PlayerGameplayTags.HasTagExact(TagSpec.Tag)) return false;
		}

		return true;
	}))
	{
		return *DialogueDataPtr;
	}

	return nullptr;
}

bool UFmDialogueComponent::HasAvailableDialogue(const AFmPlayerCharacter* Player) const
{
	return GetAvailableDialogue(Player) != nullptr;
}

void UFmDialogueComponent::EndDialogue(AFmPlayerController* PlayerController, const AFmHud* Hud) const
{
	Hud->CloseDialogueFlow();

	PlayerController->SetViewTargetWithBlend(PlayerController->GetCharacter(),
		DialogueCameraBlendTime, VTBlend_Linear, 0, true);

	if (const auto World = GetWorld())
	{
		if (DialogueCameraPlayerShoulder) World->DestroyActor(DialogueCameraPlayerShoulder);
	}

	if (const auto Character = Cast<ACharacter>(GetOwner()))
	{
		if (const auto AiController = Cast<AAIController>(Character->GetController()))
		{
			DialogueEndedDelegate.Broadcast(AiController);
		}
	}
}

void UFmDialogueComponent::StartDialogue(AFmPlayerController* PlayerController, AFmHud* Hud)
{
	const auto Player = Cast<AFmPlayerCharacter>(PlayerController->GetCharacter());
	
	if (!Owner || !Player) return;
	
	// Make the actors face each other.
	const auto NpcLocation = Owner->GetActorLocation();
	const auto PlayerLocation = Player->GetActorLocation();
	
	// TODO(P0): Lerp the rotation.
	Player->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(PlayerLocation, NpcLocation));
	Owner->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(NpcLocation, PlayerLocation));

	if (const auto World = GetWorld())
	{
		// Create an over-the-shoulder dialogue camera.
		const auto CameraPlayerShoulderLocation = PlayerLocation +
			Player->GetActorForwardVector() * -40 + Player->GetActorRightVector() * 50 + Player->GetActorUpVector() * 70;
		const auto CameraPlayerShoulderRotation = UKismetMathLibrary::FindLookAtRotation(
			CameraPlayerShoulderLocation, NpcLocation + Owner->GetActorUpVector() * 50);

		DialogueCameraPlayerShoulder = CastChecked<ACameraActor>(World->SpawnActor(ACameraActor::StaticClass(),
			&CameraPlayerShoulderLocation, &CameraPlayerShoulderRotation));
		DialogueCameraPlayerShoulder->GetCameraComponent()->SetConstraintAspectRatio(false);

		PlayerController->SetViewTargetWithBlend(DialogueCameraPlayerShoulder, DialogueCameraBlendTime);
	}

	if (const auto DialogueData = GetAvailableDialogue(Player))
	{
		if (const auto FirstDialogueStepRowHandle = DialogueData->PotentialFirstDialogueSteps.FindByPredicate([](const FDataTableRowHandle& Row)
			{
				return true;
			}))
		{
			if (const auto FirstDialogueStep = FirstDialogueStepRowHandle->GetRow<FFmDialogueStepData>(TEXT("DialogueStepData")))
			{
				// Display the dialogue UI.
				Hud->OpenDialogueFlow();
				Hud->DisplayDialogueStep(Owner, *FirstDialogueStep);
			}
		}
	}
}

void UFmDialogueComponent::AdvanceDialogueStep(const FFmDialogueStepData DialogueStepData, AFmPlayerController* PlayerController, const AFmHud* Hud) const
{
	UFmBlueprintFunctionLibrary::ProcessEntityTagSpecGrants(this, DialogueStepData.GameplayTagGrants);
	
	if (DialogueStepData.PotentialDialogueOptions.Num() > 0)
	{
		if (!Owner) return;
		
		TArray<FFmDialogueOptionData> DialogueOptions;

		for (auto& Elem : DialogueStepData.PotentialDialogueOptions)
		{
			DialogueOptions.Add(*Elem.GetRow<FFmDialogueOptionData>(TEXT("DialogueOptionData")));
		}
		
		Hud->DisplayDialogueOptions(Owner, DialogueOptions);
	} else
	{
		EndDialogue(PlayerController, Hud);
	}
}

void UFmDialogueComponent::SelectDialogueOption(const FFmDialogueOptionData DialogueOptionData, AFmPlayerController* PlayerController, const AFmHud* Hud) const
{
	if (const auto RowHandle = DialogueOptionData.PotentialNextDialogueSteps.FindByPredicate([](const FDataTableRowHandle& Elem)
		{
			return true;
		}))
	{
		if (!Owner) return;
		
		Hud->DisplayDialogueStep(Owner, *RowHandle->GetRow<FFmDialogueStepData>(TEXT("DialogueStepData")));
	} else
	{
		EndDialogue(PlayerController, Hud);
	}
}

void UFmDialogueComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = CastChecked<AFmMajorNpcCharacter>(GetOwner());
}
