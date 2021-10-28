// Fill out your copyright notice in the Description page of Project Settings.


#include "Leg.h"

ULeg::ULeg() 
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	//arrow = CreateDefaultSubobject<UArrowComponent>(FName("Arrow"));
	//USceneComponent *root;
	//RootComponent = partB;
	//this->GetParentComponents(root);

	partB = CreateDefaultSubobject<UStaticMeshComponent>(FName("Part B"));
	
	partB->SetupAttachment(this);
	//partB->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	//physicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("Physics Component"));
	//pointA = CreateDefaultSubobject<USceneComponent>(FName("Point A"));
	//pointB = CreateDefaultSubobject<USceneComponent>(FName("Point B"));
}