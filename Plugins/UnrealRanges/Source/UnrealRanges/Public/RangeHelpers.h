﻿// "Unreal Pokémon" created by Retro & Chill.

#pragma once

#include "CoreMinimal.h"
#include "range/v3/view/span.hpp"
#include "Ranges/RangeConcepts.h"
#include "Ranges/Views/ContainerView.h"
#include <range/v3/view/filter.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/cache1.hpp>

namespace RangeHelpers {

/**
 * Convert the elements in the range into an array
 * @tparam T The type of data the array will hold
 * @tparam RangeType The input range type
 * @param Range The range to process
 * @return The range values as an array
 */
template <typename T, typename RangeType>
TArray<T> ToArray(RangeType &&Range) {
    TArray<T> Ret;
    
    if constexpr (ranges::sized_range<T>) {
        Ret.Reserve(ranges::size(Range));
    }

    if constexpr (std::movable<T>) {
        for (auto &&Value : Range) {
            Ret.Emplace(MoveTempIfPossible(Value));
        }
    } else {
        for (const auto &Value : Range) {
            Ret.Emplace(Value);
        }
    }
    
    return Ret;
}

/**
 * Converter object used to transform a range into an array.
 * @tparam T The type of data the array will hold
 */
template <typename T>
struct TToArray {

    /**
     * Operator overload so that converting to an Unreal data type can be chained into the ranges stream operator
     * @tparam T The type of data the array will hold
     * @tparam RangeType The input range type
     * @param Range The range to process
     * @return The range values as an array
     */
    template <typename RangeType>
    friend TArray<T> operator|(RangeType &&Range, TToArray) {
        return ToArray<T, RangeType>(Forward<RangeType>(Range));
    }
};

} // namespace RangeHelpers