module;

#include "../Library/GLM.h"

export module ntmonkeys.com.Frameworks.SimpleMaterial;

import ntmonkeys.com.Engine.Material;

namespace Frameworks
{
	struct SimpleMaterialData
	{
	public:
		glm::vec4 color{ 0.0f, 0.0f, 0.0f, 1.0f };
	};

	export class SimpleMaterial : public Engine::TypedMaterial<SimpleMaterialData>
	{
	public:
		void setColor(const glm::vec4 &color);
	};
}

module: private;

namespace Frameworks
{
	void SimpleMaterial::setColor(const glm::vec4 &color)
	{
		_getTypedData().color = color;
		_invokeUpdateEvent();
	}
}