#pragma once
#include "Opengl//asset/all.h"
namespace Ext {

	std::tuple< std::shared_ptr<asset::Texture>, std::shared_ptr<asset::Texture>, std::shared_ptr<asset::Texture>> PrecomputeIBL(const std::string& hdri);

}