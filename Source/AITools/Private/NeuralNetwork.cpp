// Fill out your copyright notice in the Description page of Project Settings.


#include "NeuralNetwork.h"


#define INPUT_NODES		8
#define HIDDEN_NODES	12
#define OUTPUT_NODES	1
#define LEARNING_RATE	0.3


// Входные параметры
// partB->GetComponentVelocity().X
// partB->GetComponentVelocity().Z
// Текущий угол
// Угол целевой
ANeuralNetwork::ANeuralNetwork()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	input_nodes = INPUT_NODES;
	hidden_nodes = HIDDEN_NODES;
	output_nodes = OUTPUT_NODES;
	learning_rate = LEARNING_RATE;
}

void ANeuralNetwork::BeginPlay()
{
/*	file.open("U:\\projects\\testingAITools\\AITools\\Saved\\file_training.csv", std::ios::out | std::ios::trunc | std::ios::ate);
	if (!file.is_open()) {
		UE_LOG(LogTemp, Warning, TEXT("File not opening"));
	}
	controller = GetWorld()->GetFirstPlayerController();

	for (int i = 0, x = 0; i < ONE_SIDE_HINGES; i++, x += 100) {
		for (int j = 0, y = 0; j < ONE_SIDE_HINGES; j++, y += 100) {
			int idx = i * ONE_SIDE_HINGES + j;
			hinges[idx] = GetWorld()->SpawnActor<AHingePair>(FVector(x, y, 0), FRotator(0, 0, 0));
			hinges[idx]->setIndex(idx);
		}
	}
	int X_rot = 0;
	int Y_rot = 0;
	int Z_rot = 0;
	for (int i = 0, x = 0; i < ONE_SIDE_HINGES; i++, x += 75) {
		for (int j = 0, y = 0; j < ONE_SIDE_HINGES; j++, y += 75) {
			int idx = i * ONE_SIDE_HINGES + j;
			hinges[idx]->AddActorLocalRotation(FRotator(Y_rot, Z_rot, X_rot));
			X_rot += 45;
			if (X_rot >= 360 - 45) {
				Y_rot += 45;
				X_rot = 0;
			}
			if (Y_rot >= 360 - 45) {
				Z_rot += 45;
				Y_rot = 0;
			}

		}
	}


	input = Mat(input_nodes, 1);
	hidden = Mat(hidden_nodes, 1);
	out = Mat(output_nodes, 1);
	wh = Mat::Random(hidden_nodes, input_nodes) * 0.5;
	wo = Mat::Random(output_nodes, hidden_nodes) * 0.5;
	*/
	Super::BeginPlay();
}

