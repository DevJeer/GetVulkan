#include "FrameBuffer.h"
#include "BVulkan.h"
FrameBuffer::FrameBuffer() {
	mFBO = 0;
	mRenderPass = 0;
	mColorBuffer = nullptr;
	mDepthBuffer = nullptr;
}

FrameBuffer::~FrameBuffer() {
	delete mColorBuffer;
	delete mDepthBuffer;
	if (mRenderPass != 0) {
		vkDestroyRenderPass(GetVulkanDevice(), mRenderPass, nullptr);
	}
	if (mFBO != 0) {
		vkDestroyFramebuffer(GetVulkanDevice(), mFBO, nullptr);
	}
}

void FrameBuffer::SetSize(uint32_t width, uint32_t height) {
	mWidth = width;
	mHeight = height;
}

void FrameBuffer::AttachColorBuffer(VkFormat format /* = VK_FORMAT_R8G8B8A8_UNORM */) {
	mColorBuffer = new Texture2D;
	// 设置颜色图的格式 （这一步是为了延迟渲染做准备 延迟渲染用到的格式是RGB的）
	mColorBuffer->mFormat = format;
	xGenImage(mColorBuffer, mWidth, mHeight, format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT);
	xGenImageView2D(mColorBuffer);
	xGenSampler(mColorBuffer);
}

void FrameBuffer::AttachDepthBuffer() {
	Texture2D* depth_buffer = new Texture2D(VK_IMAGE_ASPECT_DEPTH_BIT);
	depth_buffer->mFormat = VK_FORMAT_D24_UNORM_S8_UINT;
	xGenImage(depth_buffer, mWidth, mHeight, depth_buffer->mFormat,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	xGenImageView2D(depth_buffer);
	xGenSampler(depth_buffer);
	mDepthBuffer = depth_buffer;
}

void FrameBuffer::Finish() {
	// attachment的描述
	VkAttachmentDescription attachments[2];
	attachments[0] = {};
	attachments[0].format = mColorBuffer->mFormat;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachments[1] = {};
	attachments[1].format = mDepthBuffer->mFormat;
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	// subpass是通过reference来关联的
	VkAttachmentReference colorattachment_ref = {};
	colorattachment_ref.attachment = 0;
	colorattachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkAttachmentReference depthattachment_ref = {};
	depthattachment_ref.attachment = 1;
	depthattachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpasses = {};
	subpasses.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses.colorAttachmentCount = 1;
	subpasses.pColorAttachments = &colorattachment_ref;
	subpasses.pDepthStencilAttachment = &depthattachment_ref;

	VkRenderPassCreateInfo rpci = {};
	rpci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	rpci.attachmentCount = 2;
	rpci.pAttachments = attachments;
	rpci.subpassCount = 1;
	rpci.pSubpasses = &subpasses;
	vkCreateRenderPass(GetVulkanDevice(), &rpci, nullptr, &mRenderPass);
	// 用来访问颜色图和深度图
	VkImageView render_targets[2];
	render_targets[0] = mColorBuffer->mImageView;
	render_targets[1] = mDepthBuffer->mImageView;
	VkFramebufferCreateInfo fbci = {};
	fbci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fbci.pAttachments = render_targets;
	fbci.attachmentCount = 2;
	fbci.width = mWidth;
	fbci.height = mHeight;
	// 定义是否是数组
	fbci.layers = 1;
	fbci.renderPass = mRenderPass;
	vkCreateFramebuffer(GetVulkanDevice(), &fbci, nullptr, &mFBO);
}