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
	// ����������ɫ��ֵ
	std::vector<VkClearValue> mClearValues;
	int mColorBufferCount;
	// ��¼���ͼ������
	int mDepthBufferIndex;
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
	// ������ɫͼ
	void SetClearColor(int index, float r, float g, float b, float a);
	// �������ͼ
	void SetClearDepthStencil(float depth, uint32_t stencil);
	// ʹ�õ�ǰ��fbo������Ⱦ
	VkCommandBuffer BeginRendering(VkCommandBuffer commandbuffer = nullptr);
};