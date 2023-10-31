// Copyright AB. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "FmPlayerCameraManager.generated.h"

class AFmPlayerCharacter;
class UCharacterMovementComponent;

UCLASS()
class FANTASYMELEE_API AFmPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	AFmPlayerCameraManager();

protected:
	virtual void BeginPlay() override;
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly)
	float CrouchBlendDuration = 0.5f;

	float CrouchBlendTime;
	
	UPROPERTY(Transient)
	TObjectPtr<AFmPlayerCharacter> Player;

	void AdjustCameraForCrouch(FTViewTarget& OutVT, const float DeltaTime, const UCharacterMovementComponent* MovementComponent);
};
