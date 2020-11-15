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
// 矩阵
struct XMatrix4x4f {
	float mData[16];
};
// vec4
struct XVector4f {
	float mData[4];
};
enum XUniformBufferType {
	kXUniformBufferTypeMatrix,
	kXUniformBufferTypeVector,
	kXUniformBufferTypeCount
};

struct XUniformBuffer {
	VkBuffer mBuffer;
	VkDeviceMemory mMemory;
	XUniformBufferType mType;
	std::vector<XMatrix4x4f> mMatrices;
	std::vector<XVector4f> mVector4s;
	XUniformBuffer();
	~XUniformBuffer();
};

class GraphicPipeline;

struct XProgram {
	VkPipelineShaderStageCreateInfo mShaderStage[2];
	int mShaderStagetCount;
	VkShaderModule mVertexShader, mFragmentShader;
	VkDescriptorSetLayout mDescriptorSetLayout;
	std::vector<VkDescriptorSetLayoutBinding> mDescriptorSetLayoutBindings;
	std::vector<VkDescriptorPoolSize> mDescriptorPoolSize;
	std::vector<VkWriteDescriptorSet> mWriteDescriptorSet;
	VkDescriptorSet mDescriptorSet;
	VkDescriptorPool mDescriptorPool;
	XUniformBuffer mVertexShaderMatrixUniformBuffer;
	XUniformBuffer mFragmentShaderMatrixUniformBuffer;
	XUniformBuffer mVertexShaderVectorUniformBuffer;
	XUniformBuffer mFragmentShaderVectorUniformBuffer;
	GraphicPipeline mFixedPipeline;
	XProgram();
	~XProgram();
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
// beginCommandbuffer
void xBeginOneTimeCommandBuffer(VkCommandBuffer* commandbuffer);
// endCommandbuffer
void xEndOneTimeCommandBuffer(VkCommandBuffer commandbuffer);
// 生成commandbuffer
void xGenCommandBuffer(VkCommandBuffer* commandbuffer, int count, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
// 等待队列提交完成
void xWaitForCommandFinish(VkCommandBuffer commandbuffer);
// 创建shadermodule
// code为我们的shader代码
// code_len为shader代码的长度
void xCreateShader(VkShaderModule& shader, unsigned char* code, int code_len);

void xAttachVertexShader(XProgram* program, VkShaderModule shader);

void xAttachFragmentShader(XProgram* program, VkShaderModule shader);
// 链接program
void xLinkProgram(XProgram* program);
// 设置uniform在GPU中的数据布局
void xInitDescriptorSetLayout(XProgram* program);

void xInitDescriptorPool(XProgram* program);

void xInitDescriptorSet(XProgram* program);