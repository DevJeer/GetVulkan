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

// ����vbo������
enum XBufferObjectType {
	kXBufferObjectTypeVertexBuffer,
	kXBufferObjectTypeIndexBuffer,
	kXBufferObjectTypeUniformBuffer,
	kXBufferObjectTypeCount
};

struct XBufferObject {
	VkBuffer mBuffer;
	VkDeviceMemory mMemory;
	XBufferObjectType mType;
	XBufferObject();
	virtual ~XBufferObject();
	// ���ٿռ�
	void OnSetSize();
	// ��������
	void SubmitData(const void* data, int size);
	virtual int GetSize();
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

// ��������
struct XVertexData {
	float mPosition[4];
	float mTexcoord[4];
	float mNormal[4];
	float mTangent[4];
	void SetPosition(float x, float y, float z, float w = 1.0f);
	void SetTexcoord(float x, float y, float z = 0.0f, float w = 0.0f);
	void SetNormal(float x, float y, float z, float w = 0.0f);
	void SetTangent(float x, float y, float z, float w = 0.0f);
	// ����vbo binding����Ϣ
	static const VkVertexInputBindingDescription& BindingDescription();
	// ����vbo�ڲ���������֯��ʽ attribute
	static const std::vector<VkVertexInputAttributeDescription>& AttributeDescriptions();
};

enum XUniformBufferType {
	kXUniformBufferTypeMatrix,
	kXUniformBufferTypeVector,
	kXUniformBufferTypeCount
};

// uniform buffer
struct XUniformBuffer {
	VkBuffer mBuffer;
	VkDeviceMemory mMemory;
	XUniformBufferType mType;
	std::vector<XMatrix4x4f> mMatrices;
	std::vector<XVector4f> mVector4s;
	XUniformBuffer();
	~XUniformBuffer();
};

// �̶�����
struct XFixedPipeline {
	VkPipeline mPipeline;
	VkPipelineLayout mPipelineLayout;
	VkDescriptorSetLayout* mDescriptorSetLayout;
	VkPipelineShaderStageCreateInfo* mShaderStages;
	int mShaderStageCount, mDescriptorSetLayoutCount;
	VkRenderPass mRenderPass;
	VkSampleCountFlagBits mSampleCount;
	VkPipelineInputAssemblyStateCreateInfo mInputAssetmlyState;
	VkPipelineViewportStateCreateInfo mViewportState;
	VkViewport mViewport;
	VkRect2D mScissor;
	VkPipelineRasterizationStateCreateInfo mRasterizer;
	VkPipelineDepthStencilStateCreateInfo mDepthStencilState;
	VkPipelineMultisampleStateCreateInfo mMultisampleState;
	std::vector<VkPipelineColorBlendAttachmentState> mColorBlendAttachmentStates;
	VkPipelineColorBlendStateCreateInfo mColorBlendState;
	XVector4f mPushConstants[16];
	int mPushConstantCount;
	VkShaderStageFlags mPushConstantShaderStage;
	float mDepthConstantFactor, mDepthClamp, mDepthSlopeFactor;
	XFixedPipeline();
	~XFixedPipeline();
	// �ͷ�������ڴ�
	void CleanUp();
};

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
	XFixedPipeline mFixedPipeline;
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
	virtual ~XTexture();
};

// ���vbo bufferdata
void xglBufferData(XVulkanHandle vbo, int size, void* data);
// ����vbo
VkResult xGenBuffer(VkBuffer& buffer, VkDeviceMemory& buffermemory, VkDeviceSize size,
	VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
// ���vbo
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
void xConfigUniformBuffer(XVulkanHandle param, int bingding, XBufferObject* ubo, VkShaderStageFlags shader_stage);
// ����image
void xGenImage(XTexture* texture, uint32_t w, uint32_t h, VkFormat f,
	VkImageUsageFlags usage, VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT, int mipmap = 1);
// ����imageCube
void xGenImageCube(XTexture* texture, uint32_t w, uint32_t h, VkFormat f,
	VkImageUsageFlags usage, VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT, int mipmap = 1);
// ��image��cpu������gpu��
void xSubmitImage2D(XTexture* texture, int width, int height, const void* pixel);
// ��imageCube��cpu������gpu��
void xSubmitImageCube(XTexture* texture, int width, int height, const void* pixel);
// ����image�ķ���ģʽ
void xSetImageLayout(VkCommandBuffer commandbuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
	VkImageSubresourceRange subresourcerange, VkPipelineStageFlags src = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	VkPipelineStageFlags dst = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
// ����ImageView
void xGenImageView2D(XTexture* texture, int mipmap = 1);
// ����ImageCubeView
void xGenImageViewCube(XTexture* texture, int mipmap = 1);
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
// ����ColorAttachmentCount
void xSetColorAttachmentCount(XFixedPipeline* pipeline, int count);
// ����blend�Ŀ�����ر�
void xEnableBlend(XFixedPipeline* pipeline, int attachment, VkBool32 enable);
// ����blend�ķ�ʽ
void xBlend(XFixedPipeline* p, int attachment, VkBlendFactor s_c, VkBlendFactor s_a,
	VkBlendFactor d_c, VkBlendFactor d_a);
// ����blend��ѡ��
void xBlendOp(XFixedPipeline* p, int attachment, VkBlendOp color, VkBlendOp alpha);
// ���ù�դ����ģʽ
void xPolygonMode(XFixedPipeline* p, VkPolygonMode mode);
// ���ƹ�դ���Ŀ�����ر�
void xDisableRasterizer(XFixedPipeline* p, VkBool32 disable);
// �Ƿ�����Ȳ���
void xEnableDepthTest(XFixedPipeline* p, VkBool32 enable);
// ��ʼ�����߲��� �����߲��־�������shader�е����룩
void xInitPipelineLayout(XFixedPipeline* p);
// ����Pipeline
void xCreateFixedPipeline(XFixedPipeline* p);
// ���ù��ߵĶ�̬״̬
void xSetDynamicState(XFixedPipeline* p, VkCommandBuffer commandbuffer);