void ANeuralNetwork::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	counter_frames++;
	//UE_LOG(LogTemp, Warning, TEXT("%s %d"), *GetName(), test);
	//test++;
	//return;


	if (controller->IsInputKeyDown(EKeys::R) && drebezg == 0) {
		
		counter_tests = 0;
		counter_right_tests = 0;
		UE_LOG(LogTemp, Warning, TEXT("RESET COUNTERS:"));
		drebezg++;
		delta_time = 0;
		launch = !launch;

	}
	//for (int i = 0; i < ONE_SIDE_HINGES * ONE_SIDE_HINGES; i++) {
	//	hinges[i]->AddActorLocalRotation(FRotator(FMath::FRandRange(-1.0, 1.0), FMath::FRandRange(-1.0, 1.0), FMath::FRandRange(-1.0, 1.0)));
	//}

	
	Mat target_output(1, 1);
	Mat cur_input;
	float angle;
	float koeff;
	float anlgeTo;
	float target_response;
	float x, z;
	FVector velocity;
	FVector vector_up;
	std::string line_to_write;
	switch (state)
	{
	case STATE_FORWARD:

		state = STATE_BACKWARD;
		break;
	case STATE_BACKWARD:
		if (hinges[0]->getCounterFrames() != counter_frames) {
			UE_LOG(LogTemp, Warning, TEXT("ERROR Sinhronization state"));
			return;
		}
		
		for (int i = 0; i < ONE_SIDE_HINGES * ONE_SIDE_HINGES; i++) {
			counter_tests++;
			hinges[i]->getParameters(angle, anlgeTo, velocity, koeff, target_response, vector_up);
			vector_up *= 0.999;
			vector_up += FVector(0.0001, 0.0001, 0.0001);
			if (FMath::IsNearlyZero(velocity.X))
				x = 1 / MAX_VELOCITY;
			else
				x = velocity.X / MAX_VELOCITY;
			if (FMath::IsNearlyZero(velocity.Z))
				z = 1 / MAX_VELOCITY;
			else
				z = velocity.Z / MAX_VELOCITY;

			if (angle > MAX_ANGLE_HINGE)
				angle = 0.9999;
			else if (angle < 0)
				angle = 0.0001;
			else
				angle /= MAX_ANGLE_HINGE;


			
			input = Mat(INPUT_NODES, 1);
			//input << x, z, vector_up.X, vector_up.Y, vector_up.Z, angle, anlgeTo / MAX_ANGLE_HINGE, koeff;
			//printMatrix(input, "INPUT");
			//forward();



			if ((out(0, 0) > 0.5 && target_response > 0.5) || (out(0,0) < 0.5 && target_response < 0.5)) {
				counter_right_tests++;
			}

			target_output(0, 0) = target_response;
			//backward(target_output);
			if (delta_time > 0.23 && launch) {
				//printMatrix(out, "OUT");

				line_to_write += std::to_string(target_response);
				line_to_write += ",";
				line_to_write += std::to_string(x);
				line_to_write += ",";
				line_to_write += std::to_string(z);
				line_to_write += ",";
				line_to_write += std::to_string(vector_up.X);
				line_to_write += ",";
				line_to_write += std::to_string(vector_up.Y);
				line_to_write += ",";
				line_to_write += std::to_string(vector_up.Z);
				line_to_write += ",";
				line_to_write += std::to_string(angle);
				line_to_write += ",";
				line_to_write += std::to_string(anlgeTo / MAX_ANGLE_HINGE);
				line_to_write += ",";
				line_to_write += std::to_string(koeff);
				line_to_write += "\n";
				//file << line_to_write;
				line_to_write.clear();
				//UE_LOG(LogTemp, Warning, TEXT("MIDDLE VALUE RESULT %f of %d tests"), (float)counter_right_tests / (float)counter_tests, counter_tests);
				drebezg = 0;
				if (i == ONE_SIDE_HINGES * ONE_SIDE_HINGES - 1) {
					UE_LOG(LogTemp, Warning, TEXT("Count samples %d "), count_samples);
					delta_time = 0;
				}
				count_samples++;
			}

			if (delta_time > 0.23) {
				hinges[i]->AddActorLocalRotation(FRotator(FMath::FRandRange(-10.0, 10.0), FMath::FRandRange(-10.0, 10.0), FMath::FRandRange(-10.0, 10.0)));
				if (i == ONE_SIDE_HINGES * ONE_SIDE_HINGES - 1) {
					//UE_LOG(LogTemp, Warning, TEXT("Count samples %d "), count_samples);
					delta_time = 0;
				}
			}
		}
		
		//if(counter_tests == 200 * 100)
		//	UKismetSystemLibrary::QuitGame(GetWorld(), controller, EQuitPreference::Quit, true);
		delta_time += DeltaTime;
		//if (delta_time > 0.5) {
		////	//printMatrix(out, "OUT");
		////	UE_LOG(LogTemp, Warning, TEXT("MIDDLE VALUE RESULT %f of %d tests"), (float)counter_right_tests / (float)counter_tests, counter_tests);
		////	for (int i = 0; i < ONE_SIDE_HINGES * ONE_SIDE_HINGES; i++) {
		////		hinges[i]->AddActorLocalRotation(FRotator(FMath::FRandRange(-1.0, 1.0), FMath::FRandRange(-1.0, 1.0), FMath::FRandRange(-1.0, 1.0)));
		////	}
		//	delta_time = 0;
		//}
		state = STATE_FORWARD;
		break;
	default:
		break;
	}
	
}

