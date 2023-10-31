// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FmGasDataAsset.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM()
enum class EFmCharacterClass : uint8
{
	BareHandedFighter,
	AxeWielder,
	BowWielder,
	KnifeWielder,
	SwordWielder
};

USTRUCT()
struct FFmCharacterClassData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> InitPrimaryAttributesEffect;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};

UCLASS()
class FANTASYMELEE_API UFmGasDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	FORCEINLINE TMap<EFmCharacterClass, FFmCharacterClassData> GetCharacterClassDataMap() const { return CharacterClassDataMap; }
	FORCEINLINE TArray<TSubclassOf<UGameplayAbility>> GetCommonStartupAbilities() const { return CommonStartupAbilities; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetInitVitalAttributesEffectClass() const { return InitVitalAttributesEffectClass; }

private:
	UPROPERTY(EditDefaultsOnly, Category="Class Data")
	TMap<EFmCharacterClass, FFmCharacterClassData> CharacterClassDataMap;
	
	UPROPERTY(EditDefaultsOnly, Category="Class Data|Common")
	TArray<TSubclassOf<UGameplayAbility>> CommonStartupAbilities;
	UPROPERTY(EditDefaultsOnly, Category="Class Data|Common")
	TSubclassOf<UGameplayEffect> InitVitalAttributesEffectClass;
};
