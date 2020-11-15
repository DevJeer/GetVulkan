#include "BVulkan.h"
#include "XVulkan.h"

XBufferObject::XBufferObject()
{
	mBuffer = 0;
	mMemory = 0;
}

XBufferObject::~XBufferObject()
{
	if (mBuffer != 0) {
		vkDestroyBuffer(GetVulkanDevice(), mBuffer, nullptr);
	}
	if (mMemory != 0) {
		vkFreeMemory(GetVulkanDevice(), mMemory, nullptr);
	}
}

XUniformBuffer::XUniformBuffer() {
	mBuffer = 0;
	mMemory = 0;
}

XUniformBuffer::~XUniformBuffer() {
	if (mBuffer != 0) {
		vkDestroyBuffer(GetVulkanDevice(), mBuffer, nullptr);
	}
	if (mMemory != 0) {
		vkFreeMemory(GetVulkanDevice(), mMemory, nullptr);
	}
}

XProgram::XProgram() {
	mShaderStagetCount = 0;
	mVertexShader = 0;
	mFragmentShader = 0;
	mDescriptorPool = 0;
	mDescriptorSetLayout = 0;
	memset(mShaderStage, 0, sizeof(VkPipelineShaderStageCreateInfo) * 2);
}

XProgram::~XProgram() {
	if (mVertexShader != 0) {
		vkDestroyShaderModule(GetVulkanDevice(), mVertexShader, nullptr);
	}
	if (mFragmentShader != 0) {
		vkDestroyShaderModule(GetVulkanDevice(), mFragmentShader, nullptr);
	}
	if (mDescriptorPool != 0) {
		vkDestroyDescriptorPool(GetVulkanDevice(), mDescriptorPool, nullptr);
	}
	if (mDescriptorSetLayout != 0) {
		vkDestroyDescriptorSetLayout(GetVulkanDevice(), mDescriptorSetLayout, nullptr);
	}
	for (int i = 0; i < mWriteDescriptorSet.size(); ++i) {
		VkWriteDescriptorSet* wds = &mWriteDescriptorSet[i];
		if (wds->pBufferInfo != nullptr) {
			delete wds->pBufferInfo;
		}
		if (wds->pImageInfo != nullptr) {
			delete wds->pImageInfo;
		}
	}
}

void xglBufferData(XVulkanHandle buffer, int size, void* data) {
	// CPU端运行， 分配vbo
	XBufferObject* vbo = (XBufferObject*)buffer;
	// 生成vbo
	xGenVertexBuffer(size, vbo->mBuffer, vbo->mMemory);
	// CPU -> GPU数据传输
	xBufferSubVertexData(vbo->mBuffer, data, size);
}

VkResult xGenBuffer(VkBuffer& buffer, VkDeviceMemory& buffermemory, VkDeviceSize size,
	VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	// createinfo GPU端  
	VkBufferCreateInfo bufferinfo = {};
	bufferinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferinfo.size = size;
	bufferinfo.usage = usage;

	VkResult ret = vkCreateBuffer(GetVulkanDevice(), &bufferinfo, nullptr, &buffer);
	if (ret != VK_SUCCESS) {
		printf("failed to create buffer\n");
		return ret;
	}

	// GPU端 物理空间
	VkMemoryRequirements requirements;
	vkGetBufferMemoryRequirements(GetVulkanDevice(), buffer, &requirements);
	VkMemoryAllocateInfo memoryallocinfo = {};
	memoryallocinfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryallocinfo.allocationSize = requirements.size;
	memoryallocinfo.memoryTypeIndex = xGetMemoryType(requirements.memoryTypeBits, properties);
	ret = vkAllocateMemory(GetVulkanDevice(), &memoryallocinfo, nullptr, &buffermemory);
	if (ret != VK_SUCCESS) {
		printf("failed to alloc memory\n");
		return ret;
	}

	// 关联在一起
	vkBindBufferMemory(GetVulkanDevice(), buffer, buffermemory, 0);
	return VK_SUCCESS;
}

void xBufferSubData(VkBuffer buffer, VkBufferUsageFlags usage, const void* data, VkDeviceSize size) {
	// 在cpu中创建了一个vbo
	// 因为cpu无法直接像gpu中传递数据
	// 在靠近gpu位置创建vbo
	VkBuffer tempbuffer;
	VkDeviceMemory tempmemory;
	xGenBuffer(tempbuffer, tempmemory, size, usage, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	void* host_memory;
	vkMapMemory(GetVulkanDevice(), tempmemory, 0, size, 0, &host_memory);
	memcpy(host_memory, data, (size_t)size);
	vkUnmapMemory(GetVulkanDevice(), tempmemory);

	VkCommandBuffer commandbuffer;
	xBeginOneTimeCommandBuffer(&commandbuffer);
	VkBufferCopy copy = { 0, 0, size };
	vkCmdCopyBuffer(commandbuffer, tempbuffer, buffer, 1, &copy);
	xEndOneTimeCommandBuffer(commandbuffer);

	vkDestroyBuffer(GetVulkanDevice(), tempbuffer, nullptr);
	vkFreeMemory(GetVulkanDevice(), tempmemory, nullptr);
}

uint32_t xGetMemoryType(uint32_t type_filters, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(GetVulkanPhysicalDevice(), &memory_properties);
	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i) {
		uint32_t flag = 1 << i;
		if ((flag & type_filters) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i; // 找到了
		}
	}
	// 查找不到，返回0
	return 0;
}

void xBeginOneTimeCommandBuffer(VkCommandBuffer* commandbuffer) {
	xGenCommandBuffer(commandbuffer, 1);
	VkCommandBufferBeginInfo cbbi = {};
	cbbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cbbi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(*commandbuffer, &cbbi);
}

void xEndOneTimeCommandBuffer(VkCommandBuffer commandbuffer) {
	vkEndCommandBuffer(commandbuffer);
	xWaitForCommandFinish(commandbuffer);
	vkFreeCommandBuffers(GetVulkanDevice(), GetCommandPool(), 1, &commandbuffer);
}

void xGenCommandBuffer(VkCommandBuffer* commandbuffer, int count, VkCommandBufferLevel level) {
	VkCommandBufferAllocateInfo cbai = {};
	cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cbai.level = level;
	cbai.commandPool = GetCommandPool();
	cbai.commandBufferCount = count;
	vkAllocateCommandBuffers(GetVulkanDevice(), &cbai, commandbuffer);
}

void xWaitForCommandFinish(VkCommandBuffer commandbuffer) {
	VkSubmitInfo submitinfo = {};
	submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitinfo.commandBufferCount = 1;
	submitinfo.pCommandBuffers = &commandbuffer;

	VkFenceCreateInfo fci = {};
	fci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	VkFence fence;
	vkCreateFence(GetVulkanDevice(), &fci, nullptr, &fence);
	// 等待cpu端提交完成
	vkQueueSubmit(GetGraphicQueue(), 1, &submitinfo, fence);
	vkWaitForFences(GetVulkanDevice(), 1, &fence, VK_TRUE, 1000000000);
	vkDestroyFence(GetVulkanDevice(), fence, nullptr);
}

void xCreateShader(VkShaderModule& shader, unsigned char* code, int code_len) {
	VkShaderModuleCreateInfo smci = {};
	smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	smci.codeSize = code_len;
	smci.pCode = (uint32_t*)code;
	vkCreateShaderModule(GetVulkanDevice(), &smci, nullptr, &shader);
}