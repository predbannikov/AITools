// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Eigen/Dense"
#include "HingePair.h"
#include "fstream"
#include "NeuralNetwork.generated.h"


#define ONE_SIDE_HINGES		23


using Mat = Eigen::MatrixXf;

UCLASS()
class AITOOLS_API ANeuralNetwork : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANeuralNetwork();
	Mat input;
	Mat wh;
	Mat hidden;
	Mat wo;
	Mat out;
	void forward();
	void backward(Mat& output);
	void printWeight(int num = 0, int _row = 1, int _col = 1);
	Mat getOutput();
	float Sigmoid(const float z);
	//void train(const std::vector<std::pair<int, std::vector<float>*> >& vec_input);
	//void query(const std::vector<std::pair<int, std::vector<float>*> >& vec_input_query);
	void printMatrix(Mat mat, FString name = "", int _row = -1, int _col = -1);
	int test = 0;
private:
	int input_nodes;
	int output_nodes;
	int hidden_nodes;
	float learning_rate;

	AHingePair *hinges[ONE_SIDE_HINGES * ONE_SIDE_HINGES];
	enum STATE { STATE_FORWARD, STATE_BACKWARD } state = STATE_FORWARD;
	int counter_tests = 0;
	int counter_right_tests = 0;
	float delta_time = 0;

	APlayerController* controller = nullptr;
	int counter_frames = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//std::fstream file;
	int drebezg = 0;
	bool launch = false;
	int count_samples = 0;
	//std::fstream *file = nullptr;


};
