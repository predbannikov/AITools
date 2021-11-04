// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity.h"
#include "DrawDebugHelpers.h"


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
	SetRootComponent(root);					// ≈сли не назначить, будет предупреждение о назначении на усмотрение редактора
	//sc->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	//sc->SetupAttachment(RootComponent);		// —амому себе не надо назначать

	arrow = CreateDefaultSubobject<UArrowComponent>(FName("Arrow"));
	//arrow->AttachToComponent(sc, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	arrow->SetupAttachment(root);		// прикрепл€ет как поддерево
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
	partB->SetRelativeLocation(partB->GetRelativeLocation() - FVector(0, 0, 17.0 ));
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


	old_position = partB->GetComponentRotation();

	root->GetOwner()->SetActorRelativeRotation(FRotator(-50, 0, 0));
	root->GetOwner()->SetActorRelativeLocation(FVector(0, 0, 50));
	//testMatrix();

}

// Called when the game starts or when spawned
void AEntity::BeginPlay()
{
	default_rot = FRotator(90, 0, 0) - partB->GetUpVector().Rotation();
	UE_LOG(LogTemp, Warning, TEXT("default_rot = %s"), *default_rot.ToString());

	startPhysicsConstraints();

	Super::BeginPlay();
	float mass = partB->GetMass();
	UE_LOG(LogTemp, Warning, TEXT("mass = %f"), mass);
	FVector velosity = partB->GetComponentVelocity();
	UE_LOG(LogTemp, Warning, TEXT("velosity = %s"), *velosity.ToString());



	//printMatrix(w);
	//UE_LOG(LogTemp, Warning, TEXT("mat=%f"), m(0,1));
	//Eigen::MatrixXd m = Eigen::MatrixXd::Random(3, 3);
	//UE_LOG(LogTemp, Warning, TEXT("%s "), *pointA->GetRelativeLocation().ToString());
}

void AEntity::testMatrix()
{
	auto sigmoid = [](const float z) -> float { return 1.0 / (1.0 + exp(-z)); }; 
	Eigen::MatrixXf w(3, 3);	// = Eigen::MatrixXf(3, 3);
	w(0, 0) = 0.9;
	w(1, 0) = 0.3;
	w(2, 0) = 0.4;
	w(0, 1) = 0.2;
	w(1, 1) = 0.8;
	w(2, 1) = 0.2;
	w(0, 2) = 0.1;
	w(1, 2) = 0.5;
	w(2, 2) = 0.6;
	printMatrix(w, "w");
	Eigen::MatrixXf i(3, 1);	// = Eigen::MatrixXf(3, 1) ;
	i << 0.9 , 0.1 , 0.8;
	printMatrix(i, "i");
	Eigen::MatrixXf o = w.transpose() * i;
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

inline void AEntity::BeginDestroy() 
{
	UE_LOG(LogTemp, Warning, TEXT("befor destroy"));
	Super::BeginDestroy();
	UE_LOG(LogTemp, Warning, TEXT("after destroy"));
}

// Called every frame
void AEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	printTransform();
	return ;	// !!!!!!!!!!!!!!!!!!!!


	FRotator rot = partB->GetComponentRotation() - default_rot;


	UE_LOG(LogTemp, Warning, TEXT("rot = %s direct=%f sign=%f"), *rot.ToString());

	return;		// #!!!!!!!!!!!!!!!!!!!!


	float direct = rot.Pitch - old_position.Pitch;
	float signUp = 1;
	if(rot.Pitch >= old_position.Pitch)
		signUp = -1;		// «нак поднимаетс€ или нет, 1 поднимаетс€ -1 опускаетс€

		
		//if(diff_direct > direct - old_direct)
		//	step += 1;
		//else
		//	step -= 1;
		
	if (rot.Pitch < old_position.Pitch) {
		force -= step;
	}
	else if (rot.Pitch == old_position.Pitch) {
		force += step;
	}
	
	if (entry) {
		
	}


	UE_LOG(LogTemp, Warning, TEXT("force = %f direct=%f sign=%f"), force, direct, signUp);

	old_direct = direct;
	FVector force_vector = getForceVector() * force;
	partB->AddForce(force_vector);
	old_position = rot;
}

