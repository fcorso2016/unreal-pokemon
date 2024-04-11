// "Unreal Pokémon" created by Retro & Chill.
#pragma once

#include "BattleTerrain.generated.h"
#include "CoreMinimal.h"
#include "UObject/Object.h"

/**
 * In-battle terrain effects caused by moves like Electric Terrain.
 */
USTRUCT(BlueprintType, meta = (DatabaseType = "Hardcoded"))
struct POKEMONDATA_API FBattleTerrain : public FTableRowBase {
    GENERATED_BODY()

    /**
     * The internal ID used for lookup by the game
     */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Basic")
    FName ID;

    /**
     * The name that is displayed to the player
     */
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Basic")
    FText RealName;
};
