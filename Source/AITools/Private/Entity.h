// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Entity.generated.h"

UCLASS()
class AEntity : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEntity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditDefaultsOnly)
	UArrowComponent* arrow = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* partA = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* partB = nullptr;

	UPROPERTY(EditDefaultsOnly)
	UPhysicsConstraintComponent* physicsConstraint = nullptr;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* pointA = nullptr;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* pointB = nullptr;

};
