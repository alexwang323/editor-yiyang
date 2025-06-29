#pragma once
#include <sol/sol.hpp>
#include "ginkgopch.h"

struct GlmBindings {
	static void CreateGlmBindings(std::shared_ptr<sol::state> lua);
};

