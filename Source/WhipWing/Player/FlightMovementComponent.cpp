#include "Player/FlightMovementComponent.h"
#include "Engine/Engine.h"

UFlightMovementComponent::UFlightMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFlightMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	ForwardYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentSpeed = BaseSpeed;
}

void UFlightMovementComponent::UpdateControllerTransforms(const FTransform& Left, const FTransform& Right)
{
	CachedLeftTransform = Left;
	CachedRightTransform = Right;
	bHasControllerData = true;
}

void UFlightMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!bIsFlightEnabled)
	{
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		return;
	}

	if (!bHasControllerData)
		return;

	float TargetPitch = CalculateTargetPitch();
	float TargetRoll = CalculateTargetRoll();

	CurrentPitch = FMath::FInterpTo(CurrentPitch, TargetPitch, DeltaTime, SteeringSmoothingSpeed);
	CurrentRoll = FMath::FInterpTo(CurrentRoll, TargetRoll, DeltaTime, SteeringSmoothingSpeed);

	if (DetectFlap(DeltaTime))
	{
		VerticalVelocity += FlapLiftImpulse;

		if (bShowDebug && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(10, 0.5f, FColor::Yellow, TEXT(">>> FLAP <<<"));
		}
	}

	VerticalVelocity -= Gravity * DeltaTime;
	VerticalVelocity = FMath::Clamp(VerticalVelocity, -MaxFallVelocity, MaxLiftVelocity);

	FlapCooldownTimer = FMath::Max(0.f, FlapCooldownTimer - DeltaTime);

	PrevLeftPos = CachedLeftTransform.GetLocation();
	PrevRightPos = CachedRightTransform.GetLocation();
	bHasPreviousPositions = true;

	float PitchRad = FMath::DegreesToRadians(CurrentPitch);
	CurrentSpeed -= Gravity * FMath::Sin(PitchRad) * GlideSpeedTransfer * DeltaTime;
	CurrentSpeed = FMath::Clamp(CurrentSpeed, MinSpeed, MaxSpeed);

	FVector ForwardDir = FRotationMatrix(FRotator(CurrentPitch, ForwardYaw, 0.f)).GetUnitAxis(EAxis::X);
	FVector RightDir = FRotationMatrix(FRotator(0.f, ForwardYaw, 0.f)).GetUnitAxis(EAxis::Y);

	FVector Movement = ForwardDir * CurrentSpeed * DeltaTime;
	Movement += RightDir * -CurrentRoll * LateralStrafeSpeed * DeltaTime;
	Movement.Z += VerticalVelocity * DeltaTime;

	FHitResult HitResult;
	GetOwner()->AddActorWorldOffset(Movement, true, &HitResult);

	if (HitResult.bBlockingHit)
	{
		if (HitResult.Normal.Z > 0.5f && VerticalVelocity < 0.f)
		{
			VerticalVelocity = 0.f;
		}
		else if (HitResult.Normal.Z < -0.5f && VerticalVelocity > 0.f)
		{
			VerticalVelocity = 0.f;
		}
	}

	if (bShowDebug)
	{
		DrawDebugVisualization();
	}
}

float UFlightMovementComponent::CalculateTargetPitch() const
{
	FVector LeftForward = CachedLeftTransform.GetRotation().GetForwardVector();
	FVector RightForward = CachedRightTransform.GetRotation().GetForwardVector();

	float LeftPitch = FMath::RadiansToDegrees(FMath::Asin(LeftForward.Z));
	float RightPitch = FMath::RadiansToDegrees(FMath::Asin(RightForward.Z));

	float AveragePitch = (LeftPitch + RightPitch) * 0.5f;
	return FMath::Clamp(AveragePitch * PitchSensitivity, -MaxPitchAngle, MaxPitchAngle);
}

float UFlightMovementComponent::CalculateTargetRoll() const
{
	float HeightDiff = CachedRightTransform.GetLocation().Z - CachedLeftTransform.GetLocation().Z;
	return FMath::Clamp(HeightDiff * RollSensitivity, -MaxRollAngle, MaxRollAngle);
}

