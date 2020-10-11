// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
//#include "Components/PrimitiveComponent.h"

// Sets default values
AItem::AItem(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(Root);

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetCollisionProfileName(TEXT("OverlapAll"));
	Trigger->SetGenerateOverlapEvents(true);
	Trigger->SetupAttachment(Root);
}

void AItem::Grab(){
	
}

// Called when the game starts or when spawned
void AItem::BeginPlay(){
	Super::BeginPlay();
	
}

// Called every frame
void AItem::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

}

