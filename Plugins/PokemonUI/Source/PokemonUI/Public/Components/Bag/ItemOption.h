﻿// "Unreal Pokémon" created by Retro & Chill.

#pragma once

#include "CoreMinimal.h"
#include "Primatives/SelectableOption.h"

#include "ItemOption.generated.h"

struct FItem;
struct FItemSlot;
class UDisplayText;

/**
 * An option that represents and item in the player's inventory.
 */
UCLASS(Abstract, Blueprintable)
class POKEMONUI_API UItemOption : public USelectableOption {
    GENERATED_BODY()

  public:
    /**
     * Get the underlying item held by this widget
     * @return The item held by this widget.
     */
    UFUNCTION(BlueprintPure, Category = Inventory)
    const FItem &GetItem() const;

    /**
     * Set the item to the one held within the given slot
     * @param Item The ID of the item in question
     * @param Quantity The quantity of the item
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Display)
    void SetItem(FName Item, int32 Quantity);

  private:
    /**
     * Represents the current item held by this option
     */
    FName CurrentItem;

    /**
     * Text that displays an item's name
     */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UDisplayText> ItemNameText;

    /**
     * Text that displays the item's quantity.
     */
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UDisplayText> ItemQuantityText;
};
