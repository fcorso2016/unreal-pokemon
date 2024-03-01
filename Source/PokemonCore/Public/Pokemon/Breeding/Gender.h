﻿// "Unreal Pokémon" created by Retro & Chill.
#pragma once

#include "CoreMinimal.h"

/**
 * Enumeration for handling a Pokémon's gender. Used for things like breeding and determining the icon to display
 * in the UI.
 */
UENUM(BlueprintType)
enum class EGender {
	/**
	 * The Pokémon is male
	 */
	Male,

	/**
	 * The Pokémon is female
	 */
	Female,

	/**
	 * The Pokémon is genderless/gender-unknown
	 */
	Genderless,
};
