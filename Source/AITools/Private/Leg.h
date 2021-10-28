// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Leg.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ULeg : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	ULeg();

private:
	UPROPERTY(EditDefaultsOnly)
	UArrowComponent* arrow = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* partB = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UPhysicsConstraintComponent* physicsConstraint = nullptr;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* pointA = nullptr;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* pointB = nullptr;
	
};
