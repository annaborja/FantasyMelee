// Copyright AB. All Rights Reserved.

#include "Components/FmCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/CapsuleComponent.h"
#include "FantasyMelee/FantasyMeleeGameModeBase.h"
#include "GAS/FmAbilitySystemComponent.h"
#include "GAS/Abilities/FmAttackAbility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Tags/InGameTags.h"
#include "Tags/TempTags.h"
#include "Utils/Constants.h"
#include "Utils/FmBlueprintFunctionLibrary.h"
#include "Utils/Macros.h"

bool UFmCombatComponent::IsCombatAbility(const UGameplayAbility* Ability)
{
	return Ability->AbilityTags.HasTag(TAG_Action_Combat);
}

bool UFmCombatComponent::IsQueueableAbility(const UGameplayAbility* Ability)
{
	return IsCombatAbility(Ability);
}

void UFmCombatComponent::TriggerHitReactionAbility(const FHitResult& HitResult)
{
	if (const auto HitActor = HitResult.GetActor())
	{
		if (const auto TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
		{
			auto HitReactionEffectContextHandle = TargetAbilitySystemComponent->MakeEffectContext();
			HitReactionEffectContextHandle.AddHitResult(HitResult);
			
			FGameplayEventData HitReactionPayload;
			HitReactionPayload.ContextHandle = HitReactionEffectContextHandle;
			
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, TAG_Effect_HitReaction, HitReactionPayload);
		}
	}
}

UFmCombatComponent::UFmCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFmCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (const auto AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		AbilitySystemComponent->AbilityActivatedCallbacks.AddUObject(this, &ThisClass::OnAbilityStarted);
		AbilitySystemComponent->AbilityEndedCallbacks.AddUObject(this, &ThisClass::OnAbilityEnded);
		AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this, &ThisClass::OnAbilityFailed);
	}
}

void UFmCombatComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HandleActiveHitBoxes();
	HandleAbilityCancellation();
}

void UFmCombatComponent::ExecuteHeavyAttack() const
{
	if (const auto AbilitySystemComponent = UFmBlueprintFunctionLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		AbilitySystemComponent->TryActivateAbilityByTag(TAG_Action_Combat_Attack_Unarmed_RearUppercutPower);
	}
}

void UFmCombatComponent::ExecuteLightAttack() const
{
	if (const auto AbilitySystemComponent = UFmBlueprintFunctionLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		FGameplayTag Tag = TAG_Action_Combat_Attack_Unarmed_Jab;

		if (AbilityExecutionRecords.Num() > 0)
		{
			if (const auto Record = AbilityExecutionRecords.Last(); FPlatformTime::Seconds() - Record.Timestamp < 1.f)
			{
				if (Record.TagId == TAG_Action_Combat_Attack_Unarmed_Jab)
				{
					Tag = TAG_Action_Combat_Attack_Unarmed_Cross;
				} else if (Record.TagId == TAG_Action_Combat_Attack_Unarmed_Cross)
				{
					Tag = TAG_Action_Combat_Attack_Unarmed_LeadHook;
				} else if (Record.TagId == TAG_Action_Combat_Attack_Unarmed_LeadHook)
				{
					Tag = TAG_Action_Combat_Attack_Unarmed_RearUppercut;
				}
			}
		}
		
		AbilitySystemComponent->TryActivateAbilityByTag(Tag);
	}
}

void UFmCombatComponent::ExecuteBlock() const
{
	if (const auto AbilitySystemComponent = UFmBlueprintFunctionLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		AbilitySystemComponent->TryActivateAbilityByTag(TAG_Action_Combat_Defense_Unarmed_Block);
	}
}

AActor* UFmCombatComponent::FindSoftTarget(const float TraceLength) const
{
	AActor* SoftTarget = nullptr;

	if (const auto Owner = GetOwner())
	{
		auto BoxTraceHalfSize = FVector(5.f, 34.f, 88.f);

		if (const auto CapsuleComponent = UFmBlueprintFunctionLibrary::GetCapsuleComponent(Owner))
		{
			BoxTraceHalfSize.Y = CapsuleComponent->GetScaledCapsuleRadius();
			BoxTraceHalfSize.Z = CapsuleComponent->GetScaledCapsuleHalfHeight();
		}
		
		const TArray ActorsToIgnore { Owner };
		const auto OwnerForwardVector = Owner->GetActorForwardVector();
		const auto OwnerLocation = Owner->GetActorLocation();
		const auto OwnerRotation = Owner->GetActorRotation();
		TArray<FHitResult> HitResults;
		
		// Prioritize targets in front of the player.
		// If there are none, then sweep over a wider area.
		UKismetSystemLibrary::BoxTraceMulti(this, OwnerLocation,
			OwnerLocation + OwnerForwardVector * TraceLength, BoxTraceHalfSize, OwnerRotation,
			UEngineTypes::ConvertToTraceType(TRACE_CHANNEL_WEAPON), false,
			ActorsToIgnore, SoftTargetDrawDebugTrace, HitResults, true);

		if (HitResults.Num() < 1)
		{
			TArray<float> YawRotationOffsets;
		
			for (const auto Offset : TArray { 15.f, 30.f, 45.f, 60.f, 75.f, 90.f })
			{
				YawRotationOffsets.Add(Offset);
				YawRotationOffsets.Add(Offset * -1);
			}
		
			for (const auto YawRotationOffset : YawRotationOffsets)
			{
				const auto RotationOffset = FRotator(0.f, YawRotationOffset, 0.f);
				TArray<FHitResult> OutHits;
				
				UKismetSystemLibrary::BoxTraceMulti(this, OwnerLocation,
					OwnerLocation + RotationOffset.RotateVector(OwnerForwardVector) * TraceLength,
					BoxTraceHalfSize, OwnerRotation + RotationOffset,
					UEngineTypes::ConvertToTraceType(TRACE_CHANNEL_WEAPON), false,
					ActorsToIgnore, SoftTargetDrawDebugTrace, OutHits, true);

				HitResults.Append(OutHits);
			}
		}
		
		if (HitResults.Num() > 0)
		{
			HitResults.Sort([](const FHitResult& HitResultA, const FHitResult& HitResultB)
			{
				return HitResultA.Distance < HitResultB.Distance;
			});

			SoftTarget = HitResults[0].GetActor();
		}
	}

	if (bDebugSoftTarget)
	{
		SCREEN_LOG(SoftTarget ? SoftTarget->GetName() : TEXT("No soft target"), 2.f)
	}

	return SoftTarget;
}

