﻿#if WITH_TESTS && HAS_AUTOMATION_HELPERS
#include "Pokemon/TrainerMemo/DefaultObtainedBlock.h"
#include "Asserts.h"
#include "Misc/AutomationTest.h"
#include "Pokemon/PokemonDTO.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(DefaultObtainedBlockTest, "Tests.Pokemon.TrainerMemo.DefaultObtainedBlockTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool DefaultObtainedBlockTest::RunTest(const FString &Parameters) {
    FPokemonDTO Blank;
    auto BlankBlock = NewObject<UDefaultObtainedBlock>()->Initialize(Blank);
    CHECK_EQUAL(5, BlankBlock->GetLevelMet());
    CHECK_EQUAL(EObtainMethod::Default, BlankBlock->GetObtainMethod());
    ASSERT_TRUE(BlankBlock->GetObtainText().IsSet());
    CHECK_EQUAL(TEXT(""), BlankBlock->GetObtainText().GetValue().ToString());

    FPokemonDTO DTO = {.Level = 10,
                       .ObtainMethod = EObtainMethod::FatefulEncounter,
                       .MetLocation = FText::FromStringView(TEXT("TestLocation"))};
    auto ObtainedBlock = NewObject<UDefaultObtainedBlock>()->Initialize(DTO);
    CHECK_EQUAL(10, ObtainedBlock->GetLevelMet());
    CHECK_EQUAL(EObtainMethod::FatefulEncounter, ObtainedBlock->GetObtainMethod());
    ASSERT_TRUE(ObtainedBlock->GetObtainText().IsSet());
    CHECK_EQUAL(TEXT("TestLocation"), ObtainedBlock->GetObtainText().GetValue().ToString());
    CHECK_NOT_NULL(ObtainedBlock->GetTimeReceived());
    CHECK_FALSE(ObtainedBlock->GetHatchedMap().IsSet());
    CHECK_NULL(ObtainedBlock->GetTimeHatched());

    return true;
}
#endif