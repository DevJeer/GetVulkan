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
// ����
struct XMatrix4x4f {
	float mData[16];
	// ���캯����ʼ��Ϊ��λ����
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
	// �߼�����
	VkImage mImage;
	// �����ڴ�
	VkDeviceMemory mMemory;
	// ������ƵĶ��󣬱���ͨ������������߼�����
	VkImageView mImageView;
	// gpu��shaderʹ�õ�image��image��gpu�еı��֣�
	VkSampler mSampler;
	// ��ʾimageLayout ��ʼ״̬
	VkImageLayout mInitLayout;
	// ����Ҫ��Ϊʲô״̬
	VkImageLayout mTargetLayout;
	// ��pipeline��Դ�׶�
	VkPipelineStageFlags mSrcStage;
	// ����Ҫ���ĸ��׶�ʹ��
	VkPipelineStageFlags mTargetStage;
	// ��ʾ��ɫ�������ͼ
	VkImageAspectFlags mImageAspectFlag;
	// texture�ĸ�ʽ
	VkFormat mFormat;
	// ������˵ķ�ʽ
	VkFilter mMinFilter, mMagFilter;
	// �߽��ظ��ķ�ʽ
	VkSamplerAddressMode mWrapU, mWrapV, mWrapW;
	// Ӳ����ص�ѡ�����msaa?��
	VkBool32 mbEnableAnisotropy;
	float mMaxAnisotropy;
	XTexture(VkImageAspectFlags image_aspect = VK_IMAGE_ASPECT_COLOR_BIT);
	~XTexture();
};

// ���vbo bufferdata
void xglBufferData(XVulkanHandle vbo, int size, void* data);
// ����vbo
VkResult xGenBuffer(VkBuffer& buffer, VkDeviceMemory& buffermemory, VkDeviceSize size,
	VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
// ���fbo
void xBufferSubData(VkBuffer buffer, VkBufferUsageFlags usage, const void* data,
	VkDeviceSize size);

// ����vbo
#define  xGenVertexBuffer(size, buffer, buffermemory)\
	xGenBuffer(buffer, buffermemory, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
// ����ibo
#define xGenIndexBuffer(size,buffer,buffermemory) \
	xGenBuffer(buffer,buffermemory,size,VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_INDEX_BUFFER_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)

#define xBufferSubVertexData(buffer, data, size)\
	xBufferSubData(buffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT|VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, data, size)
// ����ibo���������
#define xBufferSubIndexData(buffer,data,size) \
	xBufferSubData(buffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT|VK_BUFFER_USAGE_INDEX_BUFFER_BIT, data, size);

// ��ȡ�ڴ������
uint32_t xGetMemoryType(uint32_t type_filters, VkMemoryPropertyFlags properties);
// beginCommandbuffer
void xBeginOneTimeCommandBuffer(VkCommandBuffer* commandbuffer);
// endCommandbuffer
void xEndOneTimeCommandBuffer(VkCommandBuffer commandbuffer);
// ����commandbuffer
void xGenCommandBuffer(VkCommandBuffer* commandbuffer, int count, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
// �ȴ������ύ���
void xWaitForCommandFinish(VkCommandBuffer commandbuffer);
// ����shadermodule
// codeΪ���ǵ�shader����
// code_lenΪshader����ĳ���
void xCreateShader(VkShaderModule& shader, unsigned char* code, int code_len);

void xAttachVertexShader(XProgram* program, VkShaderModule shader);

void xAttachFragmentShader(XProgram* program, VkShaderModule shader);
// ����program
void xLinkProgram(XProgram* program);
// ����uniform��GPU�е����ݲ���
void xInitDescriptorSetLayout(XProgram* program);

void xInitDescriptorPool(XProgram* program);

void xInitDescriptorSet(XProgram* program);
// ��uniformBufferд�뵽gpu��
void xSubmitUniformBuffer(XUniformBuffer* uniformbuffer);
// ��uniform cpu->gpu�е�·����ͨ ����ȫһЩ��Ϣ�� 
void xConfigUniformBuffer(XVulkanHandle param, int bingding, XUniformBuffer* ubo, VkShaderStageFlags shader_stage);
// ����image
void xGenImage(XTexture* texture, uint32_t w, uint32_t h, VkFormat f,
	VkImageUsageFlags usage, VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT, int mipmap = 1);
// ��image��cpu������gpu��
void xSubmitImage2D(XTexture* texture, int width, int height, const void* pixel);
// ����image�ķ���ģʽ
void xSetImageLayout(VkCommandBuffer commandbuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
	VkImageSubresourceRange subresourcerange, VkPipelineStageFlags src = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	VkPipelineStageFlags dst = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
// ����ImageView
void xGenImageView2D(XTexture* texture, int mipmap = 1);
// ����sampler ��shader�з��������ʱ��ʹ��
void xGenSampler(XTexture* texture);
// ��ʼ��Ĭ��������ͼ
void xInitDefaultTexture();
// �˳�ʱ�����Ĭ����ͼ����Դ
void xVulkanCleanUp();
// ����sampler2D
void xConfigSampler2D(XProgram* program, int binding, VkImageView imageview, VkSampler sampler,
	VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
// ����uniform buffer (vetcor4f)
void xUniform4fv(XProgram* program, int location, float* v);
// ����ͼƬ channelΪͨ��  force_channelΪǿ��ͨ��
unsigned char* LoadImageFromFile(const char* path, int& width, int& height, int& channel, int force_channel
	, bool flipY = false);
// ��ȡĬ�ϵ�texture
XTexture* xGetDefaultTexture();
// ���°�uniform buffer
void xRebindUniformBuffer(XProgram* program, int binding, XUniformBuffer* ubo);
// ���°�texture
void xRebindSampler(XProgram* program, int binding, VkImageView iv, VkSampler s,
	VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
// ���õ�ǰ��shader����
void xUseProgram(XProgram* program);
// ��veretxbuffer
void xBindVertexBuffer(XBufferObject* vbo);
// ��index buffer object
void xBindElementBuffer(XBufferObject* ibo);
// ����
void xDrawArrays(VkCommandBuffer commandbuffer, int offset, int count);
// ͨ��ibo���л���
void xDrawElements(VkCommandBuffer commandbuffer, int offset, int count);
// ��ʼ����
VkCommandBuffer xBeginRendering(VkCommandBuffer commandbuffer = nullptr);
// ��������
void xEndRendering();
// ����ǰ�󻺳���
void xSwapBuffers(VkCommandBuffer commandbuffer = nullptr);