#pragma once
#include "XVulkan.h"
class UniformBuffer : public XBufferObject {
public:
	std::vector<XMatrix4x4f> mMatrices;
	std::vector<XVector4f> mVector4s;
public:
	// 当前uniformbuffer的类型
	XUniformBufferType mUniformBufferType;
	UniformBuffer(XUniformBufferType t);
	~UniformBuffer();
	// 设置uniform的大小
	void SetSize(int count);
	int GetSize();
	// 更新数据
	void SubmitData();
	void SetMatrix(int location, const float* v);
	void SetMatrix(int location, const glm::mat4& m);
	void SetVector4(int location, const float* v);
	void SetVector4(int location, float x, float y, float z, float w);
};