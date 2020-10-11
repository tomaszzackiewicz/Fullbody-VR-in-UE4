// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "VRAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class VRIKHANDSCPP_API UVRAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UVRAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

	UPROPERTY()
	APawn* OwningPawn;

	UPROPERTY()
	class AVRIKHandsCPPCharacter* VRCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRAnimInstance")
	bool bIsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRAnimInstance")
	bool bIsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRAnimInstance")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRAnimInstance")
	bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRAnimInstance")
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRAnimInstance")
	bool bIsGripLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRAnimInstance")
	bool bIsGripRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRAnimInstance")
	class UMotionControllerComponent* MotionControllerL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRAnimInstance")
	class UMotionControllerComponent* MotionControllerR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRAnimInstance")
	FTransform MotionControllerLTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRAnimInstance")
	FTransform MotionControllerRTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VRAnimInstance")
	bool bIsCrouched;

	FVector Velocity;

	FRotator Rotation;

	void SetIsCrouched(bool isCrouchedParam);

private:
	void UpdateAnimationProperties();

	
	
};
