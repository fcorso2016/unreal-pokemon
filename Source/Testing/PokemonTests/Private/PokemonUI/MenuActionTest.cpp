﻿// "Unreal Pokémon" created by Retro & Chill.
#include "PokemonUI/MenuActionTest.h"
#include "Actions/MenuAction.h"
#include "CommonDefines.h"
#include "RPGPlayerController.h"

void AMenuActionTest::BeginPlay() {
    Super::BeginPlay();
    OnTestStart.AddDynamic(this, &AMenuActionTest::TestRun);
}

void AMenuActionTest::TestRun() {
    TEST_ASSERT(AssertIsValid(MenuAction, TEXT("Screen class should be valid!")))

    auto PlayerController = Cast<ARPGPlayerController>(GetWorld()->GetFirstPlayerController());
    TEST_ASSERT(
        AssertIsValid(PlayerController, TEXT("The player should be is valid and of class RPG Player Controller!")))
    MenuAction->PerformAction(PlayerController->GetPawn());

    auto TopOfStack = PlayerController->GetTopOfStack();
    TEST_ASSERT(AssertIsValid(TopOfStack, TEXT("There should be a valid screen on the stack!")))

    FinishTest(EFunctionalTestResult::Succeeded, TEXT("Test passed!"));
}
