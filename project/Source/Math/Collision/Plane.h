#pragma once
#include "Math/Vector.h"

namespace Tuna {
	//=====================================================================
	// Plane構造体
	//=====================================================================
	struct Plane {
		Vector3 normal;
		float distance;
	};
}