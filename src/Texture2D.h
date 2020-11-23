#pragma once
#include "XVulkan.h"
class Texture2D : public XTexture {
public:
	Texture2D(VkImageAspectFlags imageaspect = VK_IMAGE_ASPECT_COLOR_BIT);
	~Texture2D();
	// 设置texture 并且为texture开辟空间 并且完成了数据的更新
	void SetImage(const char* path);
	// 设置过滤方式
	void SetFilter(VkFilter min_filter, VkFilter mag_filter);
	// 设置边界重复的模式
	void SetWrapMode(VkSamplerAddressMode wrapu, VkSamplerAddressMode wrapv, VkSamplerAddressMode wrapw);
};

class TextureCube : public XTexture {
public:
	TextureCube();
	~TextureCube();
	// 初始化天空盒
	void Init(const char* path);
	// 设置过滤方式
	void SetFilter(VkFilter min_filter, VkFilter mag_filter);
	// 设置边界重复方式
	void SetWrapMode(VkSamplerAddressMode wrapu, VkSamplerAddressMode wrapv, VkSamplerAddressMode wrapw);
};