// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity.h"
#include "DrawDebugHelpers.h"

#define MAX_ANGLE_HINGE			160.		// Когда шарнир развёрнут на максимум
#define ANGLE_ONESIDE_OFFSET	10.
#define MAX_VELOCITY			10000.


#define INPUT_NODES		5
#define OUTPUT_NODES	1
#define HIDDEN_NODES	7

// Sets default values
AEntity::AEntity()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("StaticMesh'/Game/SM/SM_Member'"));
	if (!meshAsset.Succeeded()) {
		UE_LOG(LogTemp, Warning, TEXT("Error: UStaticMesh not load!"));
		return;
	}

	root = CreateDefaultSubobject<USceneComponent>(FName("Root component"));
	SetRootComponent(root);					// Если не назначить, будет предупреждение о назначении на усмотрение редактора
	//sc->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	//sc->SetupAttachment(RootComponent);		// Самому себе не надо назначать

	arrow = CreateDefaultSubobject<UArrowComponent>(FName("Arrow"));
	//arrow->AttachToComponent(sc, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	arrow->SetupAttachment(root);		// прикрепляет как поддерево
	arrow->SetHiddenInGame(false);
	arrow->SetRelativeScale3D(FVector(0.5, 0.5, 0.5));

	partA = CreateDefaultSubobject<UStaticMeshComponent>(FName("Part A"));
	partA->SetupAttachment(root);
	//partA->AttachToComponent(sc, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	partA->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/SM/SM_Member'")).Object);
	partA->SetRelativeLocation(partA->GetRelativeLocation() + FVector(0, 0, 17.0));
	partA->SetMobility(EComponentMobility::Movable);
	UE_LOG(LogTemp, Warning, TEXT("partA loc %s"), *partA->GetRelativeLocation().ToString());


	partB = CreateDefaultSubobject<UStaticMeshComponent>(FName("Part B"));
	partB->SetupAttachment(root);
	//partB->AttachToComponent(root, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	partB->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/SM/SM_Member'")).Object);
	partB->SetRelativeLocation(partB->GetRelativeLocation() - FVector(0, 0, 17.0));
	partB->SetMobility(EComponentMobility::Movable);
	UE_LOG(LogTemp, Warning, TEXT("partB loc %s"), *partB->GetRelativeLocation().ToString());

	pointA = CreateDefaultSubobject<USceneComponent>(FName("Point A"));
	//pointA->AttachToComponent(partA, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	pointA->SetupAttachment(root);
	pointA->SetRelativeLocation(pointA->GetRelativeLocation() + FVector(-5, 0, 15));
	pointA->SetMobility(EComponentMobility::Movable);

	pointB = CreateDefaultSubobject<USceneComponent>(FName("Point B"));
	//pointB->AttachToComponent(partB, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	pointB->SetupAttachment(root);
	pointB->SetRelativeLocation(pointB->GetRelativeLocation() + FVector(-5, 0, -15));
	pointB->SetMobility(EComponentMobility::Movable);

	partB->OnComponentBeginOverlap.AddDynamic(this, &AEntity::OnOverlapBegin);        // set up a notification for when this component overlaps something
	partB->OnComponentEndOverlap.AddDynamic(this, &AEntity::OnOverlapEnd);      // set up a notification for when this component overlaps something


	initPhysicsConstraints();


	root->GetOwner()->SetActorRelativeRotation(FRotator(-50, 0, 0));
	root->GetOwner()->SetActorRelativeLocation(FVector(0, 0, 50));
	//testMatrix();
	n = NeuralNetwork(INPUT_NODES, HIDDEN_NODES, OUTPUT_NODES, 0.3);
}

// Called when the game starts or when spawned
void AEntity::BeginPlay()
{
	startPhysicsConstraints();

	Super::BeginPlay();

	controller = GetWorld()->GetFirstPlayerController();


	float mass = partB->GetMass();
	UE_LOG(LogTemp, Warning, TEXT("mass = %f"), mass);

}

inline void AEntity::BeginDestroy()
{
	//UE_LOG(LogTemp, Warning, TEXT("befor destroy"));
	Super::BeginDestroy();
	//UE_LOG(LogTemp, Warning, TEXT("after destroy"));
}