void AEntity::upMember()
{
}

FVector AEntity::getForceVector()
{
	//UE_LOG(LogTemp, Warning, TEXT("%s   %s"), *pointA->GetRelativeLocation().ToString() , *pointB->GetComponentLocation().ToString());
	FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(pointB->GetComponentLocation(), pointA->GetComponentLocation());
	FVector forwardVector = UKismetMathLibrary::GetForwardVector(lookAtRotation);
	//if(partB->GetComponentRotation().Pitch > -30)
	//	forwardVector *= -1;
	return forwardVector;
}

void AEntity::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//force -= 100;
	//entry = true;
	//position_cube = partB->GetRelativeRotation();
	UE_LOG(LogTemp, Warning, TEXT("Begin overlap %s"), *GetName());
}

void AEntity::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("End overlap"));
	//entry = false;
}


#define MAX_ANGLE_HINGE			160
#define ANGLE_ONESIDE_OFFSET	10
void AEntity::initPhysicsConstraints()
{
	physicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("Physics Component"));
	physicsConstraint->SetupAttachment(root);

	physicsConstraint->SetDisableCollision(true);
	//physicsConstraint->ConstraintInstance.SetAngularSwing1Limit()

	physicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 45.0);
	physicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, MAX_ANGLE_HINGE / 2.);		// ¬ыставл€ем максимальный ход в каждую сторону
	physicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 45.0);

	// —двигаем AngularRotationOffset от начальной позиции
	// ѕриминени€ AngularRotationOffset лучше выполн€ть в паре SetRelativeRotation, так отображение красной плоскости в редакторе(рабочего хода шарнира) становитс€ корректным
	physicsConstraint->ConstraintInstance.AngularRotationOffset = FRotator(-(MAX_ANGLE_HINGE / 2. + ANGLE_ONESIDE_OFFSET), 0, 0);
	physicsConstraint->AddRelativeRotation(FRotator(-(MAX_ANGLE_HINGE / 2. + ANGLE_ONESIDE_OFFSET), 0, 0));
	//physicsConstraint->SetRelativeRotation(physicsConstraint->GetRelativeRotation() + FRotator(-85, 0, 0));
}

void AEntity::startPhysicsConstraints()
{
	// „асть B (слева) - подвижна
	// „асть A (справа) - на неЄ крепитс€ PhysicsConstraint
	physicsConstraint->SetConstrainedComponents(partB, "Member B", partA, "Member A");
	partB->SetSimulatePhysics(true);
}

void AEntity::applyForce(float coefficient)
{
	FVector force_forward = getForceVector();
}

float AEntity::getAngle()
{
	FQuat bet = FQuat::FindBetweenNormals(partA->GetUpVector(), partB->GetUpVector());
	FVector nAxis;
	float angle = 0.0f;
	bet.ToAxisAndAngle(nAxis, angle);
	return FMath::RadiansToDegrees(angle) - ANGLE_ONESIDE_OFFSET;
}

void AEntity::printTransform()
{
	DrawDebugLine(GetWorld(), partA->GetComponentLocation(), partA->GetComponentLocation() + (partA->GetUpVector() * 30), FColor::Green, false, -1.0, 0, 2);
	DrawDebugLine(GetWorld(), partB->GetComponentLocation(), partB->GetComponentLocation() + (partB->GetUpVector() * 30), FColor::Green, false, -1.0, 0, 2);
	DrawDebugLine(GetWorld(), partB->GetComponentLocation(), partB->GetComponentLocation() + (partB->GetComponentVelocity() * 30), FColor::Blue, false, -1.0, 0, 2);
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
