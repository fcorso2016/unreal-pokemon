﻿// "Unreal Pokémon" created by Retro & Chill.

#include "Utilities/GridBasedCharacterUtilities.h"
#include "CharacterMovementComponentAsync.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GridBased2DSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MathUtilities.h"
#include <range/v3/functional/bind_back.hpp>

TSet<FName> UGridBasedCharacterUtilities::CollectComponentTagsForCurrentTile(ACharacter *Character) {
    static const auto GridSize = static_cast<float>(GetDefault<UGridBased2DSettings>()->GetGridSize());
    static TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = {UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
                                                                UEngineTypes::ConvertToObjectType(ECC_WorldDynamic)};

    TSet<FName> Tags;

    TArray<UPrimitiveComponent *> Components;
    UKismetSystemLibrary::SphereOverlapComponents(Character, Character->GetActorLocation() + FVector(0, 0, 2),
                                                  GridSize / 2, ObjectTypes, nullptr, {Character}, Components);
    for (auto Comp : Components) {
        Tags.Append(Comp->ComponentTags);
    }
    return Tags;
}

bool UGridBasedCharacterUtilities::InvalidFloor(ACharacter *Character, const FVector &TargetSquare,
                                                const UPrimitiveComponent *HitComponent) {
    if (HitComponent != nullptr && !CanStepUpOnComponent(Character, *HitComponent)) {
        return true;
    }

    auto Edge = FindLocationJustOffTileEdge(Character, TargetSquare);
    FFindFloorResult Result;
    Character->GetCharacterMovement()->FindFloor(Edge, Result, true);
    return !Result.bWalkableFloor;
}

bool UGridBasedCharacterUtilities::IsStandingNextToCliff(ACharacter *Character, const FVector &TargetSquare) {
    auto [Distance1, Component1] =
        PerformTraceToGround(Character, FindLocationJustOffTileEdge(Character, TargetSquare));
    auto [Distance2, Component2] =
        PerformTraceToGround(Character, FindLocationJustBeforeTileEdge(Character, TargetSquare));

    if (FMath::Abs(Distance1 - Distance2) > Character->GetCharacterMovement()->MaxStepHeight) {
        return true;
    }

    if (Component2 != nullptr) {
        return !CanStepUpOnComponent(Character, *Component2);
    }

    return false;
}

bool UGridBasedCharacterUtilities::CanStepUpOnComponent(ACharacter *Character, const UPrimitiveComponent &Component) {
    if (!Component.CanCharacterStepUp(Character)) {
        return false;
    }

    if (auto StaticMeshComponent = Cast<UStaticMeshComponent>(&Component); StaticMeshComponent != nullptr) {
        return StaticMeshComponent->GetWalkableSlopeOverride().WalkableSlopeBehavior != WalkableSlope_Unwalkable;
    }

    return true;
}

FVector UGridBasedCharacterUtilities::FindLocationJustOffTileEdge(ACharacter *Character, const FVector &TargetSquare) {
    auto Location = Character->GetActorLocation();
    auto MidPoint = UMathUtilities::Midpoint(TargetSquare, Location);
    auto Diff = TargetSquare - Location;
    Diff.Normalize();
    return MidPoint + Diff;
}

FVector UGridBasedCharacterUtilities::FindLocationJustBeforeTileEdge(ACharacter *Character,
                                                                     const FVector &TargetSquare) {
    auto Location = Character->GetActorLocation();
    auto MidPoint = UMathUtilities::Midpoint(TargetSquare, Location);
    auto Diff = TargetSquare - Location;
    Diff.Normalize();
    return MidPoint - Diff;
}

TPair<double, UPrimitiveComponent *> UGridBasedCharacterUtilities::PerformTraceToGround(ACharacter *Character,
                                                                                        const FVector &Position) {
    static constexpr double TraceMax = 100.0;
    FHitResult Result;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Character);
    auto Hit = Character->GetWorld()->LineTraceSingleByChannel(Result, Position, Position - FVector(0, 0, TraceMax),
                                                               ECC_Visibility, Params);
    return {Hit ? Result.Distance : TraceMax, Result.Component.Get()};
}