// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HingePair.generated.h"

#define MAX_ANGLE_HINGE			160.		// Когда шарнир развёрнут на максимум
#define ANGLE_ONESIDE_OFFSET	10.
#define MAX_VELOCITY			10000.

UCLASS()
class AITOOLS_API AHingePair : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHingePair();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	USceneComponent* root = nullptr;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	UArrowComponent* arrow = nullptr;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	UStaticMeshComponent* partA = nullptr;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	UStaticMeshComponent* partB = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UPhysicsConstraintComponent* physicsConstraint = nullptr;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	USceneComponent* pointA = nullptr;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	USceneComponent* pointB = nullptr;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FVector getVForceDecrease();
	FVector getVForceIncrease();
	

	enum STATE { STATE_FORWARD, STATE_BACKWARD } state = STATE_FORWARD;
	//void getResult(float &target, Mat &mat);
	float getTargetResponse();
	float getAngle();
	void printTransform();
	void payload();
	int getState();
	void setIndex(int idx);
	int getMaxVelocity();
	int getMaxHingeAngle();
	FVector getVelocityPartB();
	float getKoeff();
	float getNeedAngle();
	int getCounterFrames();
	void getParameters(float &cur_angle, float &need_angle, FVector &_velocity, float &_koef, float &_target_response, FVector &_up_vector);

private:
	void initPhysicsConstraints();
	void startPhysicsConstraints();

	struct HingeInfo {
		enum ETYPE {SHRINK, EXPAND};
		HingeInfo(ETYPE type = ETYPE::EXPAND, AHingePair* _parent = nullptr) : parent(_parent), etype(type) {
			if (etype == SHRINK)
				koeff = 1.0;
			else
				koeff = -1.0;
			force = 100.0;
		}
		ETYPE etype;
		float force;
		float koeff;
		float angle;
		static float last_angle;
		static float target_angle;
		FString getType() {
			if (etype == SHRINK)
				return FString("SHRINK");
			return FString("EXPAND");
		}
		float getAngleLeft() {
			if (etype == SHRINK)
				return parent->getAngle() - target_angle;
			return target_angle - parent->getAngle();
		}
		void prop(float _angle) {
			angle = _angle;
			
		}
		void setAngle(float _angle) {
			angle = _angle;
		}
		FVector getForce() {
			FVector force_vector = etype == SHRINK ? parent->getVForceDecrease() : parent->getVForceIncrease();
			force_vector *= force * koeff;
			return force_vector;
		}
	private:
		AHingePair* parent = nullptr;
	};
	HingeInfo shrink, expand;

	APlayerController* controller = nullptr;
	bool complate;
	int index = -1;
	float koeff = -1;
	FVector velocity;
	FVector groundForce;
	float last_delta_turn = MAX_ANGLE_HINGE;
	float save_need_angle = 0;
	float last_angle = -100.0f;
	float target_response;
	int test = 0;
	float last_force;
	float last_sign;
	int counter_frames = 0;
	bool launch = false;
	float delta_second = 0;
	//float koeef_need_turn = 0.01;


};
