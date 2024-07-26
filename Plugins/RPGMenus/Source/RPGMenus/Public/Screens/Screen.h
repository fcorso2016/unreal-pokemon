// "Unreal Pokémon" created by Retro & Chill.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "NativeGameplayTags.h"

#include "Screen.generated.h"

class USelectableWidget;
struct FInputActionInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScreenClosed);

namespace RPG::Menus {

/**
 * The native tag assigned the primary layer for display
 */
RPGMENUS_API const UE_DECLARE_GAMEPLAY_TAG_EXTERN(PrimaryMenuLayerTag);

/**
 * The native tag assigned to any overlay layers layer for display
 */
RPGMENUS_API const UE_DECLARE_GAMEPLAY_TAG_EXTERN(OverlayMenuLayerTag);

} // namespace RPG::Menus

UENUM(BlueprintType)
enum class ERPGWidgetInputMode : uint8
{
    Default,
    GameAndMenu,
    Game,
    Menu
};

/**
 * Represents a basic screen used by the UI. They tend to be added in a stack format, and are displayed one on top
 * of the other.
 */
UCLASS(Blueprintable, BlueprintType, Abstract)
class RPGMENUS_API UScreen : public UCommonActivatableWidget {
    GENERATED_BODY()

  protected:
    void NativeConstruct() override;

  public:
    /**
     * Refresh the display of this screen to the player
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Display)
    void RefreshSelf();

    TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

    /**
     * Close the screen and return to the previous one
     */
    UFUNCTION(BlueprintCallable, Category = Navigation)
    void CloseScreen();

    /**
     * Callback to bind additional functionality to when CloseScreen() gets called
     * @return Callback for when the screen is closed
     */
    FOnScreenClosed &GetOnScreenClosed();

  protected:
    void NativeOnActivated() override;
    void NativeOnDeactivated() override;

  private:
    /**
     * Callback for when the screen is closed
     */
    UPROPERTY(BlueprintAssignable)
    FOnScreenClosed OnScreenClosed;

    /**
     * The desired input mode to use while this UI is activated, for example do you want key presses to still reach
     * the game/player controller?
     */
    UPROPERTY(EditDefaultsOnly, Category = Input)
    ERPGWidgetInputMode InputConfig = ERPGWidgetInputMode::Default;

    /**
     * The desired mouse behavior when the game gets input.

     */
    UPROPERTY(EditDefaultsOnly, Category = Input)
    EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
