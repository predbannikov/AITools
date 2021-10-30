// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
//#include "PhysicsEngine/ConstraintInstance.h"
#include "Kismet/KismetMathLibrary.h"
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
	virtual void BeginDestroy() override;;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	USceneComponent *root = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UArrowComponent* arrow = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* partA = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* partB = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UPhysicsConstraintComponent* physicsConstraint = nullptr;

	//FConstraintInstance constraintInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* pointA = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* pointB = nullptr;

	UFUNCTION(BlueprintCallable)
	void upMember();

	UFUNCTION(BlueprintCallable)
	FVector getForceVector();

};
