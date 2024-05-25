// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "../../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "DreamBotsGameInstance.h"
#include "MazeConsole.h"
#include "MovieTape.h"
#include "MountableActor.h"
#include "DreamBotsCharacter.generated.h"

UENUM()
enum CharacterType
{
	Basic,
	Cleaning,
	Cooking,
	Arms,
	Typing
};

UENUM()
enum CharacterSkinColor
{
	Yellow,
	Red,
	Blue,
	Purple,
	Black,
	Orange,
	Cyan,
	White
};

class ATrampolin;
UCLASS(config = Game)
class ADreamBotsCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADreamBotsCharacter();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void SetZThreshold(double value) { ZvalueThreshold = value; };

	void JumpedOnTrampoline(bool value) { bTouchedTrampoline = value; };

	void SetTrampolinActor(ATrampolin* trampoline) { TrampolineActor = trampoline; };

	void SetCameraLookAtPoint(USceneComponent* lookAtPoint, bool inFront, float boomArmPitch, float cameraPitch, float boomArmLength, float transitionSpeed);

	UFUNCTION(BlueprintCallable, Category = MovieTape)
		int GetMovieTapes() const { return CurrentAmountOfMovieTapes; }
	void AddMovieTape(DrawingType idx);

	UFUNCTION(BlueprintCallable)
		void SetCanPlayerMove(bool flag) { AllowPlayerToMove = flag; }

	UFUNCTION(BlueprintCallable)
		void SetCanPlayerJump(bool flag) { bIsAllowedToJump = flag; }

	UFUNCTION()
		TArray<bool> GetMovieTapesArray() const { return MovieTapesFound; }
	UFUNCTION(BlueprintCallable)
		bool GetIsTapePickedUp() const { return HasAnimationOfPickUpEnded; }
	UFUNCTION(BlueprintCallable)
		void HasTapePickupAnimationEnded(bool flag)  { HasAnimationOfPickUpEnded = flag; }
	UFUNCTION(BlueprintCallable)
		bool GetIsEnteringDrone() const { return IsEnteringTheDrone; }

	UFUNCTION(BlueprintCallable)
		bool GetIsRiding() const { return bIsRiding; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Drone)
		bool PlayerInRangeOfDroneMachine = false;

	void Interact(const FInputActionValue& Value);
	void ReturnPlayer() { IsEnteringTheDrone = false; };
	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
		void ChangeCharacterType(CharacterType type, bool activateEffect, bool rememberType);
	UFUNCTION()
		void GoBackToOldType();



	UFUNCTION(BlueprintCallable)
		void ChangeCharacterColor(CharacterSkinColor color, bool activateEffect);

	UFUNCTION()
		void StartCleaning();
	UFUNCTION()
		void RotateCleaningBroom(float elapsed);
	UFUNCTION()
		void StopCleaning();

	UFUNCTION()
		void FallingAudio();

	CharacterType GetCharType() { return CharType; }

	void CheckForCameraZone();
	UFUNCTION(BlueprintCallable)
		void Dismounting();
	UFUNCTION(BlueprintCallable)
		void FootStepEvent(bool leftFoot);

	bool IsBeingPickedUp() { return bIsPickedUp; };

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHasOverlappedWithTape, bool, HasOverlapped);
	UPROPERTY(BlueprintAssignable);
	FHasOverlappedWithTape OnOverlappedWithTape;

	virtual void Jump() override;

	bool GetDisableTick() const { return bDisableTick; }

	UFUNCTION()
	void SetCameraCollision(bool value);

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called once when Game starts
	virtual void BeginPlay();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called when Game ended
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	// Override the Landed function
	virtual void Landed(const FHitResult& Hit) override;

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		float RotationCleaningSpeed = 2.f;

	/** Static Mesh which is editable -> Should contain a Drone Mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* DroneMeshComponent;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Trampolin, meta = (AllowPrivateAccess = "true"))
	class ATrampolin* TrampolineActor;

	/** Edit the value of the Z Threshold for the Player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		double ZvalueThreshold = -500.0;

	/** Edit the Z offset of the Player moving upwards -> This value decides how far the Player moves upwards while getting reset */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		double ZvalueOffset = 250.0;

	/** Edit the Interval in what the position of the Player is getting saved */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Values, meta = (AllowPrivateAccess = "true"))
		float SaveInterval = 1.0f;

	/** Path to the Drone Mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Drone, meta = (AllowPrivateAccess = "true"))
		FString DroneMeshPath = TEXT("StaticMesh'/Engine/EditorMeshes/Axis_Guide.Axis_Guide'");

	// Component for all pickup audio
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio Grab", meta = (AllowPrivateAccess = "true"))
		UAkComponent* PickupAudio;

	// Pickup events
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Grab", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEPickup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Grab", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WETopReached;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Grab", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WERelease;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Grab", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEGrounded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Grab", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEDroneUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Grab", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEDroneDown;

	// Pickup speed RTPC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Grab", meta = (AllowPrivateAccess = "true"))
		UAkRtpc* RtpcPickupSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Grab", meta = (AllowPrivateAccess = "true"))
		float RtpcMaxVelocity;

	// Movement audio
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio Footsteps", meta = (AllowPrivateAccess = "true"))
		UAkComponent* MovementAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Footsteps", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEFootstep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Jump", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEJump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Jump", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WELand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Footsteps", meta = (AllowPrivateAccess = "true"))
		UAkSwitchValue* SwitchGrass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Footsteps", meta = (AllowPrivateAccess = "true"))
		UAkSwitchValue* SwitchMetal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Footsteps", meta = (AllowPrivateAccess = "true"))
		UAkSwitchValue* SwitchPlastic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Footsteps", meta = (AllowPrivateAccess = "true"))
		UAkSwitchValue* SwitchRock;

	// Other audio
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WETapePickup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEStartCleaning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEStopCleaning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEStartBroom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEStopBroom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEColorSwap;

	// Time it takes to go back up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Drone", meta = (AllowPrivateAccess = "true"))
		float MoveUpTime = 2.f;

	// Time it takes to move to the platform
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Drone", meta = (AllowPrivateAccess = "true"))
		float MoveToPlatformTime = 2.f;

	// Time it takes to move to the platform
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Drone", meta = (AllowPrivateAccess = "true"))
		float DisableMovementTime = 1.f;

	FVector DistanceLocation{};
	double ZvaluePlatform{}, YvaluePlatform{}, XvaluePlatform{}, Distance{};
	bool bThresholdReached{}, bOnGround{}, bOnGroundSound{}, bShouldSave = true;
	bool AllowPlayerToMove = true;
	bool HasInteracted = false;
	bool bReset = false;
	bool bResetStepOne = false;
	bool bIsAllowedToJump = true;
	bool bTouchedTrampoline = false;


	//-------------------------------------------
	void MoveUpwards(double DistanceToMoveUp);
	void MoveToPlatform();							//These 3 functions are responsible for a smooth reset of the Player Character
	void DoPlayerFalloffLogic();
	void UpdatePickupSpeedRPTC();
	void CheckIfLandedAfterPickup();
	bool CanJump();
	//-------------------------------------------

	void SaveLocation();

	//Helper functions so they can get called delayed
	void HideDrone();
	void EnableMovementInput();

	void DoCameraLookAtLogic();
	void BroomTouchingGround();


	TArray<TSubclassOf<AActor>> ActorsToCheck{};

	FTimerHandle MovementUpTimer{}, MovementDownTimer{}, SaveLocationTimer{}, HideDroneTimer{}, EnableMovementInputTimer{};

	FVector LastSavedLocation{};

	UStaticMesh* DroneMesh{};

	AActor* PlayerActor{};

	APlayerController* DreamBotsController{};
	bool IsEnteringTheDrone = false;
	class ADroneMachine* DroneMachine{};
	class AMazeConsole* MazeConsole{};
	class AMountableActor* MountableActor{};
	FRotator RotationDirection = { 0.f,180.f,0.f };

	//Riding 
	bool bIsRiding = false;
	bool bIsCloseToActor = false;
	float RidingCooldown = 0.f;
	bool HasRidingCooldownStarted = false;
	void StartRidingCooldown(float DeltaSeconds);

	//Camera
	USceneComponent* CameraLookAtPoint{};
	bool bCameraLookAtInFront{};
	float BoomArmPitchOffset{}, CameraPitchOffset{}, BoomArmLength{};
	FRotator CurrBoomArmRotation{};
	UPROPERTY(EditAnywhere, Category = "Camera")
		float CameraTransitionSpeed{ 1.5f };
	void InstantSnapCamera();

	//MovieTape
	int CurrentAmountOfMovieTapes = 0;
	UPROPERTY(EditAnywhere, Category = "MovieTape", meta = (AllowPrivateAccess = "true"))
		TArray<bool> MovieTapesFound;
	bool HasPickuedUpATape = false;
	bool HasAnimationOfPickUpEnded = false;
	//Pickup
	bool bIsPickedUp = false;
	bool bIsFallingAfterPickup = false;

	//Cleaning
	bool EnableCleaning = false;

	//CharacterType
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<CharacterType> CharType{ Basic };
	CharacterType OldCharType{ Basic };

	//SkinColor
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<CharacterSkinColor> CharSkinColor{};

	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true"))
		class UNiagaraComponent* CharacterSwapSmokeComponent{};

	UPROPERTY(EditAnywhere, Category = "Effects", meta = (AllowPrivateAccess = "true"))
		class UNiagaraComponent* WalkTrailComponent{};

	UPROPERTY(EditAnywhere, Category = "Addons", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* AddonMeshComp {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Addons", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* CleaningBroomMesh {};

	UPROPERTY(EditAnywhere, Category = "Addons", meta = (AllowPrivateAccess = "true"))
		class UStaticMesh* TypingAddon{};

	UPROPERTY(EditAnywhere, Category = "Addons", meta = (AllowPrivateAccess = "true"))
		class UStaticMesh* CleaningAddon{};

	UPROPERTY(EditAnywhere, Category = "Addons", meta = (AllowPrivateAccess = "true"))
		class UStaticMesh* CookingAddon{};

	UPROPERTY(EditAnywhere, Category = "Addons", meta = (AllowPrivateAccess = "true"))
		class UStaticMesh* ArmsAddon{};


	UPROPERTY(EditAnywhere, Category = "Addons", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEType;

	UPROPERTY(EditAnywhere, Category = "Addons", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WECleaning;

	UPROPERTY(EditAnywhere, Category = "Addons", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WECooking;

	UPROPERTY(EditAnywhere, Category = "Addons", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WEArms;

	UPROPERTY(EditAnywhere, Category = "Addons", meta = (AllowPrivateAccess = "true"))
		UAkAudioEvent* WENothing;

	UPROPERTY(EditAnywhere, Category = PlayerMat, meta = (AllowPrivateAccess = "true"))
		class UMaterialInterface* MatYellow{};

	UPROPERTY(EditAnywhere, Category = PlayerMat, meta = (AllowPrivateAccess = "true"))
		class UMaterialInterface* MatRed{};

	UPROPERTY(EditAnywhere, Category = PlayerMat, meta = (AllowPrivateAccess = "true"))
		class UMaterialInterface* MatBlue{};

	UPROPERTY(EditAnywhere, Category = PlayerMat, meta = (AllowPrivateAccess = "true"))
		class UMaterialInterface* MatPurple{};

	UPROPERTY(EditAnywhere, Category = PlayerMat, meta = (AllowPrivateAccess = "true"))
		class UMaterialInterface* MatOrange{};

	UPROPERTY(EditAnywhere, Category = PlayerMat, meta = (AllowPrivateAccess = "true"))
		class UMaterialInterface* MatBlack{};

	UPROPERTY(EditAnywhere, Category = PlayerMat, meta = (AllowPrivateAccess = "true"))
		class UMaterialInterface* MatCyan{};

	UPROPERTY(EditAnywhere, Category = PlayerMat, meta = (AllowPrivateAccess = "true"))
		class UMaterialInterface* MatWhite{};

	USkeletalMeshComponent* CharMesh{};

	void SwapAddonMesh(bool activateEffect);
	void SwapColorSkin();

	//Save state
	void SaveGame();
	void LoadGame();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Niagara, meta = (AllowPrivateAccess = "true"))
		class UNiagaraSystem* CleaningNiagaraSystem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Niagara, meta = (AllowPrivateAccess = "true"))
		class UNiagaraComponent* CleaningNiagaraComponent = nullptr;

	bool bWasOnGround = true;
	bool bDroneStartedChase = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Tick, meta = (AllowPrivateAccess = "true"))
	bool bDisableTick = false;
};

