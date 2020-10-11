// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRIKHandsCPPCharacter.generated.h"

UCLASS(config=Game)
class AVRIKHandsCPPCharacter : public ACharacter{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* VROrigin;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* MotionControllerL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* MotionControllerR;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* TriggerL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* TriggerR;

public:
	AVRIKHandsCPPCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	bool bIsGripRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	bool bIsGripLeft;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Character")
	void OnGrabL(bool bIsGrabbedParam);

	UFUNCTION(BlueprintCallable, Category = "Character")
	void OnGrabR(bool bIsGrabbedParam);

	UFUNCTION(BlueprintCallable, Category = "Character")
	void OnReleaseL(bool bIsGrabbedParam);

	UFUNCTION(BlueprintCallable, Category = "Character")
	void OnReleaseR(bool bIsGrabbedParam);

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	void OnGripRight();
	void OnReleaseRight();

	void OnGripLeft();
	void OnReleaseLeft();

	void OnCrouch();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class UMotionControllerComponent* GetMotionControllerL() const { return MotionControllerL; }
	FORCEINLINE class UMotionControllerComponent* GetMotionControllerR() const { return MotionControllerR; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE class USphereComponent* GetBoxColliderL() const { return TriggerL; }
	FORCEINLINE class USphereComponent* GetBoxColliderR() const { return TriggerR; }

	FORCEINLINE bool GetIsGripLeft() { return bIsGripLeft; }
	FORCEINLINE bool GetIsGripRight() { return bIsGripRight; }
	FORCEINLINE bool GetIsCrouched() { return bIsCrouched; }

	UFUNCTION()
	void OnBoxBeginOverlapL(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlapL(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnBoxBeginOverlapR(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlapR(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
		bool bCanBeGrabbedL;
		bool bCanBeGrabbedR;

		class IIGrabItems* CurrentItem;

		bool bIsCrouched;

		float CrouchSpeed;
		float WalkSpeed;
};

