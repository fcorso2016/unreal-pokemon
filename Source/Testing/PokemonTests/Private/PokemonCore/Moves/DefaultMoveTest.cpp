﻿// "Unreal Pokémon" created by Retro & Chill.
#include "Moves/DefaultMove.h"
#include "CommonDefines.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(DefaultMoveTest, "UnrealPokemon.Core.Moves.DefaultMoveTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool DefaultMoveTest::RunTest(const FString &Parameters) {
    auto Move = NewObject<UDefaultMove>()->Initialize(TEXT("SWORDSDANCE"));

    AUTOMATION_ASSERT(TestEqual(TEXT(""), Move->GetCurrentPP(), 20));
    AUTOMATION_ASSERT(TestEqual(TEXT(""), Move->GetTotalPP(), 20));

    return true;
}
