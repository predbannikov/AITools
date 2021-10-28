// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "HingeNode.generated.h"

/**
 * 
 */
UCLASS(ClassGroup="Custom")
class AITOOLS_API UHingeNode : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UHingeNode();

private:

	UPROPERTY(EditDefaultsOnly)
	UArrowComponent* arrow = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* partB = nullptr;

	//UPROPERTY(EditDefaultsOnly)
	//UPhysicsConstraintComponent* physicsConstraint = nullptr;

	//UPROPERTY(EditDefaultsOnly)
	//USceneComponent* pointA = nullptr;

	//UPROPERTY(EditDefaultsOnly)
	//USceneComponent* pointB = nullptr;

	
};
