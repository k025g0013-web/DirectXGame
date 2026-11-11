#pragma once
#include "Math/Vector.h"

namespace Tuna {
	//=====================================================================
	// AABB構造体
	//=====================================================================
	struct AABB {
		Vector3 min;
		Vector3 max;
	};
}