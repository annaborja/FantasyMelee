// Copyright AB. All Rights Reserved.

#include "Characters/Player/FmPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Characters/Player/FmPlayerController.h"
#include "Characters/Player/FmPlayerState.h"
#include "Characters/Player/Components/FmPlayerCombatComponent.h"
#include "Characters/Player/Components/FmPlayerInteractionComponent.h"
#include "Characters/Player/Components/FmPlayerMovementComponent.h"
#include "Characters/Player/Components/FmPlayerQuestComponent.h"
#include "Characters/Player/Components/FmPlayerTutorialComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/FmInventoryComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Utils/Constants.h"

AFmPlayerCharacter::AFmPlayerCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFmPlayerMovementComponent>(CharacterMovementComponentName))
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	if (const auto CapsuleComponent = GetCapsuleComponent())
	{
		CapsuleComponent->SetCollisionObjectType(OBJECT_CHANNEL_PLAYER);
		CapsuleComponent->SetCollisionResponseToChannel(TRACE_CHANNEL_WEAPON, ECR_Block);
	}

	if (const auto Mesh = GetMesh())
	{
		Mesh->SetRelativeLocation(FVector(0.f, 0.f, -91.f));
		Mesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}
	
	FollowCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FollowCameraSpringArm"));
	FollowCameraSpringArm->SetupAttachment(GetRootComponent());
	FollowCameraSpringArm->TargetArmLength = 300.f;
	FollowCameraSpringArm->SocketOffset = FVector(0.f, 0.f, 108.f);
	FollowCameraSpringArm->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(FollowCameraSpringArm);
	FollowCamera->SetRelativeRotation(FRotator(-8.f, 0.f, 0.f));

	PlayerCombatComponent = CreateDefaultSubobject<UFmPlayerCombatComponent>(TEXT("PlayerCombatComponent"));
	CombatComponent = PlayerCombatComponent;
	
	PlayerMovementComponent = CastChecked<UFmPlayerMovementComponent>(GetCharacterMovement());
	
	InteractionComponent = CreateDefaultSubobject<UFmPlayerInteractionComponent>(TEXT("InteractionComponent"));
	QuestComponent = CreateDefaultSubobject<UFmPlayerQuestComponent>(TEXT("QuestComponent"));
	TutorialComponent = CreateDefaultSubobject<UFmPlayerTutorialComponent>(TEXT("TutorialComponent"));
}

void AFmPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (const auto CustomPlayerState = GetPlayerState<AFmPlayerState>())
	{
		CastChecked<UFmAbilitySystemComponent>(CustomPlayerState->GetAbilitySystemComponent())->
			Init(CustomPlayerState, this);
	}

	CustomPlayerController = CastChecked<AFmPlayerController>(NewController);
	
	FGameplayTagContainer PlayerTags;
	GetOwnedGameplayTags(PlayerTags);

	// If we haven't yet started the game for real (in this prototype,
	// that means completing the intro NPC dialogue), block player input.
	if (!PlayerTags.HasTagExact(TAG_InGame_StartedGameProper))
	{
		GrantTagSpec(FFmTagSpec(TAG_InGame_BlockInput));
	}
}

void AFmPlayerCharacter::Jump()
{
	if (!bCanJump) return;

	if (PlayerMovementComponent)
	{
		PlayerMovementComponent->bNotifyApex = true;
	}
	
	bCanJump = false;
	
	Super::Jump();
}

UAbilitySystemComponent* AFmPlayerCharacter::GetAbilitySystemComponent() const
{
	if (const AFmPlayerState* CustomPlayerState = GetPlayerState<AFmPlayerState>())
	{
		return CustomPlayerState->GetAbilitySystemComponent();
	}

	return nullptr;
}

void AFmPlayerCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	FGameplayTagContainer AbilitySystemTags;
	
	if (const auto AbilitySystemComponent = GetAbilitySystemComponent())
	{
		AbilitySystemComponent->GetOwnedGameplayTags(AbilitySystemTags);
	}
	
	TagContainer.AppendTags(CharacterTags);
	TagContainer.AppendTags(AbilitySystemTags);
}

void AFmPlayerCharacter::GrantTagSpec(const FFmTagSpec& TagSpec)
{
	if (bDebugGrantTagSpec)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player GrantTagSpec: %s (%d)"), *TagSpec.Tag.ToString(), TagSpec.Count)
	}
	
	if (TagSpec.Tag.IsValid())
	{
		bool bSuccess = false;

		if (TagSpec.Count > 0)
		{
			if (UFmInventoryComponent::IsItemTag(TagSpec.Tag))
			{
				bSuccess = InventoryComponent->AddItem(TagSpec.Tag, TagSpec.Count);
			} else if (UFmPlayerQuestComponent::IsQuestTag(TagSpec.Tag))
			{
				bSuccess = QuestComponent->GrantQuest(TagSpec.Tag, this);
			} else if (UFmPlayerTutorialComponent::IsTutorialTag(TagSpec.Tag)) {
				bSuccess = TutorialComponent->GrantTutorial(TagSpec.Tag, this);
			} else if (!CharacterTags.HasTagExact(TagSpec.Tag))
			{
				CharacterTags.AddTag(TagSpec.Tag);
				bSuccess = true;
			}
		} else if (TagSpec.Count < 0)
		{
			if (UFmInventoryComponent::IsItemTag(TagSpec.Tag)) {
				bSuccess = InventoryComponent->RemoveItem(TagSpec.Tag, TagSpec.Count);
			} else if (CharacterTags.HasTagExact(TagSpec.Tag))
			{
				CharacterTags.RemoveTag(TagSpec.Tag);
				bSuccess = true;
			}
		}
		
		if (bSuccess)
		{
			OnTagSpecGrant(TagSpec);
			
			if (CustomPlayerController) CustomPlayerController->OnTagSpecGrant(TagSpec);
			
			if (QuestComponent) QuestComponent->OnTagSpecGrant(this);
		}
	}
}

void AFmPlayerCharacter::EnableJump()
{
	bCanJump = true;
}

AFmHud* AFmPlayerCharacter::GetCustomHud() const
{
	if (CustomPlayerController) return CustomPlayerController->GetCustomHud();

	return nullptr;
}

void AFmPlayerCharacter::OnTagSpecGrant(const FFmTagSpec& TagSpec)
{
	// Once we've started the game for real (in this prototype,
	// that means completing the intro NPC dialogue), unblock player input.
	if (TagSpec.Tag.MatchesTagExact(TAG_InGame_StartedGameProper) && TagSpec.Count > 0)
	{
		GrantTagSpec(FFmTagSpec(TAG_InGame_BlockInput, -1));
	}
}
