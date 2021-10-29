// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity.h"

// Sets default values
AEntity::AEntity()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	physicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("Physics Component"));
	physicsConstraint->SetupAttachment(RootComponent);

	arrow = CreateDefaultSubobject<UArrowComponent>(FName("Arrow"));
	arrow->AttachToComponent(physicsConstraint, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));

	partA = CreateDefaultSubobject<UStaticMeshComponent>(FName("Part A"));
	partA->AttachToComponent(physicsConstraint, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));


	partB = CreateDefaultSubobject<UStaticMeshComponent>(FName("Part B"));
	partB->AttachToComponent(physicsConstraint, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));


	pointA = CreateDefaultSubobject<USceneComponent>(FName("Point A"));
	pointA->AttachToComponent(physicsConstraint, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
	pointB = CreateDefaultSubobject<USceneComponent>(FName("Point B"));
	pointB->AttachToComponent(physicsConstraint, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));

	
}

// Called when the game starts or when spawned
void AEntity::BeginPlay()
{
	Super::BeginPlay();
	physicsConstraint->SetConstrainedComponents(partA, "Member A", partB, "Member B");
	partB->SetSimulatePhysics(true);
	
}

// Called every frame
void AEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

