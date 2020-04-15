#ifndef BTN_TYPE_TRAITS_H
#define BTN_TYPE_TRAITS_H

#include <type_traits>
#include "btn_common.h"

namespace btn
{
    using std::remove_reference;
    using std::remove_reference_t;

    using std::is_same;
    using std::is_same_v;

    using std::is_trivial;
    using std::is_trivial_v;

    using std::is_trivially_copyable;
    using std::is_trivially_copyable_v;

    using std::is_base_of;
    using std::is_base_of_v;

    using std::decay;
    using std::decay_t;
}

#endif
