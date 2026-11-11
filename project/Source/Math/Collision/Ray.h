#pragma once
#include "Math/Vector.h"

namespace Tuna {
	//=====================================================================
	// Ray構造体
	//=====================================================================
	struct Ray {
		Vector3 origin;
		Vector3 diff;
	};
}