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

	USceneComponent *sc = CreateDefaultSubobject<USceneComponent>(FName("Root component"));
	SetRootComponent(sc);					// Если не назначить, будет предупреждение о назначении на усмотрение редактора
	sc->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	sc->SetupAttachment(RootComponent);

	physicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("Physics Component"));
	physicsConstraint->AttachToComponent(sc, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	physicsConstraint->SetupAttachment(sc);

	arrow = CreateDefaultSubobject<UArrowComponent>(FName("Arrow"));
	arrow->AttachToComponent(sc, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	arrow->SetupAttachment(sc);		// прикрепляет как поддерево
	arrow->SetHiddenInGame(false);
	arrow->SetRelativeScale3D(FVector(0.5, 0.5, 0.5));

	partA = CreateDefaultSubobject<UStaticMeshComponent>(FName("Part A"));
	partA->SetupAttachment(sc);
	partA->AttachToComponent(sc, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	partA->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/SM/SM_Member'")).Object);
	partA->SetRelativeLocation(partA->GetRelativeLocation() + FVector(0, 0, 17.0));
	partA->SetMobility(EComponentMobility::Movable);

	partB = CreateDefaultSubobject<UStaticMeshComponent>(FName("Part B"));
	partB->SetupAttachment(sc);
	partB->AttachToComponent(sc, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	partB->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/SM/SM_Member'")).Object);
	partB->SetRelativeLocation(partB->GetRelativeLocation() - FVector(0, 0, 17.0 ));
	partB->SetMobility(EComponentMobility::Movable);

	pointA = CreateDefaultSubobject<USceneComponent>(FName("Point A"));
	pointA->AttachToComponent(sc, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	pointA->SetupAttachment(partA);
	pointA->SetRelativeLocation(pointA->GetRelativeLocation() + FVector(-5, 0, 5));
	pointA->SetMobility(EComponentMobility::Movable);

	pointB = CreateDefaultSubobject<USceneComponent>(FName("Point B"));
	pointB->AttachToComponent(sc, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	pointB->SetupAttachment(partB);
	pointB->SetRelativeLocation(pointB->GetRelativeLocation() + FVector(-5, 0, 5));
	pointB->SetMobility(EComponentMobility::Movable);


	//physicsConstraint->SetConstrainedComponents(partA, "Member A", partB, "Member B");

	 
}

// Called when the game starts or when spawned
void AEntity::BeginPlay()
{
	//UE_LOG(LogTemp, Warning, TEXT("%s "), *pointA->GetRelativeLocation().ToString());
	physicsConstraint->SetConstrainedComponents(partA, "Member A", partB, "Member B");
	physicsConstraint->SetDisableCollision(true);
	physicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 45.0);
	physicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, 75.0);
	physicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 45.0);
	
	//physicsConstraint->SetRelativeRotation(physicsConstraint->GetRelativeRotation() + FRotator(0, 85.0, 0));

	Super::BeginPlay();
	partB->SetSimulatePhysics(true);
	
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

}

void AEntity::upMember()
{
	
	//pointA->GetComponentLocation();

	
}

FVector AEntity::getForceVector()
{
	//UE_LOG(LogTemp, Warning, TEXT("%s   %s"), *pointA->GetRelativeLocation().ToString() , *pointB->GetComponentLocation().ToString());
	FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(pointA->GetComponentLocation(), pointB->GetComponentLocation());
	FVector forwardVector = UKismetMathLibrary::GetForwardVector(lookAtRotation);

	return forwardVector;
}

