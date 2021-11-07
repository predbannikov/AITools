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
	NeuralNetwork(int _input_nodes = 5, int _hidden_nodes = 2, int _output_nodes = 2, float _learning_rate = 0.3);
	Mat input;
	Mat wh;
	Mat hidden;
	Mat wo;
	Mat out;
	void forward(Mat& input);
	void backward(Mat& output);
	void printWeight(int num = 0, int _row = 1, int _col = 1);
	Mat getOutput();
	//void train(const std::vector<std::pair<int, std::vector<float>*> >& vec_input);
	//void query(const std::vector<std::pair<int, std::vector<float>*> >& vec_input_query);
	static void printMatrix(Mat mat, FString name = "", int _row = -1, int _col = -1);
private:
	int input_nodes;
	int output_nodes;
	int hidden_nodes;
	float learning_rate;
};


UCLASS()
class AEntity : public AActor
{
	GENERATED_BODY()
		enum STATE { STATE_FORWARD, STATE_BACKWARD } state = STATE_FORWARD;
	float last_angle = 0;
	float save_need_angle = 0;
	int counter_tests = 0;
	int counter_right_tests = 0;
	float delta_time = 0;
	float force_usually = 1500.0f;

	FRotator position_cube;
	NeuralNetwork n;

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

	USceneComponent* root = nullptr;
	APlayerController* controller = nullptr;
	class UInputComponent* InputComponent;

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
