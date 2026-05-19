#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "WhipWingPawn.generated.h"

class USphereComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UFlightMovementComponent;

UCLASS()
class WHIPWING_API AWhipWingPawn : public APawn
{
	GENERATED_BODY()

public:
	AWhipWingPawn();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UFlightMovementComponent* FlightMovement;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* VROrigin;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UMotionControllerComponent* LeftController;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UMotionControllerComponent* RightController;
};
