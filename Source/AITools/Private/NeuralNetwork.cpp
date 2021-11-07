// Fill out your copyright notice in the Description page of Project Settings.


#include "NeuralNetwork.h"


#define INPUT_NODES		5
#define OUTPUT_NODES	1
#define HIDDEN_NODES	5
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
	output_nodes = OUTPUT_NODES;
	hidden_nodes = HIDDEN_NODES;
	learning_rate = LEARNING_RATE;


}

void ANeuralNetwork::BeginPlay()
{
	controller = GetWorld()->GetFirstPlayerController();

	for (int i = 0, x = 0; i < 10; i++, x += 75) {
		for (int j = 0, y = 0; j < 10; j++, y += 75) {
			int idx = i * 10 + j;
			hinges[idx] = GetWorld()->SpawnActor<AHingePair>(FVector(x, y, 100), FRotator(150, 0, 0));
			hinges[idx]->setIndex(idx);

		}
	}
	input = Mat(input_nodes, 1);
	hidden = Mat(hidden_nodes, 1);
	out = Mat(output_nodes, 1);
	wh = Mat::Random(hidden_nodes, input_nodes) * 0.5;
	wo = Mat::Random(output_nodes, hidden_nodes) * 0.5;
	Super::BeginPlay();
}

void ANeuralNetwork::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Mat target_output(1, 1);
	Mat cur_input;
	float target_response;

	if (controller->IsInputKeyDown(EKeys::R)) {
		counter_tests = 0;
		counter_right_tests = 0;
		UE_LOG(LogTemp, Warning, TEXT("RESET COUNTERS:"));
	}

	switch (state)
	{
	case STATE_FORWARD:

		state = STATE_BACKWARD;
		break;
	case STATE_BACKWARD:
		for (int i = 0; i < 100; i++) {
			if (hinges[i]->getState() != state) {
				UE_LOG(LogTemp, Warning, TEXT("ERROR Sinhronization state"));
				return;
			}
			counter_tests++;
			hinges[i]->getResult(target_response, cur_input);
			printMatrix(cur_input, "INPUT");
			target_output(0, 0) = target_response;
			forward(cur_input);
			if ((out(0, 0) > 0.5 && target_response > 0.5) || (out(0,0) < 0.5 && target_response < 0.5)) {
				counter_right_tests++;
			}
			backward(target_output);

		}
		if(counter_tests == 2000 * 100)
			UKismetSystemLibrary::QuitGame(GetWorld(), controller, EQuitPreference::Quit, true);
		delta_time += DeltaTime;
		if (delta_time > 0.3) {
			UE_LOG(LogTemp, Warning, TEXT("MIDDLE VALUE RESULT %f of %d tests"), (float)counter_right_tests / (float)counter_tests, counter_tests);
			//printMatrix(out, "OUT");

			delta_time = 0;
		}
		state = STATE_FORWARD;
		break;
	default:
		break;
	}
}

inline void ANeuralNetwork::forward(Mat& _input) {
	auto sigmoid = [](const float z) -> float { return 1.0 / (1.0 + exp(-z)); };
	input = _input;
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
