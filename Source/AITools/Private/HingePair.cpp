// Fill out your copyright notice in the Description page of Project Settings.


#include "HingePair.h"
#include "DrawDebugHelpers.h"



// Sets default values
AHingePair::AHingePair()
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
	//UE_LOG(LogTemp, Warning, TEXT("partA loc %s"), *partA->GetRelativeLocation().ToString());


	partB = CreateDefaultSubobject<UStaticMeshComponent>(FName("Part B"));
	partB->SetupAttachment(root);
	//partB->AttachToComponent(root, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	partB->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/SM/SM_Member'")).Object);
	partB->SetRelativeLocation(partB->GetRelativeLocation() - FVector(0, 0, 17.0));
	partB->SetMobility(EComponentMobility::Movable);
	//UE_LOG(LogTemp, Warning, TEXT("partB loc %s"), *partB->GetRelativeLocation().ToString());

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

	partB->OnComponentBeginOverlap.AddDynamic(this, &AHingePair::OnOverlapBegin);        // set up a notification for when this component overlaps something
	partB->OnComponentEndOverlap.AddDynamic(this, &AHingePair::OnOverlapEnd);      // set up a notification for when this component overlaps something

	initPhysicsConstraints();

	root->GetOwner()->SetActorRelativeRotation(FRotator(-50, 0, 0));
	root->GetOwner()->SetActorRelativeLocation(FVector(0, 0, 50));
}

// Called when the game starts or when spawned
void AHingePair::BeginPlay()
{
	startPhysicsConstraints();
	Super::BeginPlay();
	//float mass = partB->GetMass();
	//UE_LOG(LogTemp, Warning, TEXT("mass = %f"), mass);
}

inline void AHingePair::BeginDestroy()
{
	//UE_LOG(LogTemp, Warning, TEXT("befor destroy"));
	Super::BeginDestroy();
	//UE_LOG(LogTemp, Warning, TEXT("after destroy"));
	last_force = 4000.0;
	last_angle = getAngle();

}

// Called every frame
void AHingePair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//printTransform();
	//UE_LOG(LogTemp, Warning, TEXT("%s %d"), *GetName(), test);
	//test++;
	//return ;

	//DrawDebugLine(GetWorld(), partB->GetComponentLocation(), partB->GetComponentLocation() + (partB->GetUpVector() * 30), FColor::Green, false, -1.0, 0, 2);

	//velocity = partB->GetComponentVelocity();

	//AddActorLocalRotation(FRotator(0, FMath::FRandRange(0, 5.0), 0));
	float const ANGLE_TO_BE_TURN = 100.0;

	float angle = getAngle();
	if (FMath::IsNearlyEqual(angle, (float)ANGLE_TO_BE_TURN, (float)0.1))
		return;

	float was_need_angle_turn = ANGLE_TO_BE_TURN - last_angle;
	float new_need_angle_turn = ANGLE_TO_BE_TURN - angle;
	float delta_turn = was_need_angle_turn - new_need_angle_turn;

	float ration_turn = delta_turn / last_delta_turn;

	last_force *= ration_turn;
	FVector force_vector = getVForceDecrease();
	force_vector *= last_force;
	partB->AddForce(force_vector);
	UE_LOG(LogTemp, Warning, TEXT("%f, %f"), last_force, ration_turn);


	last_delta_turn = new_need_angle_turn;
	last_angle = angle;
	/*
	
	counter_frames++;
	float force = 4000;
	float angle = 0;
	float random_angle = 0;
	FVector force_vector = getVForceDecrease();
	switch (state)
	{
	case STATE_FORWARD:
		complate = false;

		angle = getAngle();
		random_angle = FMath::FRandRange(1.0, 50.0);
		if (!FMath::RandBool()) {
			random_angle *= -1.0;
			force_vector *= -1.0;
		}
		if (angle + random_angle < MAX_ANGLE_HINGE && angle + random_angle > 0)
			save_need_angle = random_angle;
		else {
			save_need_angle = -random_angle;
			force_vector *= -1.0;
		}
		last_angle = angle;

		

		koeff = FMath::FRandRange(0, 1) * 0.9999 + 0.00001;
		if(!FMath::RandBool())
			koeff *= -1.0;
		force *= koeff;
		force_vector *= force;
		partB->AddForce(force_vector);

		state = STATE_BACKWARD;
		break;
	case STATE_BACKWARD:
		angle = getAngle();
		if ((save_need_angle > 0 && angle - last_angle > 0) || (save_need_angle < 0 && angle - last_angle < 0)) 
			target_response = 0.999;
		else 
			target_response = 0.001;
		state = STATE_FORWARD;
		complate = true;
		break;
	default:
		break;
	}
	*/
}

