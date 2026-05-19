#include "Player/WhipWingPawn.h"
#include "Player/FlightMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "Components/SphereComponent.h"

AWhipWingPawn::AWhipWingPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(34.f);
	CollisionSphere->SetCollisionProfileName(TEXT("Pawn"));
	SetRootComponent(CollisionSphere);

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	VROrigin->SetupAttachment(CollisionSphere);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VROrigin);

	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	LeftController->SetupAttachment(VROrigin);
	LeftController->MotionSource = FName("Left");

	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	RightController->SetupAttachment(VROrigin);
	RightController->MotionSource = FName("Right");

	FlightMovement = CreateDefaultSubobject<UFlightMovementComponent>(TEXT("FlightMovement"));
}

void AWhipWingPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AWhipWingPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FlightMovement->UpdateControllerTransforms(
		LeftController->GetRelativeTransform(),
		RightController->GetRelativeTransform()
	);
}
