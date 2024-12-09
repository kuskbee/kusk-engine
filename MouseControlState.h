#pragma once

#include <directxtk/SimpleMath.h>

namespace kusk {

using DirectX::SimpleMath::Vector3;

struct MouseControlState {
	Vector3 prevVector;
	Vector3 prevPos;
	float prevRatio;
	bool isDragging;

	MouseControlState( ) { Reset( ); }
	void Reset( ) {
		prevVector = Vector3(0.0f);
		prevPos = Vector3(0.0f);
		prevRatio = 0.0f;
		isDragging = false;
	}
};
} // namespace kusk