// Called every frame
void AEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//printTransform();
	//return ;


	if (controller->IsInputKeyDown(EKeys::R)) {
		counter_tests = 0;
		counter_right_tests = 0;
		UE_LOG(LogTemp, Warning, TEXT("RESET COUNTERS:"));
	}



	FVector velocity = partB->GetComponentVelocity();
	//UE_LOG(LogTemp, Warning, TEXT("velocity = %s"), *velocity.ToString());

	float x, z, k, force;
	float angle = 0;
	force = force_usually;
	Mat input, output, out;
	FVector force_vector = getForceVector();
	switch (state)
	{
	case STATE_FORWARD:
		counter_tests++;
		if (FMath::IsNearlyZero(velocity.X))
			x = 1 / MAX_VELOCITY;
		else
			x = velocity.X / MAX_VELOCITY;
		if (FMath::IsNearlyZero(velocity.Z))
			z = 1 / MAX_VELOCITY;
		else
			z = velocity.Z / MAX_VELOCITY;
		angle = getAngle();
		if (angle + 50.0 < MAX_ANGLE_HINGE)
			save_need_angle = 50.0;
		else
			save_need_angle = -50.0;
		last_angle = angle;
		angle /= (MAX_ANGLE_HINGE + 1.5);
		k = FMath::FRandRange(-1, 1) * 0.999;
		input = Mat(INPUT_NODES, 1);

		input << x, z, angle, save_need_angle / MAX_ANGLE_HINGE, k;
		//mat << x, z, angle, save_need_angle / MAX_ANGLE_HINGE, k;
		//printMatrix(input, "input");
		n.forward(input);
		//printMatrix(, "OUTPUT");
		force *= k;
		force_vector *= force;
		partB->AddForce(force_vector);
		state = STATE_BACKWARD;
		break;
	case STATE_BACKWARD:
		angle = getAngle();
		output = Mat(OUTPUT_NODES, 1);

		//UE_LOG(LogTemp, Warning, TEXT("direct = %f \t dCurr = %f \t lastAngle =%f \t curAngle = %f"), save_need_angle, angle - last_angle, last_angle, angle);
		// Если нужно чтоб угол увеличился и он увеличивается
		// и наоборот если нужно чтоб уменьшался и он уменьшается значит true
		out = n.getOutput();
		if ((save_need_angle > 0 && angle - last_angle > 0) || (save_need_angle < 0 && angle - last_angle < 0)) {
			if (out(0, 0) < out(1, 0))
				counter_right_tests++;
			output << 0.999;
			//if(out(0,0) > out(1,0))
			//	counter_right_tests++;
			//output << 0.99, 0.01;
		}
		else {
			if (out(0, 0) > out(1, 0))
				counter_right_tests++;
			output << 0.001;
			//if (out(0, 0) < out(1, 0))
			//	counter_right_tests++;
			//output << 0.01, 0.99;
		}

		delta_time += DeltaTime;
		if (delta_time > 0.3) {
			UE_LOG(LogTemp, Warning, TEXT("MIDDLE VALUE RESULT %f of %d tests"), (float)counter_right_tests / (float)counter_tests, counter_tests);
			printMatrix(out, "OUT");

			delta_time = 0;
		}
		//printMatrix(mat, "output");

		n.backward(output);
		//n.printWeight(0, 2, 2);
		//n.printWeight(1, 2, 2);
		state = STATE_FORWARD;
		break;
	default:
		break;
	}


}

FVector AEntity::getForceVector()
{
	FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(pointB->GetComponentLocation(), pointA->GetComponentLocation());
	FVector forwardVector = UKismetMathLibrary::GetForwardVector(lookAtRotation);
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), pointB->GetComponentLocation(), pointA->GetComponentLocation(), 2, FLinearColor::Blue, 0, 1);
	return forwardVector;
}

void AEntity::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Begin overlap %s"), *GetName());
}

void AEntity::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("End overlap"));
}

void AEntity::initPhysicsConstraints()
{
	physicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("Physics Component"));
	physicsConstraint->SetupAttachment(root);
	physicsConstraint->SetDisableCollision(true);
	//physicsConstraint->ConstraintInstance.SetAngularSwing1Limit()
	physicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 45.0);
	physicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, MAX_ANGLE_HINGE / 2.);		// Выставляем максимальный ход в каждую сторону
	physicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 45.0);
	// Сдвигаем AngularRotationOffset от начальной позиции
	// Приминения AngularRotationOffset лучше выполнять в паре SetRelativeRotation, так отображение красной плоскости в редакторе(рабочего хода шарнира) становится корректным
	physicsConstraint->ConstraintInstance.AngularRotationOffset = FRotator(-(MAX_ANGLE_HINGE / 2. + ANGLE_ONESIDE_OFFSET), 0, 0);
	physicsConstraint->AddRelativeRotation(FRotator(-(MAX_ANGLE_HINGE / 2. + ANGLE_ONESIDE_OFFSET), 0, 0));
	//physicsConstraint->SetRelativeRotation(physicsConstraint->GetRelativeRotation() + FRotator(-85, 0, 0));
}

void AEntity::startPhysicsConstraints()
{
	// Часть B (слева) - подвижна
	// Часть A (справа) - на неё крепится PhysicsConstraint
	physicsConstraint->SetConstrainedComponents(partB, "Member B", partA, "Member A");
	partB->SetSimulatePhysics(true);
}

float AEntity::getAngle()
{
	FQuat bet = FQuat::FindBetweenNormals(partA->GetUpVector(), partB->GetUpVector());
	FVector nAxis;
	float angle = 0.0f;
	bet.ToAxisAndAngle(nAxis, angle);
	return MAX_ANGLE_HINGE - FMath::RadiansToDegrees(angle) + ANGLE_ONESIDE_OFFSET;
}

