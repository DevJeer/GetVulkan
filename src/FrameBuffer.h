#pragma once
#include "VertexBuffer.h"
#include "Texture2D.h"
#include "Material.h"
class FrameBuffer {
public:
	VkFramebuffer mFBO;
	// renderPass
	VkRenderPass mRenderPass;
	uint32_t mWidth, mHeight;
	std::vector<Texture2D*>mAttachments;
	// 保存清理颜色的值
	std::vector<VkClearValue> mClearValues;
	int mColorBufferCount;
	// 记录深度图的索引
	int mDepthBufferIndex;
	// 深度图
	Texture2D* mDepthBuffer;
public:
	FrameBuffer();
	~FrameBuffer();
	// 设置framebuffer的大小
	void SetSize(uint32_t width, uint32_t height);
	// 生成颜色图
	void AttachColorBuffer(VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);
	// 生成深度图
	void AttachDepthBuffer();
	// 创建framebuffer
	void Finish();
	// 设置颜色图
	void SetClearColor(int index, float r, float g, float b, float a);
	// 设置深度图
	void SetClearDepthStencil(float depth, uint32_t stencil);
	// 使用当前的fbo进行渲染
	VkCommandBuffer BeginRendering(VkCommandBuffer commandbuffer = nullptr);
};