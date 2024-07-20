module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.LogicalDevice;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Graphics.ConversionUtil;
import ntmonkeys.com.Graphics.Queue;
import ntmonkeys.com.Graphics.Memory;
import ntmonkeys.com.Graphics.Buffer;
import ntmonkeys.com.Graphics.Surface;
import ntmonkeys.com.Graphics.Shader;
import ntmonkeys.com.Graphics.DescriptorSetLayout;
import ntmonkeys.com.Graphics.DescriptorPool;
import ntmonkeys.com.Graphics.PipelineLayout;
import ntmonkeys.com.Graphics.RenderPass;
import ntmonkeys.com.Graphics.Pipeline;
import ntmonkeys.com.Graphics.Framebuffer;
import ntmonkeys.com.Graphics.Semaphore;
import ntmonkeys.com.Graphics.Fence;
import ntmonkeys.com.Graphics.CommandPool;
import <vector>;
import <memory>;
import <stdexcept>;
import <unordered_map>;
import <format>;

namespace Graphics
{
	export class LogicalDevice : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::InstanceProc *pInstanceProc{ };
			VkInstance hInstance{ };
			VkPhysicalDevice hPhysicalDevice{ };
			uint32_t queueFamilyIndex{ };
			
			const VkPhysicalDeviceProperties *p10Props{ };
			const VkPhysicalDeviceVulkan11Properties *p11Props{ };
			const VkPhysicalDeviceVulkan12Properties *p12Props{ };
			const VkPhysicalDeviceVulkan13Properties *p13Props{ };
			const VkPhysicalDeviceRobustness2PropertiesEXT *pRobustness2Props{ };

			const VkPhysicalDeviceFeatures *p10Features{ };
			const VkPhysicalDeviceVulkan11Features *p11Features{ };
			const VkPhysicalDeviceVulkan12Features *p12Features{ };
			const VkPhysicalDeviceVulkan13Features *p13Features{ };
			const VkPhysicalDeviceRobustness2FeaturesEXT *pRobustness2Features{ };

