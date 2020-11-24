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
	int mColorBufferCount;
	// ���ͼ
	Texture2D* mDepthBuffer;
public:
	FrameBuffer();
	~FrameBuffer();
	// ����framebuffer�Ĵ�С
	void SetSize(uint32_t width, uint32_t height);
	// ������ɫͼ
	void AttachColorBuffer(VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);
	// �������ͼ
	void AttachDepthBuffer();
	// ����framebuffer
	void Finish();
};