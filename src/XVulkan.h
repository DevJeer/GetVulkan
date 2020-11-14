#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
typedef void* XVulkanHandle;

struct XBufferObject {
	VkBuffer mBuffer;
	VkDeviceMemory mMemory;
	XBufferObject();
	~XBufferObject();
};
// 填充vbo bufferdata
void xglBufferData(XVulkanHandle vbo, int size, void* data);
// 创建vbo
VkResult xGenBuffer(VkBuffer& buffer, VkDeviceMemory& buffermemory, VkDeviceSize size,
	VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
// 填充fbo
void xBufferSubData(VkBuffer buffer, VkBufferUsageFlags usage, const void* data,
	VkDeviceSize size);


#define  xGenVertexBuffer(size, buffer, buffermemory)\
	xGenBuffer(buffer, buffermemory, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)

#define xBufferSubVertexData(buffer, data, size)\
	xBufferSubData(buffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT|VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, data, size)
// 获取内存的属性
uint32_t xGetMemoryType(uint32_t type_filters, VkMemoryPropertyFlags properties);
