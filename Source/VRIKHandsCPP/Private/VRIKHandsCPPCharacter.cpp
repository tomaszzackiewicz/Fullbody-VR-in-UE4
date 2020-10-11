// Copyright Epic Games, Inc. All Rights Reserved.

#include "VRIKHandsCPPCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "MotionControllerComponent.h"
#include "Components/SphereComponent.h"
#include "IGrabItems.h"
#include "Item.h"

//////////////////////////////////////////////////////////////////////////
// AVRIKHandsCPPCharacter

AVRIKHandsCPPCharacter::AVRIKHandsCPPCharacter(){
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	//CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	VROrigin->SetupAttachment(GetMesh());
	VROrigin->SetRelativeLocation(FVector(10.0f, 10.0f, 0.0f));
	VROrigin->SetRelativeRotation(FRotator(0.0f, 90.0f, -90.0f));

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->SetupAttachment(VROrigin);
	
	FollowCamera->SetRelativeLocation(FVector(35.0f, 0.0f, 0.0f));
	FollowCamera->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->bLockToHmd = true;

	MotionControllerL = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerL"));
	MotionControllerL->SetupAttachment(VROrigin);

	MotionControllerR = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerR"));
	MotionControllerR->SetupAttachment(VROrigin);

	TriggerL = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerL"));
	TriggerL->SetCollisionProfileName(TEXT("OverlapAll"));
	TriggerL->SetGenerateOverlapEvents(true);
	TriggerL->SetSphereRadius(10.0f);
	//TriggerL->SetRelativeLocation(FVector(15.0f,0.0f,0.0f));
	TriggerL->SetupAttachment(MotionControllerL);

	TriggerR = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerR"));
	TriggerR->SetCollisionProfileName(TEXT("OverlapAll"));
	TriggerR->SetGenerateOverlapEvents(true);
	TriggerR->SetSphereRadius(10.0f);
	//TriggerR->SetRelativeLocation(FVector(5.0f, -5.0f, 0.0f));
	TriggerR->SetupAttachment(MotionControllerR);

	bIsGripRight = false;
	bIsGripLeft = false;
	bCanBeGrabbedL = false;
	bCanBeGrabbedR = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

// Called when the game starts or when spawned
void AVRIKHandsCPPCharacter::BeginPlay() {
	Super::BeginPlay();

	VROrigin->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "head");
	VROrigin->SetRelativeRotation(FRotator(0.0f, 90.0f, -90.0f));
	VROrigin->SetRelativeLocation(FVector(10.0f, 10.0f, 0.0f));
	FollowCamera->SetRelativeLocation(FVector(35.0f, 0.0f, 0.0f));
	FollowCamera->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	TriggerL->OnComponentBeginOverlap.AddDynamic(this, &AVRIKHandsCPPCharacter::OnBoxBeginOverlapL);
	TriggerL->OnComponentEndOverlap.AddDynamic(this, &AVRIKHandsCPPCharacter::OnBoxEndOverlapL);
	TriggerR->OnComponentBeginOverlap.AddDynamic(this, &AVRIKHandsCPPCharacter::OnBoxBeginOverlapR);
	TriggerR->OnComponentEndOverlap.AddDynamic(this, &AVRIKHandsCPPCharacter::OnBoxEndOverlapR);

	TriggerL->SetRelativeLocation(FVector(5.0f, 5.0f, 0.0f));
	TriggerR->SetRelativeLocation(FVector(5.0f, -5.0f, 0.0f));
}

//////////////////////////////////////////////////////////////////////////
// Input

void AVRIKHandsCPPCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent){
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AVRIKHandsCPPCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVRIKHandsCPPCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AVRIKHandsCPPCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AVRIKHandsCPPCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AVRIKHandsCPPCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AVRIKHandsCPPCharacter::TouchStopped);

	// VR headset functionality
	//PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AVRIKHandsCPPCharacter::OnResetVR);
	PlayerInputComponent->BindAction("GripRight", IE_Pressed, this, &AVRIKHandsCPPCharacter::OnGripRight);
	PlayerInputComponent->BindAction("GripRight", IE_Released, this, &AVRIKHandsCPPCharacter::OnReleaseRight);
	PlayerInputComponent->BindAction("GripLeft", IE_Pressed, this, &AVRIKHandsCPPCharacter::OnGripLeft);
	PlayerInputComponent->BindAction("GripLeft", IE_Released, this, &AVRIKHandsCPPCharacter::OnReleaseLeft);
}

void AVRIKHandsCPPCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	OnResetVR();
}

void AVRIKHandsCPPCharacter::OnResetVR(){
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AVRIKHandsCPPCharacter::OnGripRight(){
	bIsGripRight = true;
	if (bCanBeGrabbedR) {
		OnGrabR(bIsGripRight);
	}
}

void AVRIKHandsCPPCharacter::OnReleaseRight() {
	bIsGripRight = false;
	if (bCanBeGrabbedR) {
		OnReleaseR(bIsGripRight);
	}
}

void AVRIKHandsCPPCharacter::OnGripLeft() {
	bIsGripLeft = true;
	if (bCanBeGrabbedL) {
		OnGrabL(bIsGripLeft);
	}
}

void AVRIKHandsCPPCharacter::OnReleaseLeft() {
	bIsGripLeft = false;
	if (bCanBeGrabbedL) {
		OnReleaseL(bIsGripLeft);
	}
}

void AVRIKHandsCPPCharacter::OnGrabL(bool bIsGrabbedParam) {
	if (CurrentItem) {
		AItem* Item = Cast<AItem>(CurrentItem);
		if (Item) {
			if (Item->GetVisualMesh()) {
				Item->GetVisualMesh()->SetSimulatePhysics(false);
				Item->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, "hand_lSocket");
			}
		}
	}
}

void AVRIKHandsCPPCharacter::OnGrabR(bool bIsGrabbedParam) {
	if (CurrentItem) {
		AItem* Item = Cast<AItem>(CurrentItem);
		if (Item) {
			if (Item->GetVisualMesh()) {
				Item->GetVisualMesh()->SetSimulatePhysics(false);
				Item->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, "hand_rSocket");
			}
		}
	}
}

void AVRIKHandsCPPCharacter::OnReleaseL(bool bIsGrabbedParam){
	if (CurrentItem) {
		AItem* Item = Cast<AItem>(CurrentItem);
		if (Item) {
			if (Item->GetVisualMesh()) {
				Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				Item->GetVisualMesh()->SetSimulatePhysics(true);
			}
		}
	}
}

void AVRIKHandsCPPCharacter::OnReleaseR(bool bIsGrabbedParam){
	if (CurrentItem) {
		AItem* Item = Cast<AItem>(CurrentItem);
		if (Item) {
			if (Item->GetVisualMesh()) {
				Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				Item->GetVisualMesh()->SetSimulatePhysics(true);
			}
		}
	}
}

void AVRIKHandsCPPCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location){
	Jump();
}

void AVRIKHandsCPPCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location){
	StopJumping();
}

void AVRIKHandsCPPCharacter::TurnAtRate(float Rate){
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AVRIKHandsCPPCharacter::LookUpAtRate(float Rate){
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AVRIKHandsCPPCharacter::MoveForward(float Value){
	if ((Controller != NULL) && (Value != 0.0f)){
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AVRIKHandsCPPCharacter::MoveRight(float Value){
	if ( (Controller != NULL) && (Value != 0.0f) ){
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AVRIKHandsCPPCharacter::OnBoxBeginOverlapL(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){
	if (OtherActor && (OtherActor != this)) {
		IIGrabItems* Item = Cast<IIGrabItems>(OtherActor);
		if (Item) {
			CurrentItem = Item;
			bCanBeGrabbedL = true;
		}
	}
}

void AVRIKHandsCPPCharacter::OnBoxEndOverlapL(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){
	if (OtherActor && (OtherActor != this)) {
		IIGrabItems* Item = Cast<IIGrabItems>(OtherActor);
		if (Item) {
			//CurrentItem = nullptr;
			bCanBeGrabbedL = false;
		}
	}
}

void AVRIKHandsCPPCharacter::OnBoxBeginOverlapR(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){
	if (OtherActor && (OtherActor != this)) {
		IIGrabItems* Item = Cast<IIGrabItems>(OtherActor);
		if (Item) {
			CurrentItem = Item;
			bCanBeGrabbedR = true;
		}
	}
}

void AVRIKHandsCPPCharacter::OnBoxEndOverlapR(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex){
	if (OtherActor && (OtherActor != this)) {
		IIGrabItems* Item = Cast<IIGrabItems>(OtherActor);
		if (Item) {
			//CurrentItem = nullptr;
			bCanBeGrabbedR = false;
		}
	}
}

