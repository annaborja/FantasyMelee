// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "Characters/Player/FmPlayerCharacter.h"
#include "GameFramework/Actor.h"
#include "Interfaces/FmTagIdable.h"
#include "FmBoxTrigger.generated.h"

struct FFmTagSpec;
class UBoxComponent;

UENUM()
enum EFmTriggerEffectType : uint8
{
	DestroyActor,
	GrantTag,
	ProcessEntityTagSpec,
	SetLookTarget,
	SetMoveTarget,
};

USTRUCT()
struct FFmTriggerEffectTarget
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes { UEngineTypes::ConvertToObjectType(ECC_Pawn) };
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> TargetClass;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag TagId;
	
	UPROPERTY(EditAnywhere)
	float OverlapSphereRadius = 5000.f;
};

USTRUCT()
struct FFmTriggerEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EFmTriggerEffectType> Type = GrantTag;
	
	UPROPERTY(EditAnywhere)
	bool bExecuteOnceOnly = true;
	
	UPROPERTY(VisibleInstanceOnly)
	bool bExecuted = false;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AActor>> ActorClassFilter { AFmPlayerCharacter::StaticClass() };
	
	UPROPERTY(EditAnywhere, meta=(TitleProperty="Tag"))
	TArray<FFmTagSpec> TagSpecs;
	
	UPROPERTY(EditAnywhere)
	FFmEntityTagSpec EntityTagSpec;
	
	UPROPERTY(EditAnywhere)
	FFmTriggerEffectTarget TargetA;
	UPROPERTY(EditAnywhere)
	FFmTriggerEffectTarget TargetB;
};

UCLASS()
class FANTASYMELEE_API AFmBoxTrigger : public AActor, public IGameplayTagAssetInterface, public IFmTagIdable
{
	GENERATED_BODY()
	
public:	
	AFmBoxTrigger();

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	virtual FGameplayTag GetTagId() const override { return TagId; }

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditAnywhere, Category="FM Params")
	FGameplayTagContainer GameplayTags;
	UPROPERTY(EditAnywhere, Category="FM Params")
	FGameplayTag TagId;
	
	UPROPERTY(EditAnywhere, Category="FM Params", meta=(TitleProperty="Type"))
	TArray<FFmTriggerEffect> TriggerEffects;

	static bool PassesClassFilter(const FFmTriggerEffect& TriggerEffect, const AActor* OtherActor);

	static bool HandleDestroyActor(const FFmTriggerEffect& TriggerEffect, AActor* OtherActor);
	static bool HandleGrantTag(const FFmTriggerEffect& TriggerEffect, AActor* OtherActor);
	
	bool HandleProcessEntityTagSpec(const FFmTriggerEffect& TriggerEffect, const AActor* OtherActor) const;
	bool HandleSetBlackboardKey(const FFmTriggerEffect& TriggerEffect, const AActor* OtherActor) const;
};