			const std::unordered_map<std::string_view, const VkExtensionProperties *> *pExtensionMap;
		};

		struct GraphicsPipelineCreateInfo
		{
		public:
			VkPipelineLayout hPipelineLayout{ };
			VkRenderPass hRenderPass{ };
			uint32_t subpassIndex{ };
			VkPipelineCreateFlags flags{ };
			uint32_t stageCount{ };
			const VkPipelineShaderStageCreateInfo *pStages{ };
			const VkPipelineVertexInputStateCreateInfo *pVertexInputState{ };
			const VkPipelineInputAssemblyStateCreateInfo *pInputAssemblyState{ };
			const VkPipelineTessellationStateCreateInfo *pTessellationState{ };
			const VkPipelineViewportStateCreateInfo *pViewportState{ };
			const VkPipelineRasterizationStateCreateInfo *pRasterizationState{ };
			const VkPipelineMultisampleStateCreateInfo *pMultisampleState{ };
			const VkPipelineDepthStencilStateCreateInfo *pDepthStencilState{ };
			const VkPipelineColorBlendStateCreateInfo *pColorBlendState{ };
			const VkPipelineDynamicStateCreateInfo *pDynamicState{ };
		};

		explicit LogicalDevice(const CreateInfo &createInfo) noexcept;
		virtual ~LogicalDevice() noexcept override;
		
		[[nodiscard]]
		constexpr Queue &getQueue() noexcept;

		VkResult waitIdle() const noexcept;

		[[nodiscard]]
		Memory *createMemory(
			const VkDeviceSize size, const uint32_t typeIndex,
			const VkImage hDedicatedImage, const VkBuffer hDedicatedBuffer);

		[[nodiscard]]
		Buffer *createBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage);

		[[nodiscard]]
		Surface *createSurface(const HINSTANCE hAppInstance, const HWND hwnd);

		[[nodiscard]]
		Shader *createShader(const size_t codeSize, const uint32_t *const pCode);

		[[nodiscard]]
		DescriptorSetLayout *createDescriptorSetLayout(
			const VkDescriptorSetLayoutCreateFlags flags,
			const uint32_t bindingCount, const VkDescriptorSetLayoutBinding *const pBindings);

		[[nodiscard]]
		DescriptorPool *createDescriptorPool(
			const VkDescriptorPoolCreateFlags flags, const uint32_t maxSets,
			const uint32_t poolSizeCount, const VkDescriptorPoolSize *const pPoolSizes);

		[[nodiscard]]
		PipelineLayout *createPipelineLayout(
			const uint32_t setLayoutCount, const VkDescriptorSetLayout *const pSetLayouts,
			const uint32_t pushConstantRangeCount, const VkPushConstantRange *const pPushConstantRanges);

		[[nodiscard]]
		RenderPass *createRenderPass(
			const uint32_t attachmentCount, const VkAttachmentDescription2 *const pAttachments,
			const uint32_t subpassCount, const VkSubpassDescription2 *const pSubpasses,
			const uint32_t dependencyCount, const VkSubpassDependency2 *const pDependencies);

		[[nodiscard]]
		Pipeline *createPipeline(const GraphicsPipelineCreateInfo &createInfo);

		[[nodiscard]]
		Framebuffer *createFramebuffer(
			const VkRenderPass hRenderPass, const uint32_t width, const uint32_t height,
			const uint32_t attachmentCount, const Framebuffer::AttachmentInfo *const pAttachments);

		[[nodiscard]]
		Semaphore *createSemaphore(const VkSemaphoreType type, const uint64_t initialValue);

		[[nodiscard]]
		Fence *createFence(const bool signaled);

		[[nodiscard]]
		CommandPool *createCommandPool(const VkCommandPoolCreateFlags flags);

	private:
		const VK::InstanceProc &__instanceProc;
		const VkInstance __hInstance;
		const VkPhysicalDevice __hPhysicalDevice;
		const uint32_t __queueFamilyIndex;

		VkDevice __handle{ };
		VK::DeviceProc __deviceProc;

		std::unique_ptr<Queue> __pQueue;
		VkPipelineCache __hPipelineCache{ };

		void __createDevice(const CreateInfo &createInfo);
		void __loadDeviceProc() noexcept;
		void __retrieveQueue();
		void __createPipelineCache();
	};

	constexpr Queue &LogicalDevice::getQueue() noexcept
	{
		return *__pQueue;
	}
}

module: private;

#pragma warning(disable: 5103)
#define LOAD_DEVICE_PROC(funcName) (__deviceProc.##funcName = reinterpret_cast<PFN_##funcName>(__instanceProc.vkGetDeviceProcAddr(__handle, #funcName)))

namespace Graphics
{
	LogicalDevice::LogicalDevice(const CreateInfo &createInfo) noexcept :
		__instanceProc		{ *(createInfo.pInstanceProc) },
		__hInstance			{ createInfo.hInstance },
		__hPhysicalDevice	{ createInfo.hPhysicalDevice },
		__queueFamilyIndex	{ createInfo.queueFamilyIndex }
	{
		__createDevice(createInfo);
		__loadDeviceProc();
		__retrieveQueue();
		__createPipelineCache();
	}

	LogicalDevice::~LogicalDevice() noexcept
	{
		__deviceProc.vkDestroyPipelineCache(__handle, __hPipelineCache, nullptr);
		__deviceProc.vkDestroyDevice(__handle, nullptr);
	}

	VkResult LogicalDevice::waitIdle() const noexcept
	{
		return __deviceProc.vkDeviceWaitIdle(__handle);
	}

	Memory *LogicalDevice::createMemory(
		const VkDeviceSize size, const uint32_t typeIndex,
		const VkImage hDedicatedImage, const VkBuffer hDedicatedBuffer)
	{
		const Memory::CreateInfo createInfo
		{
			.pDeviceProc		{ &__deviceProc },
			.hDevice			{ __handle },
			.size				{ size },
			.typeIndex			{ typeIndex },
			.hDedicatedImage	{ hDedicatedImage },
			.hDedicatedBuffer	{ hDedicatedBuffer }
		};

		return new Memory{ createInfo };
	}

