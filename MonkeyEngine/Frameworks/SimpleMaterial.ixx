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
		void setAlbedoTexture(const std::shared_ptr<Engine::Texture> &pTexture);

		[[nodiscard]]
		virtual uint32_t getTextureSlotCount() const noexcept override;

	private:
		static constexpr uint32_t __ALBEDO_TEX_SLOT_INDEX{ 0U };

		std::shared_ptr<Engine::Texture> __pAlbedoTexture;
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

	void SimpleMaterial::setAlbedoTexture(const std::shared_ptr<Engine::Texture> &pTexture)
	{
		__pAlbedoTexture = pTexture;
		_setTexture(__ALBEDO_TEX_SLOT_INDEX, pTexture.get());
	}

	uint32_t SimpleMaterial::getTextureSlotCount() const noexcept
	{
		return 1U;
	}
}