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

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	USceneComponent* pointA = nullptr;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	USceneComponent* pointB = nullptr;

	UFUNCTION(BlueprintCallable)
	void upMember();

	UFUNCTION(BlueprintCallable)
	FVector getForceVector();

	/** called when something enters the sphere component */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** called when something leaves the sphere component */
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool entry = false;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	float force = 5000.0f;

	FRotator old_position;
	FRotator position_cube;
	float step = 10.0;
	float diff_direct = 0;
	float old_direct = 0;
private:
	void initPhysicsConstraints();
	void startPhysicsConstraints();

	void applyForce(float coefficient);

};
