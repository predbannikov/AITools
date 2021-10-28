// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"

#include "GameFramework/Actor.h"
#include "HingeToPair.generated.h"

UCLASS()
class AITOOLS_API AHingeToPair : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHingeToPair();

protected:
	UPROPERTY(EditDefaultsOnly)
	UArrowComponent *arrow = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent *partA = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent *partB = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UPhysicsConstraintComponent *physicsConstraint = nullptr;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent *pointA = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	USceneComponent *pointB = nullptr;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

};
