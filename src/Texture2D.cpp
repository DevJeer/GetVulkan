#include "Texture2D.h"

Texture2D::Texture2D(VkImageAspectFlags imageaspect) :XTexture(imageaspect) {
}
Texture2D::~Texture2D() {
}
void Texture2D::SetImage(const char* path) {
	mFormat = VK_FORMAT_R8G8B8A8_UNORM;
	int image_width, image_height, channel;
	unsigned char* pixel = LoadImageFromFile(path, image_width, image_height, channel, 4);
	xGenImage(this, image_width, image_height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT);
	xSubmitImage2D(this, image_width, image_height, pixel);
	xGenImageView2D(this);
	xGenSampler(this);
	delete[]pixel;
}
void Texture2D::SetFilter(VkFilter min_filter, VkFilter mag_filter) {
	mMinFilter = min_filter;
	mMagFilter = mag_filter;
}
void Texture2D::SetWrapMode(VkSamplerAddressMode wrapu, VkSamplerAddressMode wrapv,
	VkSamplerAddressMode wrapw) {
	mWrapU = wrapu;
	mWrapV = wrapv;
	mWrapW = wrapw;
}