#include "XVulkan.h"
#include "BVulkan.h"

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

void xglBufferData(XVulkanHandle buffer, int size, void* data) {
	// CPU端运行， 分配vbo
	XBufferObject* vbo = (XBufferObject*)buffer;
	xGenVertexBuffer(size, vbo->mBuffer, vbo->mMemory);
	aBufferSubVertexData(vbo->mBuffer, data, size);
}

VkResult xGenBuffer(VkBuffer& buffer, VkDeviceMemory& buffermemory, VkDeviceSize size,
	VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	// createinfo CPU端  逻辑空间
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
	memoryallocinfo.memoryTypeIndex = FindMemoryType(requirements.memoryTypeBits, properties);
	ret = vkAllocateMemory(GetVulkanDevice(), &memoryallocinfo, nullptr, &buffermemory);
	if (ret != VK_SUCCESS) {
		printf("failed to alloc memory\n");
		return ret;
	}

	// 关联在一起
	vkBindBufferMemory(GetVulkanDevice(), buffer, buffermemory, 0);
	return VK_SUCCESS;
}