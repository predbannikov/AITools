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
#include "HingePair.generated.h"

#define MAX_ANGLE_HINGE			160.		// Когда шарнир развёрнут на максимум
#define ANGLE_ONESIDE_OFFSET	10.
#define MAX_VELOCITY			10000.

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

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FVector getVForceDecrease();
	FVector getVForceIncrease();
	

	enum STATE { STATE_FORWARD, STATE_BACKWARD } state = STATE_FORWARD;
	//void getResult(float &target, Mat &mat);
	float getTargetResponse();
	float getAngle();
	void printTransform();
	void payload();
	int getState();
	void setIndex(int idx);
	int getMaxVelocity();
	int getMaxHingeAngle();
	FVector getVelocityPartB();
	float getKoeff();
	float getNeedAngle();
	int getCounterFrames();
	void getParameters(float &cur_angle, float &need_angle, FVector &_velocity, float &_koef, float &_target_response, FVector &_up_vector);

private:
	void initPhysicsConstraints();
	void startPhysicsConstraints();

	bool complate;
	int index = -1;
	float koeff = -1;
	FVector velocity;
	FVector groundForce;
	float save_need_angle = 0;
	float last_angle = -100.0f;
	float target_response;
	int test = 0;
	int counter_frames = 0;



};