bool UFlightMovementComponent::DetectFlap(float DeltaTime)
{
	if (!bHasPreviousPositions)
		return false;

	if (FlapCooldownTimer > 0.f)
		return false;

	FVector LeftPos = CachedLeftTransform.GetLocation();
	FVector RightPos = CachedRightTransform.GetLocation();

	float LeftVertVel = (LeftPos.Z - PrevLeftPos.Z) / DeltaTime;
	float RightVertVel = (RightPos.Z - PrevRightPos.Z) / DeltaTime;
	float AvgVertVel = (LeftVertVel + RightVertVel) * 0.5f;

	bool bIsMovingDown = AvgVertVel < -FlapDetectionThreshold;

	if (bWasMovingDown && !bIsMovingDown)
	{
		bWasMovingDown = false;
		FlapCooldownTimer = FlapCooldown;
		return true;
	}

	bWasMovingDown = bIsMovingDown;
	return false;
}

void UFlightMovementComponent::SetForwardDirection(float NewYaw)
{
	ForwardYaw = NewYaw;
}

void UFlightMovementComponent::DrawDebugVisualization() const
{
	if (!GEngine)
		return;

	float HeightDiff = CachedRightTransform.GetLocation().Z - CachedLeftTransform.GetLocation().Z;

	GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Cyan,
		FString::Printf(TEXT("Roll: %+.1f  (height diff: %+.1f)"), CurrentRoll, HeightDiff));
	GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::Cyan,
		FString::Printf(TEXT("Pitch: %+.1f"), CurrentPitch));
	GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Cyan,
		FString::Printf(TEXT("Forward Yaw: %.1f (fixed)"), ForwardYaw));
	GEngine->AddOnScreenDebugMessage(11, 0.f, FColor::Cyan,
		FString::Printf(TEXT("Current Speed: %.1f"), CurrentSpeed));
	GEngine->AddOnScreenDebugMessage(9, 0.f, FColor::Cyan,
		FString::Printf(TEXT("Lateral: %+.1f"), -CurrentRoll * LateralStrafeSpeed));
	GEngine->AddOnScreenDebugMessage(4, 0.f, VerticalVelocity >= 0.f ? FColor::Yellow : FColor::Red,
		FString::Printf(TEXT("Vert Vel: %+.1f"), VerticalVelocity));
	GEngine->AddOnScreenDebugMessage(5, 0.f, FColor::Green,
		FString::Printf(TEXT("Position: %s"), *GetOwner()->GetActorLocation().ToString()));
	GEngine->AddOnScreenDebugMessage(6, 0.f, FColor::White,
		FString::Printf(TEXT("Grounded: %s"), VerticalVelocity == 0.f && FlapCooldownTimer <= 0.f ? TEXT("YES") : TEXT("no")));

	FVector LP = CachedLeftTransform.GetLocation();
	FVector RP = CachedRightTransform.GetLocation();
	GEngine->AddOnScreenDebugMessage(7, 0.f, FColor::Orange,
		FString::Printf(TEXT("Left ctrl:  X=%+.0f Y=%+.0f Z=%+.0f"), LP.X, LP.Y, LP.Z));
	GEngine->AddOnScreenDebugMessage(8, 0.f, FColor::Orange,
		FString::Printf(TEXT("Right ctrl: X=%+.0f Y=%+.0f Z=%+.0f"), RP.X, RP.Y, RP.Z));
}
float UFlightMovementComponent::GetCurrentSpeed() const
{
	return CurrentSpeed;
}

float UFlightMovementComponent::GetCurrentPitch() const
{
	return CurrentPitch;
}

void UFlightMovementComponent::SetFlightEnabled(bool bEnable)
{
	bIsFlightEnabled = bEnable;

	// Kui liikumine lülitatakse välja, nullime igaks juhuks ka kiirused, 
	// et tegelane ei jääks viimase kiirusega lõputult triivima
	if (!bEnable)
	{
		CurrentSpeed = 0.f;
		VerticalVelocity = 0.f;
		bHasPreviousPositions = false;
	}
}