// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity.h"

// Sets default values
AEntity::AEntity()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	arrow = CreateDefaultSubobject<UArrowComponent>(FName("Arrow"));
	//arrow->AttachToComponent(RootComponent);
	arrow->SetupAttachment(RootComponent);
	//SetRootComponent(arrow);
	//AttachToComponent(arrow, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));

	partA = CreateDefaultSubobject<UStaticMeshComponent>(FName("Part A"));


	partB = CreateDefaultSubobject<UStaticMeshComponent>(FName("Part B"));

	physicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("Physics Component"));
	pointA = CreateDefaultSubobject<USceneComponent>(FName("Point A"));
	pointB = CreateDefaultSubobject<USceneComponent>(FName("Point B"));
	partB->SetSimulatePhysics(true);

	physicsConstraint->SetConstrainedComponents(partA, "Member A", partB, "Member B");
	
}

// Called when the game starts or when spawned
void AEntity::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

