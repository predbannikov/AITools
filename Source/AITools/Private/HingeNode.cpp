// Fill out your copyright notice in the Description page of Project Settings.


#include "HingeNode.h"

UHingeNode::UHingeNode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	//partB = CreateDefaultSubobject<UStaticMeshComponent>(FName("Part A"));

	arrow = CreateDefaultSubobject<UArrowComponent>(FName("Arrow"));
	

	partB = CreateDefaultSubobject<UStaticMeshComponent>(FName("Part B"));

	//physicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("Physics Component"));
	//pointA = CreateDefaultSubobject<USceneComponent>(FName("Point A"));
	//pointB = CreateDefaultSubobject<USceneComponent>(FName("Point B"));

}
