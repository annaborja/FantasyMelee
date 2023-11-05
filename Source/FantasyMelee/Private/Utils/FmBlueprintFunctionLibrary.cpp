// Copyright AB. All Rights Reserved.

#include "Utils/FmBlueprintFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagAssetInterface.h"
#include "Animation/FmAnimInstance.h"
#include "Characters/NPCs/FmMajorNpcCharacter.h"
#include "Characters/Player/FmPlayerCharacter.h"
#include "FantasyMelee/FantasyMeleeGameModeBase.h"
#include "GameFramework/Character.h"
#include "GAS/FmAbilitySystemComponent.h"
#include "Interfaces/FmCombatable.h"
#include "Interfaces/FmMotionWarpable.h"
#include "Interfaces/FmTagIdable.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Tags/NpcTags.h"

UFmAbilitySystemComponent* UFmBlueprintFunctionLibrary::GetAbilitySystemComponent(AActor* Actor)
{
	return Cast<UFmAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor));
}

UFmAnimInstance* UFmBlueprintFunctionLibrary::GetAnimInstance(const AActor* Actor)
{
	if (const auto Character = Cast<ACharacter>(Actor))
	{
		if (const auto Mesh = Character->GetMesh())
		{
			return Cast<UFmAnimInstance>(Mesh->GetAnimInstance());
		}
	}

	return nullptr;
}

UCapsuleComponent* UFmBlueprintFunctionLibrary::GetCapsuleComponent(const AActor* Actor)
{
	if (const auto Character = Cast<ACharacter>(Actor))
	{
		return Character->GetCapsuleComponent();
	}

	return nullptr;
}

UFmCombatComponent* UFmBlueprintFunctionLibrary::GetCombatComponent(const AActor* Actor)
{
	if (const auto Combatable = Cast<IFmCombatable>(Actor))
	{
		return Combatable->GetCombatComponent();
	}

	return nullptr;
}

UMotionWarpingComponent* UFmBlueprintFunctionLibrary::GetMotionWarpingComponent(const AActor* Actor)
{
	if (const auto MotionWarpable = Cast<IFmMotionWarpable>(Actor))
	{
		return MotionWarpable->GetMotionWarpingComponent();
	}

	return nullptr;
}

USkeletalMeshComponent* UFmBlueprintFunctionLibrary::GetSkeletalMeshComponent(const AActor* Actor)
{
	if (const auto Character = Cast<ACharacter>(Actor))
	{
		return Character->GetMesh();
	}

	return nullptr;
}

bool UFmBlueprintFunctionLibrary::HasTag(const AActor* Actor, const FGameplayTag& Tag)
{
	if (const auto Taggable = Cast<IGameplayTagAssetInterface>(Actor))
	{
		FGameplayTagContainer TagContainer;
		Taggable->GetOwnedGameplayTags(TagContainer);

		return TagContainer.HasTagExact(Tag);
	}

	return false;
}

bool UFmBlueprintFunctionLibrary::HasTagId(const AActor* Actor, const FGameplayTag& Tag)
{
	if (const auto TagIdable = Cast<IFmTagIdable>(Actor))
	{
		return TagIdable->GetTagId().MatchesTagExact(Tag);
	}

	return false;
}

FGameplayTag UFmBlueprintFunctionLibrary::GetNormalizedTag(const FGameplayTag& Tag)
{
	TArray<FString> TagSegments;
	Tag.ToString().ParseIntoArray(TagSegments, TEXT("_"));

	return FGameplayTag::RequestGameplayTag(FName(TagSegments[0]));
}

bool UFmBlueprintFunctionLibrary::IsEntityTagSpecSatisfied(const UObject* WorldContextObject, const FFmEntityTagSpec& EntityTagSpec, const bool bProhibition)
{
	if (!WorldContextObject) return false;

	const auto World = WorldContextObject->GetWorld();

	if (!World) return false;

	if (EntityTagSpec.PlayerTagSpecs.Num() > 0)
	{
		const auto PlayerController = World->GetFirstPlayerController();

		if (!PlayerController) return false;

		if (const auto GameplayTagAssetInterface = Cast<IGameplayTagAssetInterface>(PlayerController->GetCharacter()))
		{
			FGameplayTagContainer PlayerGameplayTags;
			GameplayTagAssetInterface->GetOwnedGameplayTags(PlayerGameplayTags);
			
			for (const auto PlayerTagSpec : EntityTagSpec.PlayerTagSpecs)
			{
				if (bProhibition && PlayerGameplayTags.HasTagExact(PlayerTagSpec.Tag)) return false;
				if (!bProhibition && !PlayerGameplayTags.HasTagExact(PlayerTagSpec.Tag)) return false;
			}
		} else
		{
			return false;
		}
	}

	// TODO(P0) Implement.

	return true;
}

void UFmBlueprintFunctionLibrary::ProcessEntityTagSpecGrants(const UObject* WorldContextObject, const FFmEntityTagSpec& EntityTagSpec)
{
	if (EntityTagSpec.PlayerTagSpecs.Num() > 0)
	{
		if (const auto Player = Cast<AFmPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(WorldContextObject, 0)))
		{
			for (const auto PlayerTagSpec : EntityTagSpec.PlayerTagSpecs)
			{
				Player->GrantTagSpec(PlayerTagSpec);
			}
		}
	}

	if (const auto GameMode = Cast<AFantasyMeleeGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		for (const auto NpcTagToTagSpecMappings : EntityTagSpec.NpcTagSpecMappings)
		{
			for (const auto& NpcTagSpec : NpcTagToTagSpecMappings.TagSpecs)
			{
				if (NpcTagToTagSpecMappings.Tag.MatchesTag(TAG_Npc))
				{
					AFmMajorNpcCharacter::GrantTagSpec(GameMode, NpcTagToTagSpecMappings.Tag, NpcTagSpec, true);
				}
			}
		}
	}

	// TODO(P0): implement
}
