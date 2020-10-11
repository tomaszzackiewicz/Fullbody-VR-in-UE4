// Fill out your copyright notice in the Description page of Project Settings.


#include "VRAnimInstance.h"
#include "VRIKHandsCPPCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "MotionControllerComponent.h"

UVRAnimInstance::UVRAnimInstance() {

}

//This function is like PostInitAnimtree in UE3
void UVRAnimInstance::NativeInitializeAnimation() {
	//Very Important Line
	Super::NativeInitializeAnimation();
	//Cache the owning pawn for use in Tick
	OwningPawn = TryGetPawnOwner();
	VRCharacter = Cast<AVRIKHandsCPPCharacter>(OwningPawn);
	
}

void UVRAnimInstance::NativeUpdateAnimation(float DeltaTimeX) {
	Super::NativeUpdateAnimation(DeltaTimeX);
	//Time = DeltaTimeX;
	UpdateAnimationProperties();
}

void UVRAnimInstance::UpdateAnimationProperties() {

	if (!VRCharacter) {
		return;
	}

	bIsInAir = VRCharacter->GetMovementComponent()->IsFalling();
	Velocity = VRCharacter->GetVelocity();
	Rotation = VRCharacter->GetActorRotation();

	Speed = VRCharacter->GetVelocity().Size();
	Direction = CalculateDirection(Velocity, Rotation);
	if (Speed > 0) {
		bIsMoving = true;
	}
	else {
		bIsMoving = false;
	}

	MotionControllerL = VRCharacter->GetMotionControllerL();
	MotionControllerR = VRCharacter->GetMotionControllerR();
	bIsGripLeft = VRCharacter->GetIsGripLeft();
	bIsGripRight = VRCharacter->GetIsGripRight();

	MotionControllerLTransform = MotionControllerL->GetComponentTransform();
	MotionControllerRTransform = MotionControllerR->GetComponentTransform();
}

void UVRAnimInstance::SetIsCrouched(bool isCrouchedParam){
	bIsCrouched = isCrouchedParam;
}