	Buffer *LogicalDevice::createBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage)
	{
		const Buffer::CreateInfo createInfo
		{
			.pDeviceProc	{ &__deviceProc },
			.hDevice		{ __handle },
			.size			{ size },
			.usage			{ usage }
		};

		return new Buffer{ createInfo };
	}

	Surface *LogicalDevice::createSurface(const HINSTANCE hAppInstance, const HWND hwnd)
	{
		const Surface::CreateInfo createInfo
		{
			.pInstanceProc		{ &__instanceProc },
			.hInstance			{ __hInstance },
			.hPhysicalDevice	{ __hPhysicalDevice },
			.pDeviceProc		{ &__deviceProc },
			.hDevice			{ __handle },
			.queueFamilyIndex	{ __queueFamilyIndex },
			.hAppInstance		{ hAppInstance },
			.hwnd				{ hwnd }
		};

		return new Surface{ createInfo };
	}

	Shader *LogicalDevice::createShader(const size_t codeSize, const uint32_t *const pCode)
	{
		const Shader::CreateInfo createInfo
		{
			.pDeviceProc	{ &__deviceProc },
			.hDevice		{ __handle },
			.codeSize		{ codeSize },
			.pCode			{ pCode }
		};

		return new Shader{ createInfo };
	}

	DescriptorSetLayout *LogicalDevice::createDescriptorSetLayout(
		const VkDescriptorSetLayoutCreateFlags flags,
		const uint32_t bindingCount, const VkDescriptorSetLayoutBinding *const pBindings)
	{
		const DescriptorSetLayout::CreateInfo createInfo
		{
			.pDeviceProc	{ &__deviceProc },
			.hDevice		{ __handle },
			.flags			{ flags },
			.bindingCount	{ bindingCount },
			.pBindings		{ pBindings }
		};

		return new DescriptorSetLayout{ createInfo };
	}

	DescriptorPool *LogicalDevice::createDescriptorPool(
		const VkDescriptorPoolCreateFlags flags, const uint32_t maxSets,
		const uint32_t poolSizeCount, const VkDescriptorPoolSize *const pPoolSizes)
	{
		const DescriptorPool::CreateInfo createInfo
		{
			.pDeviceProc	{ &__deviceProc },
			.hDevice		{ __handle },
			.flags			{ flags },
			.maxSets		{ maxSets },
			.poolSizeCount	{ poolSizeCount },
			.pPoolSizes		{ pPoolSizes }
		};

		return new DescriptorPool{ createInfo };
	}

	PipelineLayout *LogicalDevice::createPipelineLayout(
		const uint32_t setLayoutCount,
		const VkDescriptorSetLayout *const pSetLayouts,
		const uint32_t pushConstantRangeCount,
		const VkPushConstantRange *const pPushConstantRanges)
	{
		const PipelineLayout::CreateInfo createInfo
		{
			.pDeviceProc				{ &__deviceProc },
			.hDevice					{ __handle },
			.setLayoutCount				{ setLayoutCount },
			.pSetLayouts				{ pSetLayouts },
			.pushConstantRangeCount		{ pushConstantRangeCount },
			.pPushConstantRanges		{ pPushConstantRanges }
		};

		return new PipelineLayout{ createInfo };
	}

	RenderPass *LogicalDevice::createRenderPass(
		const uint32_t attachmentCount, const VkAttachmentDescription2 *const pAttachments,
		const uint32_t subpassCount, const VkSubpassDescription2 *const pSubpasses,
		const uint32_t dependencyCount, const VkSubpassDependency2 *const pDependencies)
	{
		const RenderPass::CreateInfo createInfo
		{
			.pDeviceProc		{ &__deviceProc },
			.hDevice			{ __handle },
			.attachmentCount	{ attachmentCount },
			.pAttachments		{ pAttachments },
			.subpassCount		{ subpassCount },
			.pSubpasses			{ pSubpasses },
			.dependencyCount	{ dependencyCount },
			.pDependencies		{ pDependencies }
		};

		return new RenderPass{ createInfo };
	}

	Pipeline *LogicalDevice::createPipeline(const GraphicsPipelineCreateInfo &createInfo)
	{
		const Pipeline::GraphicsCreateInfo innerCreateInfo
		{
			.pDeviceProc			{ &__deviceProc },
			.hDevice				{ __handle },
			.hPipelineCache			{ __hPipelineCache },
			.hPipelineLayout		{ createInfo.hPipelineLayout },
			.hRenderPass			{ createInfo.hRenderPass },
			.subpassIndex			{ createInfo.subpassIndex },
			.flags					{ createInfo.flags },
			.stageCount				{ createInfo.stageCount },
			.pStages				{ createInfo.pStages },
			.pVertexInputState		{ createInfo.pVertexInputState },
			.pInputAssemblyState	{ createInfo.pInputAssemblyState },
			.pTessellationState		{ createInfo.pTessellationState },
			.pViewportState			{ createInfo.pViewportState },
			.pRasterizationState	{ createInfo.pRasterizationState },
			.pMultisampleState		{ createInfo.pMultisampleState },
			.pDepthStencilState		{ createInfo.pDepthStencilState },
			.pColorBlendState		{ createInfo.pColorBlendState },
			.pDynamicState			{ createInfo.pDynamicState }
		};

		return new Pipeline{ innerCreateInfo };
	}

	Framebuffer *LogicalDevice::createFramebuffer(
		const VkRenderPass hRenderPass, const uint32_t width, const uint32_t height,
		const uint32_t attachmentCount, const Framebuffer::AttachmentInfo *const pAttachments)
	{
		const Framebuffer::CreateInfo createInfo
		{
			.pDeviceProc		{ &__deviceProc },
			.hDevice			{ __handle },
			.hRenderPass		{ hRenderPass },
			.width				{ width },
			.height				{ height },
			.attachmentCount	{ attachmentCount },
			.pAttachments		{ pAttachments }
		}; 

		return new Framebuffer{ createInfo };
	}

	Semaphore *LogicalDevice::createSemaphore(const VkSemaphoreType type, const uint64_t initialValue)
	{
		const Semaphore::CreateInfo createInfo
		{
			.pDeviceProc		{ &__deviceProc },
			.hDevice			{ __handle },
			.type				{ type },
			.initialValue		{ initialValue }
		}; 

		return new Semaphore{ createInfo };
	}

	Fence *LogicalDevice::createFence(const bool signaled)
	{
		const Fence::CreateInfo createInfo
		{
			.pDeviceProc	{ &__deviceProc },
			.hDevice		{ __handle },
			.signaled		{ signaled }
		}; 

		return new Fence{ createInfo };
	}

	CommandPool *LogicalDevice::createCommandPool(const VkCommandPoolCreateFlags flags)
	{
		const CommandPool::CreateInfo createInfo
		{
			.pDeviceProc		{ &__deviceProc },
			.hDevice			{ __handle },
			.flags				{ flags },
			.queueFamilyIndex	{ __queueFamilyIndex }
		}; 

		return new CommandPool{ createInfo };
	}

	void LogicalDevice::__createDevice(const CreateInfo &createInfo)
	{
		const auto deviceVersion{ Graphics::ConversionUtil::fromVulkanVersion(createInfo.p10Props->apiVersion) };
		if ((deviceVersion.major <= 1U) && (deviceVersion.minor < 3U))
			throw std::runtime_error{ "The device API version is too low." };

		const bool featureSupported
		{
			createInfo.p10Features->samplerAnisotropy &&
			createInfo.p11Features->variablePointers &&
			createInfo.p11Features->variablePointersStorageBuffer &&
			createInfo.p12Features->imagelessFramebuffer &&
			createInfo.p12Features->timelineSemaphore &&
			createInfo.p13Features->pipelineCreationCacheControl &&
			createInfo.p13Features->synchronization2 &&
			createInfo.pRobustness2Features->nullDescriptor
		};

		if (!featureSupported)
			throw std::runtime_error{ "The device doesn't support the features." };

		if (!(__instanceProc.vkGetPhysicalDeviceWin32PresentationSupportKHR(__hPhysicalDevice, __queueFamilyIndex)))
			throw std::runtime_error{ "The device doesn't support win32 presentation." };
		
		VkPhysicalDeviceFeatures2 features{ };
		VkPhysicalDeviceVulkan11Features features11{ };
		VkPhysicalDeviceVulkan12Features features12{ };
		VkPhysicalDeviceVulkan13Features features13{ };
		VkPhysicalDeviceRobustness2FeaturesEXT featuresRobustness{ };

		features.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		features.features.samplerAnisotropy = VK_TRUE;
		features.pNext = &features11;

		features11.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
		features11.variablePointers = VK_TRUE;
		features11.variablePointersStorageBuffer = VK_TRUE;
		features11.pNext = &features12;

		features12.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		features12.imagelessFramebuffer = VK_TRUE;
		features12.timelineSemaphore = VK_TRUE;
		features12.pNext = &features13;

		features13.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		features13.pipelineCreationCacheControl = VK_TRUE;
		features13.synchronization2 = VK_TRUE;
		features13.pNext = &featuresRobustness;

		featuresRobustness.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
		featuresRobustness.nullDescriptor = VK_TRUE;
		featuresRobustness.pNext = nullptr;

		std::vector<const char *> extensions;
		extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
		extensions.emplace_back(VK_EXT_ROBUSTNESS_2_EXTENSION_NAME);
		//extensions.emplace_back(VK_EXT_IMAGE_COMPRESSION_CONTROL_EXTENSION_NAME);

		for (const auto extension : extensions)
		{
			if (createInfo.pExtensionMap->contains(extension))
				continue;

			throw std::runtime_error{ std::format("Device extension not supported: {}", extension) };
		}

		static constexpr float queuePriority{ 1.0f };

		const VkDeviceQueueCreateInfo queueCreateInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO },
			.queueFamilyIndex	{ __queueFamilyIndex },
			.queueCount			{ 1U },
			.pQueuePriorities	{ &queuePriority }
		};

		const VkDeviceCreateInfo vkCreateInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO },
			.pNext						{ &features },
			.queueCreateInfoCount		{ 1U },
			.pQueueCreateInfos			{ &queueCreateInfo },
			.enabledExtensionCount		{ static_cast<uint32_t>(extensions.size()) },
			.ppEnabledExtensionNames	{ extensions.data() }
		};

		__instanceProc.vkCreateDevice(__hPhysicalDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a VkDevice." };
	}

	void LogicalDevice::__loadDeviceProc() noexcept
	{
		// Device
		LOAD_DEVICE_PROC(vkDeviceWaitIdle);
		LOAD_DEVICE_PROC(vkDestroyDevice);
		
		// Queue
		LOAD_DEVICE_PROC(vkGetDeviceQueue2);
		LOAD_DEVICE_PROC(vkQueueWaitIdle);
		LOAD_DEVICE_PROC(vkQueueSubmit2);
		LOAD_DEVICE_PROC(vkQueuePresentKHR);

		// Descriptor set layout
		LOAD_DEVICE_PROC(vkCreateDescriptorSetLayout);
		LOAD_DEVICE_PROC(vkDestroyDescriptorSetLayout);

		// Descriptor pool
		LOAD_DEVICE_PROC(vkCreateDescriptorPool);
		LOAD_DEVICE_PROC(vkDestroyDescriptorPool);
		LOAD_DEVICE_PROC(vkResetDescriptorPool);

		// Descriptor set
		LOAD_DEVICE_PROC(vkAllocateDescriptorSets);
		LOAD_DEVICE_PROC(vkFreeDescriptorSets);
		
		// Shader module;
		LOAD_DEVICE_PROC(vkCreateShaderModule);
		LOAD_DEVICE_PROC(vkDestroyShaderModule);

		// Render pass
		LOAD_DEVICE_PROC(vkCreateRenderPass2);
		LOAD_DEVICE_PROC(vkDestroyRenderPass);

		// Pipeline layout
		LOAD_DEVICE_PROC(vkCreatePipelineLayout);
		LOAD_DEVICE_PROC(vkDestroyPipelineLayout);

		// Pipeline cache
		LOAD_DEVICE_PROC(vkCreatePipelineCache);
		LOAD_DEVICE_PROC(vkDestroyPipelineCache);

		// Pipeline
		LOAD_DEVICE_PROC(vkCreateGraphicsPipelines);
		LOAD_DEVICE_PROC(vkDestroyPipeline);

		// Swapchain
		LOAD_DEVICE_PROC(vkCreateSwapchainKHR);
		LOAD_DEVICE_PROC(vkDestroySwapchainKHR);
		LOAD_DEVICE_PROC(vkGetSwapchainImagesKHR);
		LOAD_DEVICE_PROC(vkAcquireNextImage2KHR);

		// Image view
		LOAD_DEVICE_PROC(vkCreateImageView);
		LOAD_DEVICE_PROC(vkDestroyImageView);

		// Framebuffer
		LOAD_DEVICE_PROC(vkCreateFramebuffer);
		LOAD_DEVICE_PROC(vkDestroyFramebuffer);

		// Semaphore
		LOAD_DEVICE_PROC(vkCreateSemaphore);
		LOAD_DEVICE_PROC(vkDestroySemaphore);

		// Fence
		LOAD_DEVICE_PROC(vkCreateFence);
		LOAD_DEVICE_PROC(vkDestroyFence);
		LOAD_DEVICE_PROC(vkWaitForFences);
		LOAD_DEVICE_PROC(vkResetFences);

		// Command pool
		LOAD_DEVICE_PROC(vkCreateCommandPool);
		LOAD_DEVICE_PROC(vkDestroyCommandPool);
		LOAD_DEVICE_PROC(vkAllocateCommandBuffers);
		LOAD_DEVICE_PROC(vkResetCommandPool);

		// Command buffer
		LOAD_DEVICE_PROC(vkBeginCommandBuffer);
		LOAD_DEVICE_PROC(vkEndCommandBuffer);
		LOAD_DEVICE_PROC(vkCmdBeginRenderPass2);
		LOAD_DEVICE_PROC(vkCmdEndRenderPass2);
		LOAD_DEVICE_PROC(vkCmdBindPipeline);
		LOAD_DEVICE_PROC(vkCmdSetViewport);
		LOAD_DEVICE_PROC(vkCmdSetScissor);
		LOAD_DEVICE_PROC(vkCmdDraw);
		LOAD_DEVICE_PROC(vkCmdDrawIndexed);
		LOAD_DEVICE_PROC(vkCmdCopyBuffer2);
		LOAD_DEVICE_PROC(vkCmdCopyBufferToImage2);
		LOAD_DEVICE_PROC(vkCmdPipelineBarrier2);
		LOAD_DEVICE_PROC(vkCmdBindVertexBuffers);
		LOAD_DEVICE_PROC(vkCmdBindIndexBuffer);

		// Memory
		LOAD_DEVICE_PROC(vkAllocateMemory);
		LOAD_DEVICE_PROC(vkFreeMemory);
		LOAD_DEVICE_PROC(vkMapMemory);
		LOAD_DEVICE_PROC(vkUnmapMemory);

		// Buffer
		LOAD_DEVICE_PROC(vkCreateBuffer);
		LOAD_DEVICE_PROC(vkDestroyBuffer);
		LOAD_DEVICE_PROC(vkGetBufferMemoryRequirements2);
		LOAD_DEVICE_PROC(vkBindBufferMemory2);
	}

	void LogicalDevice::__retrieveQueue()
	{
		const Queue::CreateInfo createInfo
		{
			.pDeviceProc	{ &__deviceProc },
			.hDevice		{ __handle },
			.familyIndex	{ __queueFamilyIndex },
			.queueIndex		{ 0U }
		};

		__pQueue = std::make_unique<Queue>(createInfo);
	}

	void LogicalDevice::__createPipelineCache()
	{
		const VkPipelineCacheCreateInfo createInfo
		{
			.sType{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO },
			.flags{ VkPipelineCacheCreateFlagBits::VK_PIPELINE_CACHE_CREATE_EXTERNALLY_SYNCHRONIZED_BIT }
		};

		__deviceProc.vkCreatePipelineCache(__handle, &createInfo, nullptr, &__hPipelineCache);
		if (!__hPipelineCache)
			throw std::runtime_error{ "Cannot create a pipeline cache." };
	}
}