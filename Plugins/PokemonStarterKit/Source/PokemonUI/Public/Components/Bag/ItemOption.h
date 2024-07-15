﻿// "Unreal Pokémon" created by Retro & Chill.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"

#include "ItemOption.generated.h"

struct FItem;
struct FItemSlot;
class UDisplayText;

/**
 * An option that represents and item in the player's inventory.
 */
UCLASS(Abstract, Blueprintable)
class POKEMONUI_API UItemOption : public UCommonButtonBase {
    GENERATED_BODY()

  public:
    /**
     * Get the underlying item held by this widget
     * @return The item held by this widget.
     */
    UFUNCTION(BlueprintPure, Category = Inventory)
    const FItem &GetItem() const;

    /**
     * Get the quantity of the item in question
     * @return The quantity held
     */
    UFUNCTION(BlueprintPure, Category = Inventory)
    int32 GetQuantity() const;

    /**
     * Set the item to the one held within the given slot
     * @param Item The ID of the item in question
     * @param Quantity The quantity of the item
     */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Display)
    void SetItem(FName Item, int32 Quantity);

protected:
    void NativeOnCurrentTextStyleChanged() override;

  private:
    /**
     * Represents the current item held by this option
     */
    FName CurrentItem;

    /**
     * Represents the quantity held
     */
    int32 QuantityHeld;

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
