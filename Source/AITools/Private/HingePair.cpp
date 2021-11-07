// Fill out your copyright notice in the Description page of Project Settings.


#include "HingePair.h"
#include "DrawDebugHelpers.h"

#define MAX_ANGLE_HINGE			160.		// Когда шарнир развёрнут на максимум
#define ANGLE_ONESIDE_OFFSET	10.
#define MAX_VELOCITY			10000.


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
}

// Called every frame
void AHingePair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//printTransform();
	//return ;

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *GetName());


	FVector velocity = partB->GetComponentVelocity();
	//UE_LOG(LogTemp, Warning, TEXT("velocity = %s"), *velocity.ToString());

	float k, force = 1500;
	float x, z;
	float angle = 0;

	FVector force_vector = getForceVector();
	switch (state)
	{
	case STATE_FORWARD:
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
		else {
			save_need_angle = -50.0;
			force_vector *= -1.0;
		}
		last_angle = angle;
		if (angle > MAX_ANGLE_HINGE) 
			angle = 0.999;
		else if(angle < 0) 
			angle = 0.001;
		else
			angle /= MAX_ANGLE_HINGE;
		

		k = FMath::FRandRange(-1, 1) * 0.9999;
		input = Mat(5, 1);
		input << x, z, angle, save_need_angle / MAX_ANGLE_HINGE, k;
		force *= k;
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
		break;
	default:
		break;
	}

}

FVector AHingePair::getForceVector()
{
	FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(pointB->GetComponentLocation(), pointA->GetComponentLocation());
	FVector forwardVector = UKismetMathLibrary::GetForwardVector(lookAtRotation);
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), pointB->GetComponentLocation(), pointA->GetComponentLocation(), 2, FLinearColor::Blue, 0, 1);
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

void AHingePair::getResult(float& target, Mat& mat)
{
	target = target_response;
	mat = input;
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