void AHingePair::getParameters(float& cur_angle, float& need_angle, FVector& _velocity, float &_koef, float &_target_response, FVector &_up_vector)
{
	cur_angle = getAngle();
	need_angle = save_need_angle;
	_velocity = partB->GetComponentVelocity();
	_koef = koeff;
	_target_response = target_response;
	_up_vector = partB->GetUpVector();
}


FVector AHingePair::getVForceDecrease()
{
	FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(pointB->GetComponentLocation(), pointA->GetComponentLocation());
	FVector forwardVector = UKismetMathLibrary::GetForwardVector(lookAtRotation);
	//UKismetSystemLibrary::DrawDebugArrow(GetWorld(), pointB->GetComponentLocation(), pointA->GetComponentLocation(), 2, FLinearColor::Blue, 0, 1);
	return forwardVector;
}

FVector AHingePair::getVForceIncrease()
{
	FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(pointA->GetComponentLocation(), pointB->GetComponentLocation());
	FVector forwardVector = UKismetMathLibrary::GetForwardVector(lookAtRotation);
	//UKismetSystemLibrary::DrawDebugArrow(GetWorld(), pointB->GetComponentLocation(), pointA->GetComponentLocation(), 2, FLinearColor::Blue, 0, 1);
	return forwardVector;
}

void AHingePair::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Begin overlap %s"), *GetName());
}

void AHingePair::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("End overlap"));
}

void AHingePair::initPhysicsConstraints()
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

void AHingePair::startPhysicsConstraints()
{
	// Часть B (слева) - подвижна
	// Часть A (справа) - на неё крепится PhysicsConstraint
	physicsConstraint->SetConstrainedComponents(partB, "Member B", partA, "Member A");
	partB->SetSimulatePhysics(true);
}

//void AHingePair::getResult(float& target, Mat& mat)
//{
//	target = target_response;
//	mat = input;
//}

float AHingePair::getTargetResponse()
{
	return target_response;
}

float AHingePair::getAngle()
{
	FQuat bet = FQuat::FindBetweenNormals(partA->GetUpVector(), partB->GetUpVector());
	FVector nAxis;
	float angle = 0.0f;
	bet.ToAxisAndAngle(nAxis, angle);
	return MAX_ANGLE_HINGE - FMath::RadiansToDegrees(angle) + ANGLE_ONESIDE_OFFSET;
}

void AHingePair::printTransform()
{
	//DrawDebugLine(GetWorld(), partA->GetComponentLocation(), partA->GetComponentLocation() + (partA->GetUpVector() * 30), FColor::Green, false, -1.0, 0, 2);
	//DrawDebugLine(GetWorld(), partB->GetComponentLocation(), partB->GetComponentLocation() + (partB->GetUpVector() * 30), FColor::Green, false, -1.0, 0, 2);
	//DrawDebugLine(GetWorld(), partB->GetComponentLocation(), partB->GetComponentLocation() + (partB->GetComponentVelocity() * 30), FColor::Blue, false, -1.0, 0, 2);
	UE_LOG(LogTemp, Warning, TEXT("Angle: %f"), getAngle());
}

void AHingePair::payload()
{
	UE_LOG(LogTemp, Warning, TEXT("START PAYLOAD"));
	for (int i = 0; i < 100000; i++) {
		partB->AddForce(FVector(1, 1, 1));
	}
}

int AHingePair::getState()
{
	return state;
}

void AHingePair::setIndex(int idx)
{
	index = idx;
}

int AHingePair::getMaxVelocity()
{
	return MAX_VELOCITY;
}

int AHingePair::getMaxHingeAngle()
{
	return MAX_ANGLE_HINGE;
}

FVector AHingePair::getVelocityPartB()
{
	return velocity;
}

float AHingePair::getKoeff()
{
	return koeff;
}

float AHingePair::getNeedAngle()
{
	return save_need_angle;
}

int AHingePair::getCounterFrames()
{
	return counter_frames;
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

