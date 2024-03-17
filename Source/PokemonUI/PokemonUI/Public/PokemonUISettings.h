﻿// "Unreal Pokémon" created by Retro & Chill.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PokemonUISettings.generated.h"

class UTextDisplayScreen;
/**
 * Global configuration settings for the Pokémon UI classes
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "Pokémon UI Settings"))
class POKEMONUI_API UPokemonUISettings : public UDeveloperSettings {
	GENERATED_BODY()

public:
	/**
	 * Get the name of the package that contains the Pokémon Icon graphics
	 * @return The name of the package that contains the Pokémon Icon graphics
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Search Paths")
	const FString& GetPokemonIconsPackageName() const;

	/**
	 * Get the screen class use for the text screen
	 * @return The screen class use for the text screen
	 */
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Menus")
	const TArray<TSubclassOf<UTextDisplayScreen>> &GetTextScreenClasses() const;

private:
	/**
	 * The name of the package that contains the Pokémon Icon graphics
	 */
	UPROPERTY(EditDefaultsOnly, Config, BlueprintGetter=GetPokemonIconsPackageName, Category = "Search Paths")
	FString PokemonIconsPackageName;

	/**
	 * The screen class use for the text screen
	 */
	UPROPERTY(EditDefaultsOnly, Config, BlueprintGetter=GetTextScreenClasses, Category = "Menus")
	TArray<TSubclassOf<UTextDisplayScreen>> TextScreenClasses;
};
