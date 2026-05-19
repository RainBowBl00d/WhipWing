#include "Game/WhipWingGameMode.h"
#include "Player/WhipWingPawn.h"

AWhipWingGameMode::AWhipWingGameMode()
{
	DefaultPawnClass = AWhipWingPawn::StaticClass();
}
