﻿// "Unreal Pokémon" created by Retro & Chill.

#include "Asserts.h"
#include "Data/Windowskin.h"
#include "Misc/AutomationTest.h"
#include "Windowskin/WindowskinAssetActions.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestWindowskinAssetActions, "Unit Tests.RPGMenus.TestWindowskinAssetActions",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestWindowskinAssetActions::RunTest(const FString &Parameters) {
    FWindowskinAssetActions AssetActions;

    UE_CHECK_EQUAL(TEXT("Windowskin"), AssetActions.GetName().ToString());
    UE_CHECK_EQUAL(UWindowskin::StaticClass(), AssetActions.GetSupportedClass());
    UE_CHECK_EQUAL(FColor::Silver, AssetActions.GetTypeColor());
    UE_CHECK_EQUAL(EAssetTypeCategories::UI, AssetActions.GetCategories());

    return true;
}