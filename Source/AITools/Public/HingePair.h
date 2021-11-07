// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Eigen/Dense"
#include "HingePair.generated.h"

using Mat = Eigen::MatrixXf;

UCLASS()
class AITOOLS_API AHingePair : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHingePair();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	USceneComponent* root = nullptr;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	UArrowComponent* arrow = nullptr;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	UStaticMeshComponent* partA = nullptr;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	UStaticMeshComponent* partB = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UPhysicsConstraintComponent* physicsConstraint = nullptr;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	USceneComponent* pointA = nullptr;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	USceneComponent* pointB = nullptr;

	UFUNCTION(BlueprintCallable)
	FVector getForceVector();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	enum STATE { STATE_FORWARD, STATE_BACKWARD } state = STATE_FORWARD;
	void getResult(float &target, Mat &mat);
	float getAngle();
	void printTransform();
	void payload();
	int getState();
	void setIndex(int idx);

private:
	void initPhysicsConstraints();
	void startPhysicsConstraints();

	int index = -1;
	Mat input;
	float save_need_angle = 0;
	float last_angle = 0;
	float target_response;
};
