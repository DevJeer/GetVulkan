#include "BVulkan.h"
#include "XVulkan.h"
#include "../third-party/stb_image_aug.h"
static XTexture* sDefaultTexture = nullptr;
// 当前的shader program
static XProgram* sCurrentProgram = nullptr;
// 当前的vbo
static XBufferObject* sCurrentVBO = nullptr;
// 当前的ibo
static XBufferObject* sCurrentIBO = nullptr;
// 当前绘制的commandbuffer
static VkCommandBuffer sMainCommandBuffer;
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

void XBufferObject::OnSetSize() {
	if (mType == kXBufferObjectTypeVertexBuffer) {
		xGenVertexBuffer(GetSize(), mBuffer, mMemory);
	}
	else if (mType == kXBufferObjectTypeIndexBuffer) {
		xGenIndexBuffer(GetSize(), mBuffer, mMemory);
	}
}

void XBufferObject::SubmitData(const void* data, int size) {
	if (mType == kXBufferObjectTypeVertexBuffer) {
		xBufferSubVertexData(mBuffer, data, size);
	}
	else if (mType == kXBufferObjectTypeIndexBuffer) {
		xBufferSubIndexData(mBuffer, data, size);
	}
}

int XBufferObject::GetSize() {
	return 0;
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
	mDescriptorSet = 0;
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

XTexture::XTexture(VkImageAspectFlags image_aspect) {
	mImageAspectFlag = image_aspect;
	mImage = 0;
	mImageView = 0;
	mMemory = 0;
	mSampler = 0;
	mInitLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	mTargetLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	mMinFilter = VK_FILTER_LINEAR;
	mMagFilter = VK_FILTER_LINEAR;
	mWrapU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	mWrapV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	mWrapW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	mbEnableAnisotropy = VK_FALSE;
	mMaxAnisotropy = 0.0f;
}

XTexture::~XTexture() {
	if (mMemory != 0) {
		vkFreeMemory(GetVulkanDevice(), mMemory, nullptr);
	}
	if (mImageView != 0) {
		vkDestroyImageView(GetVulkanDevice(), mImageView, nullptr);
	}
	if (mImage != 0) {
		vkDestroyImage(GetVulkanDevice(), mImage, nullptr);
	}
	if (mSampler != 0) {
		vkDestroySampler(GetVulkanDevice(), mSampler, nullptr);
	}
}

XFixedPipeline::XFixedPipeline() {
	mPipelineLayout = 0;
	mPipeline = 0;
	mInputAssetmlyState = {};
	mInputAssetmlyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	mInputAssetmlyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	mInputAssetmlyState.primitiveRestartEnable = VK_FALSE;
	mViewport = {};
	mScissor = {};
	mViewportState = {};
	mViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	mViewportState.viewportCount = 1;
	mViewportState.pViewports = &mViewport;
	mViewportState.scissorCount = 1;
	mViewportState.pScissors = &mScissor;
	mRasterizer = {};
	mRasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	mRasterizer.depthClampEnable = VK_FALSE;
	mRasterizer.rasterizerDiscardEnable = VK_FALSE;
	mRasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	mRasterizer.lineWidth = 1.0f;
	mRasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	mRasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	mRasterizer.depthBiasEnable = VK_TRUE;
	mRasterizer.depthBiasConstantFactor = 0.0f;
	mRasterizer.depthBiasClamp = 0.0f;
	mRasterizer.depthBiasSlopeFactor = 0.0f;
	mDepthStencilState = {};
	mDepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	mDepthStencilState.depthTestEnable = VK_TRUE;
	mDepthStencilState.depthWriteEnable = VK_TRUE;
	mDepthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	mDepthStencilState.depthBoundsTestEnable = VK_FALSE;
	mDepthStencilState.minDepthBounds = 0.0f;
	mDepthStencilState.maxDepthBounds = 1.0f;
	mDepthStencilState.stencilTestEnable = VK_FALSE;
	mDepthStencilState.front = {};
	mDepthStencilState.back = {};
	mMultisampleState = {};
	mMultisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	mMultisampleState.sampleShadingEnable = VK_TRUE;
	mMultisampleState.rasterizationSamples = GetGlobalFrameBufferSampleCount();
	mMultisampleState.minSampleShading = 1.0f;
	mMultisampleState.pSampleMask = nullptr;
	mMultisampleState.alphaToCoverageEnable = VK_FALSE;
	mMultisampleState.alphaToOneEnable = VK_FALSE;
	mColorBlendState = {};
	mColorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	mColorBlendState.logicOpEnable = VK_FALSE;
	mColorBlendState.logicOp = VK_LOGIC_OP_COPY;
	mDescriptorSetLayout = nullptr;
	mShaderStages = nullptr;
	mShaderStageCount = 0;
	mDescriptorSetLayoutCount = 0;
	mRenderPass = 0;
	mSampleCount = GetGlobalFrameBufferSampleCount();
	mPushConstantShaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
	mPushConstantCount = 8;
	mDepthConstantFactor = 0.0f;
	mDepthClamp = 0.0f;
	mDepthSlopeFactor = 0.0f;
}

XFixedPipeline::~XFixedPipeline() {
	CleanUp();
}

void XFixedPipeline::CleanUp() {
	if (mPipeline != 0) {
		vkDestroyPipeline(GetVulkanDevice(), mPipeline, nullptr);
	}
	if (mPipelineLayout != 0) {
		vkDestroyPipelineLayout(GetVulkanDevice(), mPipelineLayout, nullptr);
	}
}

void XVertexData::SetPosition(float x, float y, float z, float w /* = 1.0f */) {
	mPosition[0] = x;
	mPosition[1] = y;
	mPosition[2] = z;
	mPosition[3] = w;
}
void XVertexData::SetTexcoord(float x, float y, float z, float w /* = 1.0f */) {
	mTexcoord[0] = x;
	mTexcoord[1] = y;
	mTexcoord[2] = z;
	mTexcoord[3] = w;
}
void XVertexData::SetNormal(float x, float y, float z, float w /* = 1.0f */) {
	mNormal[0] = x;
	mNormal[1] = y;
	mNormal[2] = z;
	mNormal[3] = w;
}
void XVertexData::SetTangent(float x, float y, float z, float w /* = 1.0f */) {
	mTangent[0] = x;
	mTangent[1] = y;
	mTangent[2] = z;
	mTangent[3] = w;
}

const VkVertexInputBindingDescription& XVertexData::BindingDescription() {
	static VkVertexInputBindingDescription binding_description = {};
	binding_description.binding = 0;
	binding_description.stride = sizeof(XVertexData);
	binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return binding_description;
}

const std::vector<VkVertexInputAttributeDescription>& XVertexData::AttributeDescriptions() {
	static std::vector<VkVertexInputAttributeDescription> attributesDescriptions;
	attributesDescriptions.resize(4);
	// 指明绑定的是哪个vbo
	attributesDescriptions[0].binding = 0;
	// 指明绑定的location
	attributesDescriptions[0].location = 0;
	attributesDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributesDescriptions[0].offset = 0;

	attributesDescriptions[1].binding = 0;
	attributesDescriptions[1].location = 1;
	attributesDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributesDescriptions[1].offset = sizeof(float) * 4;

	attributesDescriptions[2].binding = 0;
	attributesDescriptions[2].location = 2;
	attributesDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributesDescriptions[2].offset = sizeof(float) * 8;

	attributesDescriptions[3].binding = 0;
	attributesDescriptions[3].location = 3;
	attributesDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributesDescriptions[3].offset = sizeof(float) * 12;
	return attributesDescriptions;
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
	// 将gpu中的内存与cpu中的内存关联起来
	vkMapMemory(GetVulkanDevice(), tempmemory, 0, size, 0, &host_memory);
	memcpy(host_memory, data, (size_t)size);
	vkUnmapMemory(GetVulkanDevice(), tempmemory);

	VkCommandBuffer commandbuffer;
	xBeginOneTimeCommandBuffer(&commandbuffer);
	VkBufferCopy copy = { 0, 0, size };
	// 将tempbuffer 拷贝到 buffer中 （vbo之间进行拷贝）
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

void xAttachVertexShader(XProgram* program, VkShaderModule shader) {
	program->mShaderStage[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	program->mShaderStage[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	program->mShaderStage[0].module = shader;
	program->mShaderStage[0].pName = "main";
	program->mVertexShader = shader;
}
void xAttachFragmentShader(XProgram* program, VkShaderModule shader) {
	program->mShaderStage[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	program->mShaderStage[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	program->mShaderStage[1].module = shader;
	program->mShaderStage[1].pName = "main";
	program->mFragmentShader = shader;
}

void xLinkProgram(XProgram* program) {
	// 设置uniform的值
	program->mVertexShaderVectorUniformBuffer.mType = kXUniformBufferTypeVector;
	program->mVertexShaderVectorUniformBuffer.mVector4s.resize(8);
	program->mVertexShaderVectorUniformBuffer.mVector4s[0].mData[0] = 1.0f;
	program->mVertexShaderVectorUniformBuffer.mVector4s[0].mData[1] = 0.0f;
	program->mVertexShaderVectorUniformBuffer.mVector4s[0].mData[2] = 0.0f;
	program->mVertexShaderVectorUniformBuffer.mVector4s[0].mData[3] = 1.0f;
	program->mVertexShaderVectorUniformBuffer.mVector4s[1].mData[0] = 0.0f;
	program->mVertexShaderVectorUniformBuffer.mVector4s[1].mData[1] = 0.0f;
	program->mVertexShaderVectorUniformBuffer.mVector4s[1].mData[2] = 1.0f;
	program->mVertexShaderVectorUniformBuffer.mVector4s[1].mData[3] = 1.0f;
	program->mVertexShaderVectorUniformBuffer.mVector4s[2].mData[0] = 0.0f;
	program->mVertexShaderVectorUniformBuffer.mVector4s[2].mData[1] = 1.0f;
	program->mVertexShaderVectorUniformBuffer.mVector4s[2].mData[2] = 0.0f;
	program->mVertexShaderVectorUniformBuffer.mVector4s[2].mData[3] = 1.0f;
	// 生成对应的buffer
	xGenBuffer(program->mVertexShaderVectorUniformBuffer.mBuffer, program->mVertexShaderVectorUniformBuffer.mMemory,
		sizeof(XVector4f) * 8, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	xSubmitUniformBuffer(&program->mVertexShaderVectorUniformBuffer);
	xConfigUniformBuffer(program, 0, &program->mVertexShaderVectorUniformBuffer, VK_SHADER_STAGE_VERTEX_BIT);
	// 矩阵的uniformbuffer
	program->mVertexShaderMatrixUniformBuffer.mType = kXUniformBufferTypeMatrix;
	program->mVertexShaderMatrixUniformBuffer.mMatrices.resize(8);
	// 模型矩阵 （先旋转后移动）
	glm::mat4 model = glm::translate(0.0f, 0.0f, -2.0f) * glm::rotate(-30.0f, 0.0f, 1.0f, 0.0f);
	glm::mat4 projection = glm::perspective(45.0f, float(GetViewportWidth()) / float(GetViewportHeight()), 0.1f, 100.0f);
	projection[1][1] *= -1.0f;
	// 拷贝0号位置的模型矩阵
	memcpy(program->mVertexShaderMatrixUniformBuffer.mMatrices[0].mData,
		glm::value_ptr(model), sizeof(XMatrix4x4f));
	// 2号位置为projection 投影矩阵，需要改变
	memcpy(program->mVertexShaderMatrixUniformBuffer.mMatrices[2].mData, glm::value_ptr(projection), sizeof(XMatrix4x4f));
	xGenBuffer(program->mVertexShaderMatrixUniformBuffer.mBuffer, program->mVertexShaderMatrixUniformBuffer.mMemory,
		sizeof(XMatrix4x4f) * 8, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	xSubmitUniformBuffer(&program->mVertexShaderMatrixUniformBuffer);
	xConfigUniformBuffer(program, 1, &program->mVertexShaderMatrixUniformBuffer, VK_SHADER_STAGE_VERTEX_BIT);
	// 生成3 4号Uniform
	program->mFragmentShaderVectorUniformBuffer.mType = kXUniformBufferTypeVector;
	program->mFragmentShaderVectorUniformBuffer.mVector4s.resize(8);
	xGenBuffer(program->mFragmentShaderVectorUniformBuffer.mBuffer,
		program->mFragmentShaderVectorUniformBuffer.mMemory,
		sizeof(XVector4f) * 8, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	program->mFragmentShaderMatrixUniformBuffer.mType = kXUniformBufferTypeMatrix;
	program->mFragmentShaderMatrixUniformBuffer.mMatrices.resize(8);
	xGenBuffer(program->mFragmentShaderMatrixUniformBuffer.mBuffer,
		program->mFragmentShaderMatrixUniformBuffer.mMemory,
		sizeof(XMatrix4x4f) * 8, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	xConfigUniformBuffer(program, 2, &program->mFragmentShaderVectorUniformBuffer, VK_SHADER_STAGE_FRAGMENT_BIT);
	xConfigUniformBuffer(program, 3, &program->mFragmentShaderMatrixUniformBuffer, VK_SHADER_STAGE_FRAGMENT_BIT);

	// 纹理 texture 这里纹理不需要初始化是因为在初始的时候已经初始化过了
	xConfigSampler2D(program, 4, sDefaultTexture->mImageView, sDefaultTexture->mSampler);

	xInitDescriptorSetLayout(program);
	xInitDescriptorPool(program);
	xInitDescriptorSet(program);
	// 初始化渲染管线布局
	program->mFixedPipeline.mDescriptorSetLayout = &program->mDescriptorSetLayout;
	program->mFixedPipeline.mDescriptorSetLayoutCount = 1;
	program->mFixedPipeline.mPushConstants[0].mData[1] = 1.0f;
	xInitPipelineLayout(&program->mFixedPipeline);
	aSetShaderStage(&program->mFixedPipeline, program->mShaderStage, 2);
	xSetColorAttachmentCount(&program->mFixedPipeline, 1);
	// 开启aplha混合
	//xEnableBlend(&program->mFixedPipeline, 0, VK_TRUE);
	// 设置blend的方式
	/*xBlend(&program->mFixedPipeline, 0, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_SRC_ALPHA,
		VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE);*/
	//xPolygonMode(&program->mFixedPipeline, VK_POLYGON_MODE_LINE);
	//xEnableDepthTest(&program->mFixedPipeline, VK_TRUE);
	aSetRenderPass(&program->mFixedPipeline, GetGlobalRenderPass());
	program->mFixedPipeline.mViewport = { 0.0f,0.0f,float(GetViewportWidth()),float(GetViewportHeight()) };
	program->mFixedPipeline.mScissor = { {0,0} ,{uint32_t(GetViewportWidth()),uint32_t(GetViewportHeight())} };
	xCreateFixedPipeline(&program->mFixedPipeline);
}

void xInitDescriptorSetLayout(XProgram* program) {
	VkDescriptorSetLayoutCreateInfo dslci = {};
	dslci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dslci.bindingCount = uint32_t(program->mDescriptorSetLayoutBindings.size());
	dslci.pBindings = program->mDescriptorSetLayoutBindings.data();
	vkCreateDescriptorSetLayout(GetVulkanDevice(), &dslci, nullptr, &program->mDescriptorSetLayout);
}

void xInitDescriptorPool(XProgram* program) {
	if (program->mDescriptorPoolSize.empty()) {
		return;
	}
	VkDescriptorPoolCreateInfo dpci = {};
	dpci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	dpci.poolSizeCount = uint32_t(program->mDescriptorPoolSize.size());
	dpci.pPoolSizes = program->mDescriptorPoolSize.data();
	dpci.maxSets = 1;
	vkCreateDescriptorPool(GetVulkanDevice(), &dpci, nullptr, &program->mDescriptorPool);
}

void xInitDescriptorSet(XProgram* program) {
	if (program->mWriteDescriptorSet.empty()) {
		return;
	}
	VkDescriptorSetAllocateInfo dsai = {};
	dsai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	dsai.descriptorPool = program->mDescriptorPool;
	dsai.descriptorSetCount = 1;
	dsai.pSetLayouts = &program->mDescriptorSetLayout;
	vkAllocateDescriptorSets(GetVulkanDevice(), &dsai, &program->mDescriptorSet);
	for (int i = 0; i < program->mWriteDescriptorSet.size(); ++i) {
		program->mWriteDescriptorSet[i].dstSet = program->mDescriptorSet;
	}
	vkUpdateDescriptorSets(GetVulkanDevice(), uint32_t(program->mWriteDescriptorSet.size()), program->mWriteDescriptorSet.data(), 0, nullptr);
}

void xSubmitUniformBuffer(XUniformBuffer* uniformbuffer) {
	void* dst;
	if (uniformbuffer->mType == kXUniformBufferTypeMatrix) {
		int size = sizeof(XMatrix4x4f) * uniformbuffer->mMatrices.size();
		vkMapMemory(GetVulkanDevice(), uniformbuffer->mMemory, 0, size, 0, &dst);
		memcpy(dst, uniformbuffer->mMatrices.data(), size);
	}
	else if (uniformbuffer->mType == kXUniformBufferTypeVector) {
		int size = sizeof(XVector4f) * uniformbuffer->mVector4s.size();
		vkMapMemory(GetVulkanDevice(), uniformbuffer->mMemory, 0, size, 0, &dst);
		memcpy(dst, uniformbuffer->mVector4s.data(), size);
	}
	vkUnmapMemory(GetVulkanDevice(), uniformbuffer->mMemory);
}

void xConfigUniformBuffer(XVulkanHandle param, int bingding, XUniformBuffer* ubo, VkShaderStageFlags shader_stage) {
	XProgram* program = (XProgram*)param;
	// 描述gpu中的uniform绑定点的布局
	VkDescriptorSetLayoutBinding dslb = {};
	dslb.binding = bingding;
	dslb.descriptorCount = 1;
	dslb.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	dslb.stageFlags = shader_stage;
	program->mDescriptorSetLayoutBindings.push_back(dslb);
	// 描述插槽的信息
	VkDescriptorPoolSize dps = {};
	dps.descriptorCount = 1; // 插槽是一个
	dps.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // 插槽的类型的是Uniform_buffer类型的
	program->mDescriptorPoolSize.push_back(dps);
	// 关联gpu中的数据
	VkDescriptorBufferInfo* bufferinfo = new VkDescriptorBufferInfo;
	bufferinfo->offset = 0;
	bufferinfo->buffer = ubo->mBuffer;
	if (ubo->mType == kXUniformBufferTypeMatrix) {
		bufferinfo->range = sizeof(XMatrix4x4f) * ubo->mMatrices.size();
	}
	else {
		bufferinfo->range = sizeof(XVector4f) * ubo->mVector4s.size();
	}
	// cpu中uniform的来源
	VkWriteDescriptorSet descriptorwriter = {};
	descriptorwriter.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorwriter.dstSet = program->mDescriptorSet;
	descriptorwriter.dstBinding = bingding;
	descriptorwriter.dstArrayElement = 0;
	descriptorwriter.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorwriter.descriptorCount = 1;
	descriptorwriter.pBufferInfo = bufferinfo;
	program->mWriteDescriptorSet.push_back(descriptorwriter);
}

void xGenImage(XTexture* texture, uint32_t w, uint32_t h, VkFormat f,
	VkImageUsageFlags usage, VkSampleCountFlagBits sample_count, int mipmap) {
	VkImageCreateInfo ici = {};
	ici.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ici.imageType = VK_IMAGE_TYPE_2D;
	ici.extent = { w,h,1 };
	ici.mipLevels = mipmap;
	ici.arrayLayers = 1;
	ici.format = f;
	ici.initialLayout = texture->mInitLayout;
	ici.usage = usage;
	ici.samples = sample_count;
	if (vkCreateImage(GetVulkanDevice(), &ici, nullptr, &texture->mImage) != VK_SUCCESS) {
		printf("failed to create image\n");
	}
	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(GetVulkanDevice(), texture->mImage, &memory_requirements);
	VkMemoryAllocateInfo mai = {};
	mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mai.allocationSize = memory_requirements.size;
	mai.memoryTypeIndex = xGetMemoryType(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vkAllocateMemory(GetVulkanDevice(), &mai, nullptr, &texture->mMemory);
	// 绑定image 和 物理内存
	vkBindImageMemory(GetVulkanDevice(), texture->mImage, texture->mMemory, 0);
}

void xSubmitImage2D(XTexture* texture, int width, int height, const void* pixel) {
	// 计算图片的大小
	VkDeviceSize image_size = width * height;
	if (texture->mFormat == VK_FORMAT_R8G8B8A8_UNORM) {
		image_size *= 4;
	}
	// 创建临时内存，用来拷贝cpu中的数据
	VkBuffer tempbuffer;
	VkDeviceMemory tempmemory;
	xGenBuffer(tempbuffer, tempmemory, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	void* data;
	// 将数据从pixel 拷贝到 data
	vkMapMemory(GetVulkanDevice(), tempmemory, 0, image_size, 0, &data);
	memcpy(data, pixel, image_size);
	vkUnmapMemory(GetVulkanDevice(), tempmemory);

	// 拷贝数据
	VkCommandBuffer commandbuffer;
	xBeginOneTimeCommandBuffer(&commandbuffer);
	VkImageSubresourceRange subresourcerange = { texture->mImageAspectFlag,0,1,0,1 };
	xSetImageLayout(commandbuffer, texture->mImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourcerange);
	VkBufferImageCopy copy = {};
	copy.imageSubresource.aspectMask = texture->mImageAspectFlag;
	copy.imageSubresource.mipLevel = 0;
	copy.imageSubresource.baseArrayLayer = 0;
	copy.imageSubresource.layerCount = 1;

	copy.imageOffset = { 0,0,0 };
	copy.imageExtent = { uint32_t(width),uint32_t(height),1 };
	vkCmdCopyBufferToImage(commandbuffer, tempbuffer, texture->mImage,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
	xSetImageLayout(commandbuffer, texture->mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourcerange);
	xEndOneTimeCommandBuffer(commandbuffer);
	vkDestroyBuffer(GetVulkanDevice(), tempbuffer, nullptr);
	vkFreeMemory(GetVulkanDevice(), tempmemory, nullptr);
}

void xInitSrcAccessMask(VkImageLayout oldLayout, VkImageMemoryBarrier& barrier) {
	switch (oldLayout) {
	case VK_IMAGE_LAYOUT_UNDEFINED:
		barrier.srcAccessMask = 0;
		break;
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		printf("init src access mask : unprocessed %d\n", oldLayout);
		break;
	}
}

void xInitDstAccessMask(VkImageLayout newLayout, VkImageMemoryBarrier& barrier) {
	switch (newLayout) {
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | barrier.dstAccessMask;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		if (barrier.srcAccessMask == 0) {
			barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		printf("init dst access mask : unprocessed %d\n", newLayout);
		break;
	}
}

void xSetImageLayout(VkCommandBuffer commandbuffer, VkImage image, VkImageLayout oldLayout,
	VkImageLayout newLayout, VkImageSubresourceRange subresourcerange,
	VkPipelineStageFlags src, VkPipelineStageFlags dst) {
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.image = image;
	barrier.subresourceRange = subresourcerange;
	xInitSrcAccessMask(oldLayout, barrier);
	xInitDstAccessMask(newLayout, barrier);
	vkCmdPipelineBarrier(commandbuffer, src, dst, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void xGenImageView2D(XTexture* texture, int mipmap /* = 1 */) {
	VkImageViewCreateInfo ivci = {};
	ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ivci.image = texture->mImage;
	ivci.viewType = VK_IMAGE_VIEW_TYPE_2D;
	ivci.format = texture->mFormat;
	ivci.subresourceRange.aspectMask = texture->mImageAspectFlag;
	ivci.subresourceRange.baseMipLevel = 0;
	ivci.subresourceRange.levelCount = mipmap;
	ivci.subresourceRange.baseArrayLayer = 0;
	ivci.subresourceRange.layerCount = 1;
	vkCreateImageView(GetVulkanDevice(), &ivci, nullptr, &texture->mImageView);
}

void xGenSampler(XTexture* texture) {
	VkSamplerCreateInfo samplercreateinfo = {};
	samplercreateinfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	// 过滤方式
	samplercreateinfo.minFilter = texture->mMinFilter;
	samplercreateinfo.magFilter = texture->mMagFilter;
	// 边界重复的方式
	samplercreateinfo.addressModeU = texture->mWrapU;
	samplercreateinfo.addressModeV = texture->mWrapV;
	samplercreateinfo.addressModeW = texture->mWrapW;
	samplercreateinfo.anisotropyEnable = texture->mbEnableAnisotropy;
	samplercreateinfo.maxAnisotropy = texture->mMaxAnisotropy;
	// 采样到边界使用的颜色
	samplercreateinfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	vkCreateSampler(GetVulkanDevice(), &samplercreateinfo, nullptr, &texture->mSampler);
}

void xInitDefaultTexture() {
	sDefaultTexture = new XTexture;
	sDefaultTexture->mFormat = VK_FORMAT_R8G8B8A8_UNORM;
	unsigned char* pixel = new unsigned char[256 * 256 * 4];
	memset(pixel, 255, sizeof(unsigned char) * 256 * 256 * 4);
	// 生成Image对象
	xGenImage(sDefaultTexture, 256, 256, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT);
	// 将数据copy到gpu中
	xSubmitImage2D(sDefaultTexture, 256, 256, pixel);
	// 生成ImageView
	xGenImageView2D(sDefaultTexture);
	// 生成Sampler对象
	xGenSampler(sDefaultTexture);
	delete[] pixel;
}

void xConfigSampler2D(XProgram* program, int binding, VkImageView imageview, VkSampler sampler,
	VkImageLayout layout) {
	// shader中具体绑定的信息
	VkDescriptorSetLayoutBinding layoutbinding = {};
	layoutbinding.binding = binding;
	layoutbinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	layoutbinding.descriptorCount = 1;
	layoutbinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	layoutbinding.pImmutableSamplers = nullptr;
	program->mDescriptorSetLayoutBindings.push_back(layoutbinding);
	// 存储插槽的信息
	VkDescriptorPoolSize poolsize = {};
	poolsize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolsize.descriptorCount = 1;
	program->mDescriptorPoolSize.push_back(poolsize);
	// ImageInfo
	VkDescriptorImageInfo* imageinfo = new VkDescriptorImageInfo;
	imageinfo->imageLayout = layout;
	imageinfo->imageView = imageview;
	imageinfo->sampler = sampler;
	// cpu中的数据位置
	VkWriteDescriptorSet wds = {};
	wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	wds.dstSet = program->mDescriptorSet;
	wds.dstBinding = binding;
	wds.dstArrayElement = 0;
	wds.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	wds.descriptorCount = 1;
	wds.pImageInfo = imageinfo;
	program->mWriteDescriptorSet.push_back(wds);
}

void xUniform4fv(XProgram* program, int location, float* v) {
	// 先将数据拷贝到对应的location位置上
	memcpy(program->mVertexShaderVectorUniformBuffer.mVector4s[location].mData, v, sizeof(XVector4f));
	// 再提交到gpu中
	xSubmitUniformBuffer(&program->mVertexShaderVectorUniformBuffer);
}

unsigned char* LoadImageFromFile(const char* path, int& width, int& height, int& channel,
	int force_channel, bool flipY) {
	unsigned char* result = stbi_load(path, &width, &height, &channel, force_channel);
	if (result == nullptr) {
		return nullptr;
	}
	// 翻转图片
	// stb中默认左上角为0 0点
	// 翻转的算法，（以后看）
	if (false == flipY) {
		for (int j = 0; j * 2 < height; ++j) {
			int index1 = j * width * channel;
			int index2 = (height - 1 - j) * width * channel;
			for (int i = width * channel; i > 0; --i) {
				unsigned char temp = result[index1];
				result[index1] = result[index2];
				result[index2] = temp;
				++index1;
				++index2;
			}
		}
	}
	return result;
}

XTexture* xGetDefaultTexture() {
	return sDefaultTexture;
}

void xRebindUniformBuffer(XProgram* program, int binding, XUniformBuffer* ubo) {
	VkDescriptorBufferInfo* bufferinfo = new VkDescriptorBufferInfo;
	bufferinfo->buffer = ubo->mBuffer;
	bufferinfo->offset = 0;
	if (ubo->mType == kXUniformBufferTypeMatrix) {
		bufferinfo->range = sizeof(XMatrix4x4f) * ubo->mMatrices.size();
	}
	else {
		bufferinfo->range = sizeof(XVector4f) * ubo->mVector4s.size();
	}
	delete program->mWriteDescriptorSet[binding].pBufferInfo;
	program->mWriteDescriptorSet[binding].pBufferInfo = bufferinfo;
	vkUpdateDescriptorSets(GetVulkanDevice(), uint32_t(program->mWriteDescriptorSet.size()),
		program->mWriteDescriptorSet.data(), 0, nullptr);
}

void xRebindSampler(XProgram* program, int binding, VkImageView iv, VkSampler s,
	VkImageLayout layout) {
	VkDescriptorImageInfo* bufferinfo = new VkDescriptorImageInfo;
	bufferinfo->imageView = iv;
	bufferinfo->imageLayout = layout;
	bufferinfo->sampler = s;
	delete program->mWriteDescriptorSet[binding].pImageInfo;
	program->mWriteDescriptorSet[binding].pImageInfo = bufferinfo;
	vkUpdateDescriptorSets(GetVulkanDevice(), uint32_t(program->mWriteDescriptorSet.size()),
		program->mWriteDescriptorSet.data(), 0, nullptr);
}

void xUseProgram(XProgram* program) {
	sCurrentProgram = program;
}

void xBindVertexBuffer(XBufferObject* vbo) {
	sCurrentVBO = vbo;
}

void xBindElementBuffer(XBufferObject* ibo) {
	sCurrentIBO = ibo;
}

void xDrawArrays(VkCommandBuffer commandbuffer, int offset, int count) {
	aSetDynamicState(&sCurrentProgram->mFixedPipeline, commandbuffer);
	VkBuffer vertexbuffers[] = { sCurrentVBO->mBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindPipeline(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		sCurrentProgram->mFixedPipeline.mPipeline);
	vkCmdBindVertexBuffers(commandbuffer, 0, 1, vertexbuffers, offsets);
	if (sCurrentProgram->mDescriptorSet != 0) {
		vkCmdBindDescriptorSets(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			sCurrentProgram->mFixedPipeline.mPipelineLayout, 0, 1, &sCurrentProgram->mDescriptorSet,
			0, nullptr);
	}
	vkCmdDraw(commandbuffer, count, 1, offset, 0);
}

void xDrawElements(VkCommandBuffer commandbuffer, int offset, int count) {
	xSetDynamicState(&sCurrentProgram->mFixedPipeline, commandbuffer);
	VkBuffer vertexbuffers[] = { sCurrentVBO->mBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindPipeline(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		sCurrentProgram->mFixedPipeline.mPipeline);
	vkCmdBindVertexBuffers(commandbuffer, 0, 1, vertexbuffers, offsets);
	// 添加绑定ibo的方法
	vkCmdBindIndexBuffer(commandbuffer, sCurrentIBO->mBuffer, 0, VK_INDEX_TYPE_UINT32);
	if (sCurrentProgram->mDescriptorSet != 0) {
		vkCmdBindDescriptorSets(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			sCurrentProgram->mFixedPipeline.mPipelineLayout, 0, 1, &sCurrentProgram->mDescriptorSet,
			0, nullptr);
	}
	// 通过索引绘制
	vkCmdDrawIndexed(commandbuffer, count, 1, offset, 0, 0);
}

VkCommandBuffer xBeginRendering(VkCommandBuffer commandbuffer) {
	VkCommandBuffer cmd;
	// 判断是否有传入的commandBuffer
	// 如果没有，就自己创建
	if (commandbuffer != nullptr) {
		cmd = commandbuffer;
	}
	else {
		xBeginOneTimeCommandBuffer(&cmd);
	}
	VkFramebuffer render_target = AquireRenderTarget();
	VkRenderPass render_pass = GetGlobalRenderPass();
	VkClearValue clearvalues[2] = {};
	clearvalues[0].color = { 0.1f,0.4f,0.6f,1.0f };
	clearvalues[1].depthStencil = { 1.0f,0 };

	VkRenderPassBeginInfo rpbi = {};
	rpbi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpbi.framebuffer = render_target;
	rpbi.renderPass = render_pass;
	rpbi.renderArea.offset = { 0,0 };
	rpbi.renderArea.extent = { uint32_t(GetViewportWidth()),uint32_t(GetViewportHeight()) };
	rpbi.clearValueCount = 2;
	rpbi.pClearValues = clearvalues;
	vkCmdBeginRenderPass(cmd, &rpbi, VK_SUBPASS_CONTENTS_INLINE);
	sMainCommandBuffer = cmd;
	return cmd;
}

void xEndRendering() {
	vkCmdEndRenderPass(sMainCommandBuffer);
	vkEndCommandBuffer(sMainCommandBuffer);
}
// 提交绘制指令
static void xSubmitDrawCommand(VkCommandBuffer commandbuffer) {
	VkSemaphore ready_to_render[] = { GetReadyToRenderSemaphore() };
	VkSemaphore ready_to_present[] = { GetReadyToPresentSemaphore() };
	VkSubmitInfo submitinfo = {};
	submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	// 等待可以渲染输出的信息之后再进行渲染
	// 这时候ready_to_render就会被置为1
	// 当渲染完成的时候 ready_to_present就会被置为1
	VkPipelineStageFlags waitstages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitinfo.waitSemaphoreCount = 1;
	submitinfo.pWaitSemaphores = ready_to_render;
	submitinfo.pWaitDstStageMask = waitstages;
	submitinfo.pCommandBuffers = &commandbuffer;
	submitinfo.commandBufferCount = 1;
	submitinfo.signalSemaphoreCount = 1;
	submitinfo.pSignalSemaphores = ready_to_present;
	vkQueueSubmit(GetGraphicQueue(), 1, &submitinfo, VK_NULL_HANDLE);
}
// 显示framebuffer
static void PresentFrameBuffer() {
	VkSemaphore ready_to_present[] = { GetReadyToPresentSemaphore() };
	VkSwapchainKHR swapchain = GetSwapchain();
	VkPresentInfoKHR presentinfo = {};
	// 获取当前渲染的target
	uint32_t current_render_target_index = GetCurrentRenderTargetIndex();
	presentinfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentinfo.pWaitSemaphores = ready_to_present;
	presentinfo.waitSemaphoreCount = 1;
	presentinfo.pSwapchains = &swapchain;
	presentinfo.swapchainCount = 1;
	presentinfo.pImageIndices = &current_render_target_index;
	// 显示队列
	vkQueuePresentKHR(GetPresentQueue(), &presentinfo);
	vkQueueWaitIdle(GetPresentQueue());
}

void xSwapBuffers(VkCommandBuffer commandbuffer) {
	VkCommandBuffer cmd;
	// 当commandbuffer为空时，使用默认的commandbuffer
	if (commandbuffer == nullptr) {
		cmd = sMainCommandBuffer;
	}
	else {
		cmd = commandbuffer;
	}
	xSubmitDrawCommand(cmd);
	PresentFrameBuffer();
	vkFreeCommandBuffers(GetVulkanDevice(), GetCommandPool(), 1, &cmd);
	sMainCommandBuffer = nullptr;
}

void xSetColorAttachmentCount(XFixedPipeline* pipeline, int count) {
	pipeline->mColorBlendAttachmentStates.resize(count);
	// 对每个attachment进行设置
	for (int i = 0; i < count; ++i) {
		pipeline->mColorBlendAttachmentStates[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		pipeline->mColorBlendAttachmentStates[i].blendEnable = VK_FALSE;
		pipeline->mColorBlendAttachmentStates[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		pipeline->mColorBlendAttachmentStates[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		pipeline->mColorBlendAttachmentStates[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		pipeline->mColorBlendAttachmentStates[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		pipeline->mColorBlendAttachmentStates[i].colorBlendOp = VK_BLEND_OP_ADD;
		pipeline->mColorBlendAttachmentStates[i].alphaBlendOp = VK_BLEND_OP_ADD;
	}
}

void xEnableBlend(XFixedPipeline* pipeline, int attachment, VkBool32 enable) {
	pipeline->mColorBlendAttachmentStates[attachment].blendEnable = enable;
}

void xBlend(XFixedPipeline* p, int attachment, VkBlendFactor s_c, VkBlendFactor s_a,
	VkBlendFactor d_c, VkBlendFactor d_a) {
	p->mColorBlendAttachmentStates[attachment].srcColorBlendFactor = s_c;
	p->mColorBlendAttachmentStates[attachment].srcAlphaBlendFactor = s_a;
	p->mColorBlendAttachmentStates[attachment].dstColorBlendFactor = d_c;
	p->mColorBlendAttachmentStates[attachment].dstAlphaBlendFactor = d_a;
}

void xBlendOp(XFixedPipeline* p, int attachment, VkBlendOp color, VkBlendOp alpha) {
	p->mColorBlendAttachmentStates[attachment].colorBlendOp = color;
	p->mColorBlendAttachmentStates[attachment].alphaBlendOp = alpha;
}

void xPolygonMode(XFixedPipeline* p, VkPolygonMode mode) {
	p->mRasterizer.polygonMode = mode;
}
void xDisableRasterizer(XFixedPipeline* p, VkBool32 disable) {
	p->mRasterizer.rasterizerDiscardEnable = disable;
}

void xEnableDepthTest(XFixedPipeline* p, VkBool32 enable) {
	p->mDepthStencilState.depthTestEnable = enable;
}

void xInitPipelineLayout(XFixedPipeline* p) {
	VkPushConstantRange pushconstancrange = {};
	pushconstancrange.stageFlags = p->mPushConstantShaderStage;
	pushconstancrange.offset = 0;
	pushconstancrange.size = sizeof(XVector4f) * p->mPushConstantCount;
	VkPipelineLayoutCreateInfo ci = {};
	ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	ci.pSetLayouts = p->mDescriptorSetLayout;
	ci.setLayoutCount = p->mDescriptorSetLayoutCount;
	ci.pPushConstantRanges = &pushconstancrange;
	ci.pushConstantRangeCount = 1;
	vkCreatePipelineLayout(GetVulkanDevice(), &ci, nullptr, &p->mPipelineLayout);
}

void xCreateFixedPipeline(XFixedPipeline* p) {
	const auto& bindingdescriptions = XVertexData::BindingDescription();
	const auto& attributeDescriptions = XVertexData::AttributeDescriptions();
	VkPipelineVertexInputStateCreateInfo vertexinputinfo = {};
	vertexinputinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexinputinfo.vertexBindingDescriptionCount = 1;
	vertexinputinfo.pVertexBindingDescriptions = &bindingdescriptions;
	vertexinputinfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
	vertexinputinfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	p->mColorBlendState.attachmentCount = p->mColorBlendAttachmentStates.size();
	p->mColorBlendState.pAttachments = p->mColorBlendAttachmentStates.data();
	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH,
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_DEPTH_BIAS
	};
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 4;
	dynamicState.pDynamicStates = dynamicStates;
	VkGraphicsPipelineCreateInfo pipelineinfo = {};
	pipelineinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineinfo.stageCount = p->mShaderStageCount;
	pipelineinfo.pStages = p->mShaderStages;
	pipelineinfo.pVertexInputState = &vertexinputinfo;
	pipelineinfo.pInputAssemblyState = &p->mInputAssetmlyState;
	pipelineinfo.pViewportState = &p->mViewportState;
	pipelineinfo.pRasterizationState = &p->mRasterizer;
	pipelineinfo.pMultisampleState = &p->mMultisampleState;
	pipelineinfo.pDepthStencilState = &p->mDepthStencilState;
	pipelineinfo.pColorBlendState = &p->mColorBlendState;
	pipelineinfo.pDynamicState = &dynamicState;
	pipelineinfo.layout = p->mPipelineLayout;
	pipelineinfo.renderPass = p->mRenderPass;
	pipelineinfo.subpass = 0;
	pipelineinfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineinfo.basePipelineIndex = -1;
	vkCreateGraphicsPipelines(GetVulkanDevice(), VK_NULL_HANDLE, 1, &pipelineinfo, nullptr,
		&p->mPipeline);
}

void xSetDynamicState(XFixedPipeline* p, VkCommandBuffer commandbuffer) {
	vkCmdSetViewport(commandbuffer, 0, 1, &p->mViewport);
	vkCmdSetScissor(commandbuffer, 0, 1, &p->mScissor);
	vkCmdSetDepthBias(commandbuffer, p->mDepthConstantFactor, p->mDepthClamp, p->mDepthSlopeFactor);
	vkCmdPushConstants(commandbuffer, p->mPipelineLayout, p->mPushConstantShaderStage, 0,
		sizeof(XVector4f) * p->mPushConstantCount, p->mPushConstants);
}

void xVulkanCleanUp() {
	if (sDefaultTexture != nullptr) {
		// 释放纹理资源
		delete sDefaultTexture;
	}
}
