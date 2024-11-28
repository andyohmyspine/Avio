#pragma once

#include "core.hpp"

#ifndef RHI_FUNC_PTR
#define RHI_FUNC_PTR(name, signature)      \
  using AV_PASTE2(PFN_, name) = signature; \
  inline AV_PASTE2(PFN_, name) name;
#endif

namespace avio {}