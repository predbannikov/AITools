// Fill out your copyright notice in the Description page of Project Settings.


#include "HingeToPair.h"

// Sets default values
AHingeToPair::AHingeToPair()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//partA = CreateDefaultSubobject<UStaticMeshComponent>(FName("Part A"));
	//SetRootComponent(partA);

	//arrow = CreateDefaultSubobject<UArrowComponent>(FName("Arrow"));
	////arrow->AttachToComponent(RootComponent);


	//partB = CreateDefaultSubobject<UStaticMeshComponent>(FName("Part B"));
	//
	//physicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("Physics Component"));
	//pointA = CreateDefaultSubobject<USceneComponent>(FName("Point A"));
	//pointB = CreateDefaultSubobject<USceneComponent>(FName("Point B"));


}

// Called when the game starts or when spawned
void AHingeToPair::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHingeToPair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

