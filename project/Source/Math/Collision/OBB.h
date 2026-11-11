#pragma once
#include "Math/Vector.h"

namespace Tuna {
	//=====================================================================
	// OBB構造体
	//=====================================================================
	struct OBB {
		Vector3 center;
		Vector3 orientations[3];
		Vector3 size;
	};
}