inline void ANeuralNetwork::forward() {
	auto sigmoid = [](const float z) -> float { return 1.0 / (1.0 + exp(-z)); };
	//input = _input;
	hidden = wh * input;
	hidden = hidden.unaryExpr(sigmoid);
	out = wo * hidden;
	out = out.unaryExpr(sigmoid);
}

void ANeuralNetwork::backward(Mat& target_list)
{
	Mat out_errors = target_list - out;
	Mat hidden_errors = wo.transpose() * out_errors;
	wo += learning_rate * Mat(out_errors.array() * out.array() * (Mat::Constant(out.rows(), 1, 1.0) - out).array()) * hidden.transpose();
	wh += learning_rate * Mat(hidden_errors.array() * hidden.array() * (Mat::Constant(hidden.rows(), 1, 1.0) - hidden).array()) * input.transpose();
}

void ANeuralNetwork::printMatrix(Mat mat, FString name, int _row, int _col)
{
	int row, col;
	if (_row == -1)
		row = mat.rows();
	else
		row = _row;
	if (_col == -1)
		col = mat.cols();
	else
		col = _col;
	FString str;
	UE_LOG(LogTemp, Warning, TEXT("PRINT %s"), *name);
	for (size_t i = 0; i < row; i++) {
		for (size_t j = 0; j < col; j++)
			str.Append(FString::Printf(TEXT("%f\t"), (mat(i, j))));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
		str.Reset();
	}
}

void ANeuralNetwork::printWeight(int num, int row, int col)
{
	if (num == 0)
		printMatrix(wh, "WH", row, col);
	else
		printMatrix(wo, "WO", row, col);
}
Mat ANeuralNetwork::getOutput()
{
	return out;
}

inline float ANeuralNetwork::Sigmoid(const float z) {
	return 1.0 / (1.0 + exp(-z));
}

inline void ANeuralNetwork::BeginDestroy()
{
	//file.close();
	//delete file;
	Super::BeginDestroy();
	//UE_LOG(LogTemp, Warning, TEXT("after destroy"));
}

//inline void NeuralNetwork::query(const std::vector<std::pair<int, std::vector<float>*> >& vec_input_query) {
	//int count_query = 0;
	//int count_right_query = 0;
	//for (auto it = vec_input_query.begin(); it != vec_input_query.end(); it++) {
	//	const std::vector<float>& v = *(*it).second;
	//	int target = (*it).first;

	//	for (int i = 0; i < v.size(); i++)
	//		input(i, 0) = v[i];

	//	forward();
	//	std::cout << "Number:\t(" << target << ")";
	//	float max_koeff = out(0, 0);
	//	int num = 0;
	//	for (int i = 1; i < out.rows(); i++) {
	//		if (max_koeff < out(i, 0)) {
	//			max_koeff = out(i, 0);
	//			num = i;
	//		}
	//	}
	//	std::cout << " - " << max_koeff << " " << num << std::endl;
	//	count_query++;
	//	if (num == target)
	//		count_right_query++;
	//}
	//std::cout << "\n middle value right = " << (float)count_right_query / (float)count_query * 100.0 << std::endl;
//}

//void NeuralNetwork::train(const std::vector<std::pair<int, std::vector<float>*> >& vec_input)
//{
	//for (auto it = vec_input.begin(); it != vec_input.end(); it++) {
	//	const std::vector<float>& v = *(*it).second;
	//	int target = (*it).first;

	//	for (int i = 0; i < v.size(); i++)
	//		input(i, 0) = (v[i] / 255.) * 0.99 + 0.01;

	//	Mat target_output = Mat::Zero(10, 1) + Mat::Constant(10, 1, 0.01);
	//	target_output(target, 0) = 0.99;

	//	forward();
	//	backward(target_output);
	//}
//}