void AEntity::printTransform()
{
	//DrawDebugLine(GetWorld(), partA->GetComponentLocation(), partA->GetComponentLocation() + (partA->GetUpVector() * 30), FColor::Green, false, -1.0, 0, 2);
	//DrawDebugLine(GetWorld(), partB->GetComponentLocation(), partB->GetComponentLocation() + (partB->GetUpVector() * 30), FColor::Green, false, -1.0, 0, 2);
	//DrawDebugLine(GetWorld(), partB->GetComponentLocation(), partB->GetComponentLocation() + (partB->GetComponentVelocity() * 30), FColor::Blue, false, -1.0, 0, 2);
	UE_LOG(LogTemp, Warning, TEXT("Angle: %f"), getAngle());
}

/*
			Example 1: on component end overlap c++
UFUNCTION()
void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

			Example 2: oncomponentbeginoverlap ue4 c++
			// Signature for OnOverlapBegin
UFUNCTION()
void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
			// In the constructor make sure to use AddDynamic to handle this function dynamically
TriggerComp->OnComponentBeginOverlap.AddDynamic(this, &AClass::OnOverlapBegin);

			Example 3: on component begin overlap c++
UFUNCTION()
void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

*/



void AEntity::testMatrix()
{
	//NeuralNetwork n;

	return;
	auto sigmoid = [](const float z) -> float { return 1.0 / (1.0 + exp(-z)); };
	Eigen::MatrixXf w(3, 3);	// = Eigen::MatrixXf(3, 3);
	w(0, 0) = 0.9;
	w(0, 1) = 0.3;
	w(0, 2) = 0.4;
	w(1, 0) = 0.2;
	w(1, 1) = 0.8;
	w(1, 2) = 0.2;
	w(2, 0) = 0.1;
	w(2, 1) = 0.5;
	w(2, 2) = 0.6;
	printMatrix(w, "w");
	Eigen::MatrixXf i(3, 1);	// = Eigen::MatrixXf(3, 1) ;
	i << 0.9, 0.1, 0.8;
	printMatrix(i, "i");
	Eigen::MatrixXf o = w * i;
	printMatrix(o, "o");
	Eigen::MatrixXf out(3, 1);
	out = o.unaryExpr(sigmoid);
	printMatrix(out, "out");

}

void AEntity::printMatrix(Mat mat, FString name)
{
	int row = mat.rows();
	int col = mat.cols();
	FString str;
	UE_LOG(LogTemp, Warning, TEXT("PRINT %s"), *name);
	for (size_t i = 0; i < row; i++) {
		for (size_t j = 0; j < col; j++) {
			str.Append(FString::Printf(TEXT("%f\t"), (mat(i, j))));
		}
		UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
		str.Reset();
	}
}

void AEntity::printMatrix3(Eigen::Matrix3f mat, FString name)
{
	FString str;
	UE_LOG(LogTemp, Warning, TEXT("PRINT %s"), *name);
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			str.Append(FString::Printf(TEXT("%f\t"), (mat(i, j))));
		}
		UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
		str.Reset();
	}
}

inline float AEntity::Sigmoid(const float z) {
	return 1.0 / (1.0 + exp(-z));
}

inline float AEntity::SigmoidDerivative(const float z) {
	return z * (1.0 - z);
}

// Входные параметры
// partB->GetComponentVelocity().X
// partB->GetComponentVelocity().Z
// Текущий угол
// Угол целевой
// 
NeuralNetwork::NeuralNetwork(int _input_nodes, int _hidden_nodes, int _output_nodes, float _learning_rate) {
	input_nodes = _input_nodes;
	output_nodes = _output_nodes;
	hidden_nodes = _hidden_nodes;
	learning_rate = _learning_rate;

	input = Mat(input_nodes, 1);
	out = Mat(output_nodes, 1);
	hidden = Mat(hidden_nodes, 1);
	wh = Mat::Random(hidden_nodes, input_nodes) * 0.5;
	wo = Mat::Random(output_nodes, hidden_nodes) * 0.5;
}

inline void NeuralNetwork::forward(Mat& _input) {
	auto sigmoid = [](const float z) -> float { return 1.0 / (1.0 + exp(-z)); };
	input = _input;
	hidden = wh * input;
	hidden = hidden.unaryExpr(sigmoid);
	out = wo * hidden;
	out = out.unaryExpr(sigmoid);
}

void NeuralNetwork::backward(Mat& target_list)
{
	Mat out_errors = target_list - out;
	Mat hidden_errors = wo.transpose() * out_errors;
	wo += learning_rate * Mat(out_errors.array() * out.array() * (Mat::Constant(out.rows(), 1, 1.0) - out).array()) * hidden.transpose();
	wh += learning_rate * Mat(hidden_errors.array() * hidden.array() * (Mat::Constant(hidden.rows(), 1, 1.0) - hidden).array()) * input.transpose();
}

void NeuralNetwork::printMatrix(Mat mat, FString name, int _row, int _col)
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

void NeuralNetwork::printWeight(int num, int row, int col)
{
	if (num == 0)
		printMatrix(wh, "WH", row, col);
	else
		printMatrix(wo, "WO", row, col);
}
Mat NeuralNetwork::getOutput()
{
	return out;
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
