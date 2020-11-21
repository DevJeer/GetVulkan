#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include "../third-party/Glm/glm.hpp"
#include "../third-party/Glm/ext.hpp"
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
	// 构造函数初始化为单位矩阵
	XMatrix4x4f() {
		memset(mData, 0, sizeof(float) * 16);
		mData[0] = 1.0f;
		mData[5] = 1.0f;
		mData[10] = 1.0f;
		mData[15] = 1.0f;
	}
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

struct XTexture {
	// 逻辑对象
	VkImage mImage;
	// 物体内存
	VkDeviceMemory mMemory;
	// 插槽类似的对象，必须通过这个来访问逻辑对象
	VkImageView mImageView;
	// gpu中shader使用的image（image在gpu中的表现）
	VkSampler mSampler;
	// 表示imageLayout 初始状态
	VkImageLayout mInitLayout;
	// 最终要成为什么状态
	VkImageLayout mTargetLayout;
	// 在pipeline中源阶段
	VkPipelineStageFlags mSrcStage;
	// 最终要在哪个阶段使用
	VkPipelineStageFlags mTargetStage;
	// 表示颜色或者深度图
	VkImageAspectFlags mImageAspectFlag;
	// texture的格式
	VkFormat mFormat;
	// 纹理过滤的方式
	VkFilter mMinFilter, mMagFilter;
	// 边界重复的方式
	VkSamplerAddressMode mWrapU, mWrapV, mWrapW;
	// 硬件相关的选项（类似msaa?）
	VkBool32 mbEnableAnisotropy;
	float mMaxAnisotropy;
	XTexture(VkImageAspectFlags image_aspect = VK_IMAGE_ASPECT_COLOR_BIT);
	~XTexture();
};

// 填充vbo bufferdata
void xglBufferData(XVulkanHandle vbo, int size, void* data);
// 创建vbo
VkResult xGenBuffer(VkBuffer& buffer, VkDeviceMemory& buffermemory, VkDeviceSize size,
	VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
// 填充fbo
void xBufferSubData(VkBuffer buffer, VkBufferUsageFlags usage, const void* data,
	VkDeviceSize size);

// 创建vbo
#define  xGenVertexBuffer(size, buffer, buffermemory)\
	xGenBuffer(buffer, buffermemory, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
// 创建ibo
#define xGenIndexBuffer(size,buffer,buffermemory) \
	xGenBuffer(buffer,buffermemory,size,VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_INDEX_BUFFER_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)

#define xBufferSubVertexData(buffer, data, size)\
	xBufferSubData(buffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT|VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, data, size)
// 更新ibo里面的数据
#define xBufferSubIndexData(buffer,data,size) \
	xBufferSubData(buffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT|VK_BUFFER_USAGE_INDEX_BUFFER_BIT, data, size);

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
// 将uniformBuffer写入到gpu中
void xSubmitUniformBuffer(XUniformBuffer* uniformbuffer);
// 将uniform cpu->gpu中的路径打通 （补全一些信息） 
void xConfigUniformBuffer(XVulkanHandle param, int bingding, XUniformBuffer* ubo, VkShaderStageFlags shader_stage);
// 生成image
void xGenImage(XTexture* texture, uint32_t w, uint32_t h, VkFormat f,
	VkImageUsageFlags usage, VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT, int mipmap = 1);
// 将image从cpu拷贝到gpu中
void xSubmitImage2D(XTexture* texture, int width, int height, const void* pixel);
// 设置image的访问模式
void xSetImageLayout(VkCommandBuffer commandbuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
	VkImageSubresourceRange subresourcerange, VkPipelineStageFlags src = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	VkPipelineStageFlags dst = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
// 生成ImageView
void xGenImageView2D(XTexture* texture, int mipmap = 1);
// 生成sampler 在shader中访问纹理的时候使用
void xGenSampler(XTexture* texture);
// 初始化默认纹理贴图
void xInitDefaultTexture();
// 退出时，清除默认贴图的资源
void xVulkanCleanUp();
// 配置sampler2D
void xConfigSampler2D(XProgram* program, int binding, VkImageView imageview, VkSampler sampler,
	VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
// 更新uniform buffer (vetcor4f)
void xUniform4fv(XProgram* program, int location, float* v);
// 加载图片 channel为通道  force_channel为强制通道
unsigned char* LoadImageFromFile(const char* path, int& width, int& height, int& channel, int force_channel
	, bool flipY = false);
// 获取默认的texture
XTexture* xGetDefaultTexture();
// 重新绑定uniform buffer
void xRebindUniformBuffer(XProgram* program, int binding, XUniformBuffer* ubo);
// 重新绑定texture
void xRebindSampler(XProgram* program, int binding, VkImageView iv, VkSampler s,
	VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
// 设置当前的shader程序
void xUseProgram(XProgram* program);
// 绑定veretxbuffer
void xBindVertexBuffer(XBufferObject* vbo);
// 绑定index buffer object
void xBindElementBuffer(XBufferObject* ibo);
// 绘制
void xDrawArrays(VkCommandBuffer commandbuffer, int offset, int count);
// 通过ibo进行绘制
void xDrawElements(VkCommandBuffer commandbuffer, int offset, int count);
// 开始绘制
VkCommandBuffer xBeginRendering(VkCommandBuffer commandbuffer = nullptr);
// 结束绘制
void xEndRendering();
// 交换前后缓冲区
void xSwapBuffers(VkCommandBuffer commandbuffer = nullptr);