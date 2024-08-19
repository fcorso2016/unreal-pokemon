﻿// "Unreal Pokémon" created by Retro & Chill.

#pragma once

#include "CoreMinimal.h"
#include "Ranges/Functional/Bindings.h"
#include <range/v3/view/filter.hpp>

namespace UE::Ranges {


    struct FFilter {
        template <typename... A>
        FORCEINLINE constexpr auto operator()(A&&... Args) const {
            return ranges::views::filter(CreateBinding<A...>(Forward<A>(Args)...));
        }
    };

    /**
     * Equivalent to ranges::views::filter in the STL/range-v3, this version differs in that there are several things
     * you can supply to it in order to create a valid binding.
     */
    constexpr FFilter Filter;
    
}
