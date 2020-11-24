#include "FrameBuffer.h"
#include "BVulkan.h"
FrameBuffer::FrameBuffer() {
	mFBO = 0;
	mRenderPass = 0;
	mDepthBuffer = nullptr;
}

FrameBuffer::~FrameBuffer() {
	for (auto iter = mAttachments.begin(); iter != mAttachments.end(); ++iter) {
		delete* iter;
	}
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
	Texture2D* color_buffer = new Texture2D;
	// 设置颜色图的格式 （这一步是为了延迟渲染做准备 延迟渲染用到的格式是RGB的）
	color_buffer->mFormat = format;
	xGenImage(color_buffer, mWidth, mHeight, format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT);
	xGenImageView2D(color_buffer);
	xGenSampler(color_buffer);
	// 更新colorAttachment的索引
	mColorBufferCount++;
	mAttachments.push_back(color_buffer);
}

void FrameBuffer::AttachDepthBuffer() {
	Texture2D* depth_buffer = new Texture2D(VK_IMAGE_ASPECT_DEPTH_BIT);
	depth_buffer->mFormat = VK_FORMAT_D24_UNORM_S8_UINT;
	xGenImage(depth_buffer, mWidth, mHeight, depth_buffer->mFormat,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	xGenImageView2D(depth_buffer);
	xGenSampler(depth_buffer);
	mAttachments.push_back(depth_buffer);
	mDepthBuffer = depth_buffer;
}

void FrameBuffer::Finish() {
	std::vector<VkAttachmentDescription> attachments;
	// 深度图和颜色图
	std::vector<VkAttachmentReference> colorattachment_refences;
	VkAttachmentReference depthattachment_ref = {};
	// fbo用到的target
	std::vector<VkImageView> render_targets;

	render_targets.resize(mAttachments.size());
	attachments.resize(mAttachments.size());

	int color_buffer_count = 0;
	// 用来访问颜色图和深度图
	for (size_t i = 0; i < mAttachments.size(); ++i) {
		Texture2D* texture = mAttachments[i];
		if (texture->mImageAspectFlag == VK_IMAGE_ASPECT_COLOR_BIT) {
			color_buffer_count++;
		}
		else if (texture->mImageAspectFlag == VK_IMAGE_ASPECT_DEPTH_BIT) {

		}
		render_targets[i] = texture->mImageView;
	}

	colorattachment_refences.resize(color_buffer_count);
	int color_buffer_index = 0;
	int attachment_point = 0;
	// 为深度图和颜色图生成colorattachmentReference 
	for (size_t i = 0; i < mAttachments.size(); ++i) {
		Texture2D* texture = mAttachments[i];
		if (texture->mImageAspectFlag == VK_IMAGE_ASPECT_COLOR_BIT) {
			attachments[i] = {
				0,
				texture->mFormat,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			};
			colorattachment_refences[color_buffer_index++] = {
				uint32_t(attachment_point++),VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			};
		}
		else if (texture->mImageAspectFlag == VK_IMAGE_ASPECT_DEPTH_BIT) {
			attachments[i] = {
				0,
				texture->mFormat,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			};
			depthattachment_ref.attachment = attachment_point++;
			depthattachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
	}


	VkSubpassDescription subpasses = {};
	subpasses.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses.colorAttachmentCount = colorattachment_refences.size();
	subpasses.pColorAttachments = colorattachment_refences.data();
	subpasses.pDepthStencilAttachment = &depthattachment_ref;

	VkRenderPassCreateInfo rpci = {};
	rpci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	rpci.attachmentCount = attachments.size();
	rpci.pAttachments = attachments.data();
	rpci.subpassCount = 1;
	rpci.pSubpasses = &subpasses;
	vkCreateRenderPass(GetVulkanDevice(), &rpci, nullptr, &mRenderPass);
	VkFramebufferCreateInfo fbci = {};
	fbci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fbci.pAttachments = render_targets.data();
	fbci.attachmentCount = render_targets.size();
	fbci.width = mWidth;
	fbci.height = mHeight;
	// 定义是否是数组
	fbci.layers = 1;
	fbci.renderPass = mRenderPass;
	vkCreateFramebuffer(GetVulkanDevice(), &fbci, nullptr, &mFBO);
}