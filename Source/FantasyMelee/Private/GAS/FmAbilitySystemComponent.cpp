// Copyright AB. All Rights Reserved.

#include "GAS/FmAbilitySystemComponent.h"

#include "FantasyMelee/FantasyMeleeGameModeBase.h"
#include "GAS/FmGasDataAsset.h"
#include "GAS/Abilities/FmGameplayAbility.h"
#include "Interfaces/FmCombatable.h"
#include "Utils/FmBlueprintFunctionLibrary.h"

void UFmAbilitySystemComponent::Init(AActor* InOwnerActor, AActor* InAvatarActor)
{
	InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	
	const auto Combatable = Cast<IFmCombatable>(InAvatarActor);

	if (InitPrimaryAttributesEffect)
	{
		ApplyGameplayEffectSpecToTarget(
			*MakeOutgoingSpec(InitPrimaryAttributesEffect, Combatable ? Combatable->GetCombatLevel() : 1.f, MakeEffectContext()).Data.Get(), this);
	}

	if (InAvatarActor->HasAuthority())
	{
		for (const auto AbilityClass : StartupAbilities)
		{
			GrantAbility(AbilityClass);
		}
	}

	if (const auto GameMode = UFmBlueprintFunctionLibrary::GetGameMode<AFantasyMeleeGameModeBase>(GetWorld()))
	{
		if (const auto CharacterClassInfo = GameMode->GetGasDataAsset())
		{
			for (const auto AbilityClass : CharacterClassInfo->GetCommonStartupAbilities())
			{
				GrantAbility(AbilityClass);
			}
		}
	}
}

void UFmAbilitySystemComponent::GrantAbility(const TSubclassOf<UGameplayAbility> AbilityClass)
{
	const auto Combatable = Cast<IFmCombatable>(GetAvatarActor());
	auto AbilitySpec = FGameplayAbilitySpec(AbilityClass, Combatable ? Combatable->GetCombatLevel() : 1);

	if (const auto Ability = Cast<UFmGameplayAbility>(AbilitySpec.Ability))
	{
		if (const auto TagId = Ability->GetTagId(); TagId.IsValid())
		{
			AbilitySpec.DynamicAbilityTags.AddTag(TagId);
		}
	}
	
	GiveAbility(AbilitySpec);
}

bool UFmAbilitySystemComponent::TryActivateAbilityByTag(const FGameplayTag& Tag)
{
	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(Tag))
		{
			return TryActivateAbility(AbilitySpec.Handle);
		}
	}

	return false;
}
