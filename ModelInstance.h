#pragma once

#include <memory>

#include "Model.h"

// 참고: DirectX-Graphics-Sampels
// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Model/Model.h

namespace kusk {

class ModelInstance {
public:
	std::shared_ptr<const Model> m_model;
};

} // namespace kusk