FName UFmCombatComponent::GetHitReactionMontageSectionName(const FHitResult& HitResult) const
{
	if (const auto Owner = GetOwner())
	{
		const auto ForwardVector = Owner->GetActorForwardVector();
		const auto ImpactNormal = HitResult.ImpactNormal;
		auto Theta = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardVector, ImpactNormal)));

		if (FVector::CrossProduct(ForwardVector, ImpactNormal).Z < 0)
		{
			Theta *= -1.f;
		}

		if (Theta >= -45.f && Theta <= 45.f) return FName(MONTAGE_SECTION_HIT_REACTION_FRONT_LARGE);
		if (Theta < -45.f && Theta > -135.f) return FName(MONTAGE_SECTION_HIT_REACTION_LEFT_LARGE);
		if (Theta > 45.f && Theta < 135.f) return FName(MONTAGE_SECTION_HIT_REACTION_RIGHT_LARGE);
		
		return FName(MONTAGE_SECTION_HIT_REACTION_BACK_LARGE);
	}

	return FName();
}

void UFmCombatComponent::AddActiveHitBox(const FDataTableRowHandle& HitBoxDataRowHandle)
{
	const auto HitBoxName = HitBoxDataRowHandle.RowName;
	
	if (const auto HitBoxCount = ActiveHitBoxCountMap.Find(HitBoxName))
	{
		ActiveHitBoxCountMap.Add(HitBoxName, *HitBoxCount + 1);
	} else
	{
		ActiveHitBoxCountMap.Add(HitBoxName, 1);
	}
}

void UFmCombatComponent::RemoveActiveHitBox(const FDataTableRowHandle& HitBoxDataRowHandle)
{
	const auto HitBoxName = HitBoxDataRowHandle.RowName;
	
	if (const auto HitBoxCount = ActiveHitBoxCountMap.Find(HitBoxName))
	{
		if (const auto NewHitBoxCount = *HitBoxCount - 1; NewHitBoxCount > 0)
		{
			ActiveHitBoxCountMap.Add(HitBoxName, NewHitBoxCount);
		} else
		{
			ActiveHitBoxCountMap.Remove(HitBoxName);
		}
	}
}

void UFmCombatComponent::SetCancellableAbility(const FGameplayAbilitySpecHandle& AbilitySpecHandle)
{
	CancellableAbilitySpecHandle = AbilitySpecHandle;

	HandleAbilityCancellation();
}

void UFmCombatComponent::UnsetCancellableAbility(const FGameplayAbilitySpecHandle& AbilitySpecHandle)
{
	if (CancellableAbilitySpecHandle == AbilitySpecHandle)
	{
		CancellableAbilitySpecHandle = FGameplayAbilitySpecHandle();
	}
}

void UFmCombatComponent::OnAbilityEnded(UGameplayAbility* Ability)
{
	CleanUpAbilitySpecificData(Ability);
	HandleAbilityQueue(Ability);
}

void UFmCombatComponent::OnAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& TagContainer)
{
	if (!IsQueueableAbility(Ability)) return;
	
	if (TagContainer.HasTagExact(TAG_Gas_AbilityFailure_TagsBlocked))
	{
		QueuedAbility = Ability;
	}
}

void UFmCombatComponent::OnAbilityStarted(UGameplayAbility* Ability)
{
	if (IsCombatAbility(Ability))
	{
		PrimaryActiveAbilitySpecHandle = Ability->GetCurrentAbilitySpecHandle();

		if (const auto CustomAbility = Cast<UFmGameplayAbility>(Ability))
		{
			CleanUpAbilityExecutionRecords(1);
			AbilityExecutionRecords.Add(FFmAbilityExecutionRecord(CustomAbility->GetTagId()));
		}
	}
}

