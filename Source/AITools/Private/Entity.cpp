// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity.h"

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

	partB = CreateDefaultSubobject<UStaticMeshComponent>(FName("Part B"));
	partB->SetupAttachment(root);
	//partB->AttachToComponent(sc, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	partB->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/SM/SM_Member'")).Object);
	partB->SetRelativeLocation(partB->GetRelativeLocation() - FVector(0, 0, 17.0 ));
	partB->SetMobility(EComponentMobility::Movable);

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

	old_position = partB->GetComponentRotation();
	
	initPhysicsConstraints();
}

// Called when the game starts or when spawned
void AEntity::BeginPlay()
{
	startPhysicsConstraints();

	Super::BeginPlay();

	//UE_LOG(LogTemp, Warning, TEXT("%s "), *pointA->GetRelativeLocation().ToString());
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
	FRotator rot = partB->GetComponentRotation();

	float direct = rot.Pitch - old_position.Pitch;
	float signUp = 1;
	if(rot.Pitch >= old_position.Pitch)
		signUp = -1;		// Знак поднимается или нет, 1 поднимается -1 опускается

		
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
	if(partB->GetComponentRotation().Pitch > -30)
		forwardVector *= -1;
	return forwardVector;
}

void AEntity::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	force -= 100;
	entry = true;
	position_cube = partB->GetRelativeRotation();
	UE_LOG(LogTemp, Warning, TEXT("Begin overlap rot%s"), *position_cube.ToString());
}

void AEntity::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("End overlap"));
	entry = false;
}

void AEntity::initPhysicsConstraints()
{
	physicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("Physics Component"));
	physicsConstraint->SetupAttachment(root);

	physicsConstraint->SetDisableCollision(true);

	physicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 45.0);
	physicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, 75.0);
	physicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 45.0);

	// Приминения AngularRotationOffset лучше выполнять в паре SetRelativeRotation, так отображение красной плоскости в редакторе(рабочего хода шарнира) становится корректным
	physicsConstraint->ConstraintInstance.AngularRotationOffset = FRotator(-85, 0, 0);
	physicsConstraint->SetRelativeRotation(physicsConstraint->GetRelativeRotation() + FRotator(-85, 0, 0));
}

void AEntity::startPhysicsConstraints()
{
	// Часть B (слева) - подвижна
	// Часть A (справа) - на неё крепится PhysicsConstraint
	physicsConstraint->SetConstrainedComponents(partB, "Member B", partA, "Member A");
	partB->SetSimulatePhysics(true);
}

void AEntity::applyForce(float coefficient)
{
	FVector force_forward = getForceVector();
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
