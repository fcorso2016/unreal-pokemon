﻿// "Unreal Pokémon" created by Retro & Chill.

#pragma once

#include "CoreMinimal.h"
#include "ObtainMethod.h"
#include "UObject/Interface.h"

#include "ObtainedBlock.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, NotBlueprintable)
class UObtainedBlock : public UInterface {
    GENERATED_BODY()
};

/**
 * Interface for all information how a Pokémon was obtained.
 */
class POKEMONCORE_API IObtainedBlock {
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
  public:
    /**
     * Get the method this Pokémon was obtained by.
     * @return The means the Pokémon was obtained by.
     */
    UFUNCTION(BlueprintCallable, Category = "Trainer Memo")
    virtual EObtainMethod GetObtainMethod() const = 0;

    /**
     * Get the time this Pokémon was received
     * @return When was this Pokémon received
     */
    UFUNCTION(BlueprintCallable, Category = "Trainer Memo")
    virtual FDateTime GetTimeReceived() const = 0;

    /**
     * Get the location or other means of obtaining that this Pokémon has.
     * @return How this Pokémon was obtained
     */
    virtual const TOptional<FText> &GetObtainText() = 0;

    /**
     * Get the time this Pokémon hatched
     * @return The means the Pokémon was obtained by.
     */
    virtual const TOptional<FDateTime> &GetTimeHatched() const = 0;

    /**
     * Get the map this Pokémon hatched on.
     * @return The location of hatching.
     */
    virtual const TOptional<FText> &GetHatchedMap() = 0;
};