void UFmCombatComponent::CleanUpAbilityExecutionRecords(const uint8 BufferSize)
{
	const auto NumRecordsToRemove = AbilityExecutionRecords.Num() + BufferSize - MaxNumAbilityExecutionRecords;

	for (uint8 i = 1; i <= NumRecordsToRemove; i++)
	{
		if (AbilityExecutionRecords.IsValidIndex(0))
		{
			AbilityExecutionRecords.RemoveAt(0);
		}
	}
}

void UFmCombatComponent::CleanUpAbilitySpecificData(const UGameplayAbility* Ability)
{
	if (PrimaryActiveAbilitySpecHandle == Ability->GetCurrentAbilitySpecHandle())
	{
		CancellableAbilitySpecHandle = FGameplayAbilitySpecHandle();
		PrimaryActiveAbilitySpecHandle = FGameplayAbilitySpecHandle();
		
		ActiveHitBoxCountMap.Reset();
		HitActors.Reset();
	}
}

void UFmCombatComponent::HandleAbilityCancellation()
{
	if (!QueuedAbility) return;
	
	if (CancellableAbilitySpecHandle.IsValid() && CancellableAbilitySpecHandle == PrimaryActiveAbilitySpecHandle)
	{
		if (const auto AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
		{
			AbilitySystemComponent->CancelAbilityHandle(CancellableAbilitySpecHandle);
			CancellableAbilitySpecHandle = FGameplayAbilitySpecHandle();
		}
	}
}

void UFmCombatComponent::HandleAbilityQueue(const UGameplayAbility* FinishedAbility)
{
	if (!QueuedAbility || !IsQueueableAbility(FinishedAbility)) return;

	if (const auto AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		for (const auto& AbilitySpec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if (AbilitySpec.Ability == QueuedAbility)
			{
				AbilitySystemComponent->TryActivateAbility(AbilitySpec.Handle);
				break;
			}
		}
	}
	
	QueuedAbility = nullptr;
}

void UFmCombatComponent::ApplyDamageEffect(const FHitResult& HitResult) const
{
	if (const auto AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		bool bIsInstance;
						
		if (const auto AttackAbility = Cast<UFmAttackAbility>(
			UAbilitySystemBlueprintLibrary::GetGameplayAbilityFromSpecHandle(AbilitySystemComponent, PrimaryActiveAbilitySpecHandle, bIsInstance)))
		{
			if (const auto DamageEffectClass = AttackAbility->GetDamageEffectClass())
			{
				if (const auto TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor()))
				{
					const auto DamageEffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
						DamageEffectClass, AttackAbility->GetAbilityLevel(), AbilitySystemComponent->MakeEffectContext());
								
					AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*DamageEffectSpecHandle.Data.Get(), TargetAbilitySystemComponent);
				}
			}
		}
	}
}

void UFmCombatComponent::HandleActiveHitBoxes()
{
	const UDataTable* HitBoxDataTable = nullptr;
			
	for (const auto& ActiveHitBoxMapping : ActiveHitBoxCountMap)
	{
		if (ActiveHitBoxMapping.Value > 0)
		{
			if (!HitBoxDataTable)
			{
				if (const auto GameMode = UFmBlueprintFunctionLibrary::GetGameMode<AFantasyMeleeGameModeBase>(GetWorld()))
				{
					HitBoxDataTable = GameMode->GetHitBoxDataTable();
				}
			}

			if (HitBoxDataTable)
			{
				HandleActiveHitBox(ActiveHitBoxMapping.Key, HitBoxDataTable);
			}
		}
	}
}

void UFmCombatComponent::HandleActiveHitBox(const FName& HitBoxName, const UDataTable* HitBoxDataTable)
{
	if (const auto HitBoxData = HitBoxDataTable->FindRow<FFmHitBoxData>(HitBoxName, TEXT("HitBoxData")))
	{
		if (const auto Owner = GetOwner(); Owner && Owner->HasAuthority())
		{
			const auto Origin = Owner->GetActorLocation();
			const auto Rotation = Owner->GetActorRotation();

			TArray<FHitResult> OutHits;
			UKismetSystemLibrary::SphereTraceMulti(this,
				Origin + Rotation.RotateVector(HitBoxData->StartOffsetLocation),
				Origin + Rotation.RotateVector(HitBoxData->EndOffsetLocation),
				HitBoxData->HalfSize, UEngineTypes::ConvertToTraceType(TRACE_CHANNEL_WEAPON), false,
				TArray { Owner }, HitBoxDrawDebugTrace, OutHits, true);

			for (const auto& OutHit : OutHits)
			{
				HandleHitResult(OutHit);
			}
		}
	}
}

void UFmCombatComponent::HandleHitResult(const FHitResult& HitResult)
{
	if (const auto HitActor = HitResult.GetActor(); HitActor && !HitActors.Contains(HitActor))
	{
		HitActors.Add(HitActor);
		
		TriggerHitReactionAbility(HitResult);
		ApplyDamageEffect(HitResult);
	}
}
