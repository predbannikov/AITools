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

class AHingePair;

#define MEM_HINGE_INFO	5 

struct HingeInfo {
	enum ETYPE { SHRINK, EXPAND };
	HingeInfo();
	ETYPE etype;
	ETYPE last_etype;
	float force = 100.0f;
	float target_angle;
	float angle;
	float delta_turn_angle;
	float delta_k_shrink;
	float delta_k_expand;
	float k_mult = 1;
	float k_force = 1;
	float k_shrinks[MEM_HINGE_INFO];
	float k_shrink;
	float k_expand;
	float k_expands[MEM_HINGE_INFO];
	float angles[MEM_HINGE_INFO];
	float delta_turn_angles[MEM_HINGE_INFO];
	int signs_delta_turn[MEM_HINGE_INFO];
	int sign_delta;
	float error = 1.0;
	void incKoeff(float arg);
	void decrKoeff(float arg);
	float getDeltaKoeff();
	FString getType();
	void setTargetAngle(float target);
	float getDeltaTurn();
	float getLeftAngleTurn();
	float updateKoeffs(float _k);
	void tick();
	FVector getForce();
	float getKoeff();
	void setHingePair(AHingePair* parent);
	void updateMem();
	void begin();
	void end();
	void printMem();
	int count_ticks = 0;
private:
	AHingePair* parent = nullptr;
};

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


	HingeInfo hinge;

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

	bool lock = false;
};
