#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(XUniformBufferType t) {
	mType = kXBufferObjectTypeUniformBuffer;
	mUniformBufferType = t;
}

UniformBuffer::~UniformBuffer() {
}

int UniformBuffer::GetSize() {
	// 根据当前uniformbuffer的类型来获取内存大小
	if (mUniformBufferType == kXUniformBufferTypeMatrix) {
		return sizeof(XMatrix4x4f) * mMatrices.size();
	}
	return sizeof(XVector4f) * mVector4s.size();
}

void UniformBuffer::SetSize(int count) {
	if (mUniformBufferType == kXUniformBufferTypeMatrix) {
		mMatrices.resize(count);
	}
	else {
		mVector4s.resize(count);
	}
	// 生成空间
	XBufferObject::OnSetSize();
}

void UniformBuffer::SubmitData() {
	if (mUniformBufferType == kXUniformBufferTypeMatrix) {
		XBufferObject::SubmitData(mMatrices.data(), GetSize());
	}
	else {
		XBufferObject::SubmitData(mVector4s.data(), GetSize());
	}
}

void UniformBuffer::SetMatrix(int location, const glm::mat4& m) {
	SetMatrix(location, glm::value_ptr(m));
}

void UniformBuffer::SetMatrix(int location, const float* v) {
	memcpy(mMatrices[location].mData, v, sizeof(XMatrix4x4f));
}

void UniformBuffer::SetVector4(int location, const float* v) {
	memcpy(mVector4s[location].mData, v, sizeof(XVector4f));
}

void UniformBuffer::SetVector4(int location, float x, float y, float z, float w) {
	mVector4s[location].mData[0] = x;
	mVector4s[location].mData[1] = y;
	mVector4s[location].mData[2] = z;
	mVector4s[location].mData[3] = w;
}