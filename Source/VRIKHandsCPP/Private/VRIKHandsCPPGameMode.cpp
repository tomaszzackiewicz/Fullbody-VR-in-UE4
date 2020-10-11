// Copyright Epic Games, Inc. All Rights Reserved.

#include "VRIKHandsCPPGameMode.h"
#include "VRIKHandsCPPCharacter.h"
#include "UObject/ConstructorHelpers.h"

AVRIKHandsCPPGameMode::AVRIKHandsCPPGameMode(){
	// set default pawn class to our Blueprinted character
	/*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL){
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}*/
}
