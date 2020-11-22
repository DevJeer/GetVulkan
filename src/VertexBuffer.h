#pragma once
#include "XVulkan.h"

class VertexBuffer : public XBufferObject {
public:
	int mVertexCount;
	XVertexData* mVertexes;
public:
	VertexBuffer();
	~VertexBuffer();
	// 设置buffer的大小
	void SetSize(int count);
	// 获取buffer的大小（按字节）
	int GetSize();
	// 设置位置
	void SetPosition(int index, float x, float y, float z, float w = 1.0f);
	// 设置纹理坐标
	void SetTexcoord(int index, float x, float y, float z = 0.0f, float w = 0.0f);
	// 设置法线
	void SetNormal(int index, float x, float y, float z, float w = 0.0f);
	// 设置矩阵
	void SetTangent(int index, float x, float y, float z, float w = 0.0f);
	// 更新数据
	void SubmitData();
};