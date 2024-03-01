// "Unreal Pokémon" created by Retro & Chill.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Screens/Screen.h"
#include "PauseMenuScreen.generated.h"

class UCommandWindow;
/**
 * Basic Pokémon pause menu screen
 */
UCLASS(Blueprintable, Abstract)
class POKEMONUI_API UPauseMenuScreen : public UScreen {
	GENERATED_BODY()

protected:
	void NativePreConstruct() override;
	
	
private:
	/**
	 * The command window that is displayed to the player
	 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommandWindow> CommandWindow;
};
