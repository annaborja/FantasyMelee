// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FmBlueprintFunctionLibrary.generated.h"

struct FFmEntityTagSpec;
class UBehaviorTreeComponent;
class UFmPlayerInteractionComponent;
class UFmPlayerMovementComponent;
class UFmAbilitySystemComponent;
class UCapsuleComponent;
struct FGameplayTag;
class UFmAnimInstance;
class UMotionWarpingComponent;
class UFmCombatComponent;

UCLASS()
class FANTASYMELEE_API UFmBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Getters (Actor)
	static UFmAbilitySystemComponent* GetAbilitySystemComponent(AActor* Actor);
	static UFmAnimInstance* GetAnimInstance(const AActor* Actor);
	static UCapsuleComponent* GetCapsuleComponent(const AActor* Actor);
	static UFmCombatComponent* GetCombatComponent(const AActor* Actor);
	static UMotionWarpingComponent* GetMotionWarpingComponent(const AActor* Actor);
	static USkeletalMeshComponent* GetSkeletalMeshComponent(const AActor* Actor);

	// Getters (World)
	template <typename T>
	static T* GetGameMode(const UWorld* World);

	// States
	static bool HasTag(const AActor* Actor, const FGameplayTag& Tag);
	static bool HasTagId(const AActor* Actor, const FGameplayTag& Tag);

	// Tags
	static FGameplayTag GetNormalizedTag(const FGameplayTag& Tag);
	static bool IsEntityTagSpecSatisfied(const UObject* WorldContextObject, const FFmEntityTagSpec& EntityTagSpec, const bool bProhibition = false);
	static void ProcessEntityTagSpecGrants(const UObject* WorldContextObject, const FFmEntityTagSpec& EntityTagSpec);

	UFUNCTION(BlueprintCallable)
	static FText GetInGameNameifiedText(const UObject* WorldContextObject, const FText& InText);
	
	static FVector CalculateVelocity(const FVector& From, const FVector& To, const float Duration, const float GravityScale = 1.f);
};

template <typename T>
T* UFmBlueprintFunctionLibrary::GetGameMode(const UWorld* World)
{
	if (!World) return nullptr;

	return Cast<T>(World->GetAuthGameMode());
}
