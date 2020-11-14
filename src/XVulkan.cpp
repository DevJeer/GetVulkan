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
	// CPU�����У� ����vbo
	XBufferObject* vbo = (XBufferObject*)buffer;
	// ����vbo
	xGenVertexBuffer(size, vbo->mBuffer, vbo->mMemory);
	// CPU -> GPU���ݴ���
	xBufferSubVertexData(vbo->mBuffer, data, size);
}

VkResult xGenBuffer(VkBuffer& buffer, VkDeviceMemory& buffermemory, VkDeviceSize size,
	VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	// createinfo GPU��  
	VkBufferCreateInfo bufferinfo = {};
	bufferinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferinfo.size = size;
	bufferinfo.usage = usage;

	VkResult ret = vkCreateBuffer(GetVulkanDevice(), &bufferinfo, nullptr, &buffer);
	if (ret != VK_SUCCESS) {
		printf("failed to create buffer\n");
		return ret;
	}

	// GPU�� ����ռ�
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

	// ������һ��
	vkBindBufferMemory(GetVulkanDevice(), buffer, buffermemory, 0);
	return VK_SUCCESS;
}

void xBufferSubData(VkBuffer buffer, VkBufferUsageFlags usage, const void* data, VkDeviceSize size) {
	// ��cpu�д�����һ��vbo
	// ��Ϊcpu�޷�ֱ����gpu�д�������
	// �ڿ���gpuλ�ô���vbo
	VkBuffer tempbuffer;
	VkDeviceMemory tempmemory;
	xGenBuffer(tempbuffer, tempmemory, size, usage, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	void* host_memory;
	vkMapMemory(GetVulkanDevice(), tempmemory, 0, size, 0, &host_memory);
	memcpy(host_memory, data, (size_t)size);
	vkUnmapMemory(GetVulkanDevice(), tempmemory);

	VkCommandBuffer commandbuffer;
	aBeginOneTimeCommandBuffer(&commandbuffer);
	VkBufferCopy copy = { 0, 0, size };
	vkCmdCopyBuffer(commandbuffer, tempbuffer, buffer, 1, &copy);
	aEndOneTimeCommandBuffer(commandbuffer);

	vkDestroyBuffer(GetVulkanDevice(), tempbuffer, nullptr);
	vkFreeMemory(GetVulkanDevice(), tempmemory, nullptr);
}