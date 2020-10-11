// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IGrabItems.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class VRIKHANDSCPP_API AItem : public AActor, public IIGrabItems
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	void Grab();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* VisualMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Trigger;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE class USceneComponent* GetMotionControllerL() const { return Root; }
	FORCEINLINE class UStaticMeshComponent* GetMotionControllerR() const { return VisualMesh; }
	FORCEINLINE class UBoxComponent* GetBox() const { return Trigger; }
	FORCEINLINE const AItem* GetThis() const { return this; }
};
