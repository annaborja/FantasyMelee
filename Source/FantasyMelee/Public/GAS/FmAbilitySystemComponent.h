// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FmAbilitySystemComponent.generated.h"

UCLASS()
class FANTASYMELEE_API UFmAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void Init(AActor* InOwnerActor, AActor* InAvatarActor);

	void GrantAbility(const TSubclassOf<UGameplayAbility> AbilityClass);
	bool TryActivateAbilityByTag(const FGameplayTag& Tag);
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	UPROPERTY(EditDefaultsOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> InitPrimaryAttributesEffect;
};
