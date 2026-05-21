#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlightMovementComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WHIPWING_API UFlightMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFlightMovementComponent();

	void UpdateControllerTransforms(const FTransform& Left, const FTransform& Right);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Speed")
	float BaseSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Speed")
	float MinSpeed = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Speed")
	float MaxSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Speed")
	float GlideSpeedTransfer = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Gravity")
	float Gravity = 980.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Flap")
	float FlapLiftImpulse = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Flap")
	float MaxLiftVelocity = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Flap")
	float MaxFallVelocity = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Flap")
	float FlapCooldown = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Flap")
	float FlapDetectionThreshold = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Steering")
	float PitchSensitivity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Steering")
	float RollSensitivity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Steering")
	float MaxPitchAngle = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Steering")
	float MaxRollAngle = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Steering")
	float SteeringSmoothingSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Steering")
	float LateralStrafeSpeed = 400.f;

	void SetForwardDirection(float NewYaw);
	float GetForwardYaw() const { return ForwardYaw; }

	UFUNCTION(BlueprintCallable, Category = "Flight|Data")
	float GetCurrentSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "Flight|Data")
	float GetCurrentPitch() const;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Debug")
	bool bShowDebug = false;

	UFUNCTION(BlueprintCallable, Category = "Flight|Control")
	void SetFlightEnabled(bool bEnable);

	// Funktsioon, et kontrollida, kas lendamine on hetkel lubatud
	UFUNCTION(BlueprintPure, Category = "Flight|Control")
	bool IsFlightEnabled() const { return bIsFlightEnabled; }

protected:
	virtual void BeginPlay() override;

private:
	float CurrentSpeed = 0.f;
	float VerticalVelocity = 0.f;
	float CurrentPitch = 0.f;
	float CurrentRoll = 0.f;
	float ForwardYaw = 0.f;

	FVector PrevLeftPos = FVector::ZeroVector;
	FVector PrevRightPos = FVector::ZeroVector;
	bool bHasPreviousPositions = false;

	float FlapCooldownTimer = 0.f;
	bool bWasMovingDown = false;

	FTransform CachedLeftTransform;
	FTransform CachedRightTransform;
	bool bHasControllerData = false;

	float CalculateTargetPitch() const;
	float CalculateTargetRoll() const;
	bool DetectFlap(float DeltaTime);
	void DrawDebugVisualization() const;

	bool bIsFlightEnabled = true;
};
