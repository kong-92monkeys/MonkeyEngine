module;

#include "../Library/GLM.h"

export module ntmonkeys.com.Frameworks.SimpleMaterial;

import ntmonkeys.com.Engine.Material;
import ntmonkeys.com.Engine.Texture;
import <memory>;

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
		void setTexture(const std::shared_ptr<Engine::Texture> &pTexture);

	private:
		static constexpr uint32_t __TEXTURE_SLOT_INDEX{ 0U };

		std::shared_ptr<Engine::Texture> __pTexture;
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

	void SimpleMaterial::setTexture(const std::shared_ptr<Engine::Texture> &pTexture)
	{
		if (__pTexture == pTexture)
			return;

		if (__pTexture)
			_unregisterTexture(__pTexture.get());

		__pTexture = pTexture;

		if (pTexture)
			_registerTexture(pTexture.get(), __TEXTURE_SLOT_INDEX);
	}
}