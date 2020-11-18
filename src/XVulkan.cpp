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
	glm::mat4 projection = glm::perspective(45.0f, float(GetViewportWidth()) / float(GetViewportHeight()), 0.1f, 100.0f);
	projection[1][1] *= -1.0f;
	// 2号位置为projection 投影矩阵，需要改变
	memcpy(program->mVertexShaderMatrixUniformBuffer.mMatrices[2].mData, glm::value_ptr(projection), sizeof(XMatrix4x4f));
	xGenBuffer(program->mVertexShaderMatrixUniformBuffer.mBuffer, program->mVertexShaderMatrixUniformBuffer.mMemory,
		sizeof(XMatrix4x4f) * 8, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	xSubmitUniformBuffer(&program->mVertexShaderMatrixUniformBuffer);
	xConfigUniformBuffer(program, 1, &program->mVertexShaderMatrixUniformBuffer, VK_SHADER_STAGE_VERTEX_BIT);

	xInitDescriptorSetLayout(program);
	xInitDescriptorPool(program);
	xInitDescriptorSet(program);
	aSetDescriptorSetLayout(&program->mFixedPipeline, &program->mDescriptorSetLayout);
	aSetShaderStage(&program->mFixedPipeline, program->mShaderStage, 2);
	aSetColorAttachmentCount(&program->mFixedPipeline, 1);
	aSetRenderPass(&program->mFixedPipeline, GetGlobalRenderPass());
	program->mFixedPipeline.mViewport = { 0.0f,0.0f,float(GetViewportWidth()),float(GetViewportHeight()) };
	program->mFixedPipeline.mScissor = { {0,0} ,{uint32_t(GetViewportWidth()),uint32_t(GetViewportHeight())} };
	aCreateGraphicPipeline(&program->mFixedPipeline);
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
	VkBufferImageCopy copy = {};
	copy.imageSubresource.aspectMask = texture->mImageAspectFlag;
	copy.imageSubresource.mipLevel = 0;
	copy.imageSubresource.baseArrayLayer = 0;
	copy.imageSubresource.layerCount = 1;

	copy.imageOffset = { 0,0,0 };
	copy.imageExtent = { uint32_t(width),uint32_t(height),1 };
	vkCmdCopyBufferToImage(commandbuffer, tempbuffer, texture->mImage,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
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