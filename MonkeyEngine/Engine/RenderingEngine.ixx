module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.RenderingEngine;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.PhysicalDevice;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.PipelineCache;
import ntmonkeys.com.Graphics.ConversionUtil;
import ntmonkeys.com.Engine.RenderTarget;
import <optional>;
import <stdexcept>;
import <memory>;
import <format>;

namespace Engine
{
	export class RenderingEngine : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const Graphics::PhysicalDevice *pPhysicalDevice{ };
		};

		RenderingEngine(const CreateInfo &createInfo);
		virtual ~RenderingEngine() noexcept override;

		[[nodiscard]]
		std::unique_ptr<RenderTarget> createRenderTarget(const HINSTANCE hinstance, const HWND hwnd);

	private:
		const Graphics::PhysicalDevice &__physicalDevice;

		uint32_t __queueFamilyIndex{ };
		std::unique_ptr<Graphics::LogicalDevice> __pLogicalDevice;
		std::unique_ptr<Graphics::PipelineCache> __pPipelineCache;

		void __resolveQueueFamilyIndex();
		void __createLogicalDevice();
		void __createPipelineCache();
	};
}

module: private;

namespace Engine
{
	RenderingEngine::RenderingEngine(const CreateInfo &createInfo) :
		__physicalDevice{ *(createInfo.pPhysicalDevice) }
	{
		__resolveQueueFamilyIndex();
		__createLogicalDevice();
		__createPipelineCache();
	}

	RenderingEngine::~RenderingEngine() noexcept
	{
		__pPipelineCache = nullptr;
		__pLogicalDevice = nullptr;
	}

	std::unique_ptr<RenderTarget> RenderingEngine::createRenderTarget(const HINSTANCE hinstance, const HWND hwnd)
	{
		const RenderTarget::CreateInfo createInfo
		{
			.pLogicalDevice	{ __pLogicalDevice.get() },
			.hinstance		{ hinstance },
			.hwnd			{ hwnd }
		};

		return std::make_unique<RenderTarget>(createInfo);
	}

	void RenderingEngine::__resolveQueueFamilyIndex()
	{
		std::optional<uint32_t> familyIndex;

		const auto &familyInfos		{ __physicalDevice.getQueueFamilyInfos() };
		const uint32_t familyCount	{ static_cast<uint32_t>(familyInfos.size()) };
		
		for (uint32_t iter{ }; iter < familyCount; ++iter)
		{
			const auto &familyInfo{ familyInfos[iter] };

			const bool hasGraphicsFlag{ static_cast<bool>(familyInfo.pProps->queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT) };
			if (!hasGraphicsFlag)
				continue;

			familyIndex = iter;
			break;
		}

		if (!(familyIndex.has_value()))
			throw std::runtime_error{ "The device doesn't support graphics queue." };

		__queueFamilyIndex = familyIndex.value();
	}

	void RenderingEngine::__createLogicalDevice()
	{
		const auto &prop10				{ __physicalDevice.get10Props() };
		const auto &prop11				{ __physicalDevice.get11Props() };
		const auto &prop12				{ __physicalDevice.get12Props() };
		const auto &prop13				{ __physicalDevice.get13Props() };
		const auto &propRobustness		{ __physicalDevice.getRobustness2Props() };

		const auto &feature10			{ __physicalDevice.get10Features() };
		const auto &feature11			{ __physicalDevice.get11Features() };
		const auto &feature12			{ __physicalDevice.get12Features() };
		const auto &feature13			{ __physicalDevice.get13Features() };
		const auto &featureRobustness	{ __physicalDevice.getRobustness2Features() };

		const auto &extensionMap		{ __physicalDevice.getExtensionMap() };

		const auto deviceVersion		{ Graphics::ConversionUtil::fromVulkanVersion(prop10.apiVersion) };

		const bool featureSupported
		{
			feature10.samplerAnisotropy &&
			feature12.imagelessFramebuffer &&
			feature12.timelineSemaphore &&
			feature13.synchronization2 &&
			featureRobustness.nullDescriptor
		};

		if (!(__physicalDevice.isWin32PresentSupported(__queueFamilyIndex)))
			throw std::runtime_error{ "The device doesn't support win32 presentation." };

		if ((deviceVersion.major <= 1U) && (deviceVersion.minor < 3U))
			throw std::runtime_error{ "The device API version is too low." };

		if (!featureSupported)
			throw std::runtime_error{ "The device doesn't support the features." };

		VkPhysicalDeviceFeatures2 features{ };
		VkPhysicalDeviceVulkan11Features features11{ };
		VkPhysicalDeviceVulkan12Features features12{ };
		VkPhysicalDeviceVulkan13Features features13{ };
		VkPhysicalDeviceRobustness2FeaturesEXT featuresRobustness{ };

		features.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		features.features.samplerAnisotropy = VK_TRUE;
		features.pNext = &features11;

		features11.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
		features11.pNext = &features12;

		features12.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		features12.imagelessFramebuffer = VK_TRUE;
		features12.timelineSemaphore = VK_TRUE;
		features12.pNext = &features13;

		features13.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		features13.synchronization2 = VK_TRUE;
		features13.pNext = &featuresRobustness;

		featuresRobustness.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
		featuresRobustness.nullDescriptor = VK_TRUE;
		featuresRobustness.pNext = nullptr;

		std::vector<const char *> extensions;
		extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		extensions.emplace_back(VK_EXT_ROBUSTNESS_2_EXTENSION_NAME);
		extensions.emplace_back(VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME);
		//extensions.emplace_back(VK_EXT_IMAGE_COMPRESSION_CONTROL_EXTENSION_NAME);

		for (const auto extension : extensions)
		{
			if (extensionMap.contains(extension))
				continue;

			throw std::runtime_error{ std::format("Device extension not supported: {}", extension) };
		}

		__pLogicalDevice = __physicalDevice.createLogicalDevice(__queueFamilyIndex, features, extensions);
	}

	void RenderingEngine::__createPipelineCache()
	{
		__pPipelineCache = __pLogicalDevice->createPipelineCache();
	}
}