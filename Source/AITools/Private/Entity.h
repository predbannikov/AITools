// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
//#include "PhysicsEngine/ConstraintInstance.h"
#include "Eigen/Dense"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Entity.generated.h"

using Mat = Eigen::MatrixXf;


struct NeuralNetwork {
	Mat input;
	Mat wh;
	Mat hidden;
	Mat wo;
	Mat out;
	float rate;
	NeuralNetwork(int input_nodes, int output_nodes, int hiden_nodes, float learning_rate = 0.3);
	void train(Mat &input_nodes);
	void query();
	void printMatrix(Mat mat, FString name = "");
};


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

	/** called when something enters the sphere component */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** called when something leaves the sphere component */
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	float force = 5000.0f;

	FRotator old_position;
	FRotator position_cube;
private:
	void initPhysicsConstraints();
	void startPhysicsConstraints();

	float getAngle();
	void printTransform();
	void printMatrix(Mat mat, FString name = "");
	void printMatrix3(Eigen::Matrix3f mat, FString name = "");


	void testMatrix();

	float Sigmoid(const float z);

	float SigmoidDerivative(const float z);
};
