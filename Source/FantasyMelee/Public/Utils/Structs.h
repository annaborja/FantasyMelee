// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Structs.generated.h"

UENUM(BlueprintType)
namespace EFmSpeedCategory
{
	enum Type : uint8
	{
		Normal,
		Fast,
		Slow
	};
}

USTRUCT(BlueprintType)
struct FFmTagSpec
{
	GENERATED_BODY()

	FFmTagSpec() {}
	
	explicit FFmTagSpec(const FGameplayTag& InTag, const int32& InCount = 1) :
		Tag(InTag), Count(InCount) {}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Tag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Count = 1;
};

USTRUCT(BlueprintType)
struct FFmTagToTagSpecsMapping
{
	GENERATED_BODY();
	
	UPROPERTY(EditAnywhere)
	FGameplayTag Tag;
	
	UPROPERTY(EditAnywhere)
	TArray<FFmTagSpec> TagSpecs;
};

USTRUCT()
struct FFmEntityTagSpec
{
	GENERATED_BODY();

	FFmEntityTagSpec() {}
	explicit FFmEntityTagSpec(const TArray<FFmTagSpec>& InPlayerTagSpecs) : PlayerTagSpecs(InPlayerTagSpecs) {}
	
	UPROPERTY(EditAnywhere)
	TArray<FFmTagSpec> PlayerTagSpecs;
	
	UPROPERTY(EditAnywhere)
	TArray<FFmTagToTagSpecsMapping> NpcTagSpecMappings;
	UPROPERTY(EditAnywhere)
	TArray<FFmTagToTagSpecsMapping> WorldTagSpecMappings;
};

USTRUCT(BlueprintType)
struct FFmMontageData
{
	GENERATED_BODY()

	bool IsValid() const { return Montage != nullptr; }
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Rate = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName StartSection;
};
