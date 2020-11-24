#include "FrameBuffer.h"
#include "BVulkan.h"
FrameBuffer::FrameBuffer() {
	mFBO = 0;
	mColorBuffer = nullptr;
	mDepthBuffer = nullptr;
}

FrameBuffer::~FrameBuffer() {
	delete mColorBuffer;
	delete mDepthBuffer;
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
	Texture2D* depth_buffer = new Texture2D;
	depth_buffer->mFormat = VK_FORMAT_D24_UNORM_S8_UINT;
	xGenImage(depth_buffer, mWidth, mHeight, depth_buffer->mFormat,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	xGenImageView2D(depth_buffer);
	xGenSampler(depth_buffer);
	mDepthBuffer = depth_buffer;
}

void FrameBuffer::Finish() {
	VkAttachmentDescription attachments[2];
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
	vkCreateFramebuffer(GetVulkanDevice(), &fbci, nullptr, &mFBO);
}