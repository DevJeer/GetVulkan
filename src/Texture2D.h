#pragma once
#include "XVulkan.h"
class Texture2D : public XTexture {
public:
	Texture2D(VkImageAspectFlags imageaspect = VK_IMAGE_ASPECT_COLOR_BIT);
	~Texture2D();
	// ����texture ����Ϊtexture���ٿռ� ������������ݵĸ���
	void SetImage(const char* path);
	// ���ù��˷�ʽ
	void SetFilter(VkFilter min_filter, VkFilter mag_filter);
	// ���ñ߽��ظ���ģʽ
	void SetWrapMode(VkSamplerAddressMode wrapu, VkSamplerAddressMode wrapv, VkSamplerAddressMode wrapw);
};

class TextureCube : public XTexture {
public:
	TextureCube();
	~TextureCube();
	// ��ʼ����պ�
	void Init(const char* path);
	// ���ù��˷�ʽ
	void SetFilter(VkFilter min_filter, VkFilter mag_filter);
	// ���ñ߽��ظ���ʽ
	void SetWrapMode(VkSamplerAddressMode wrapu, VkSamplerAddressMode wrapv, VkSamplerAddressMode wrapw);
};