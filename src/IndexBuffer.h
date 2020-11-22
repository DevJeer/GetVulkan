#pragma once
#include "XVulkan.h"
class IndexBuffer : public XBufferObject {
public:
	int mIndexCount;
	// 用来计数
	int mCurrentIndex;
	unsigned int* mIndexes;
public:
	IndexBuffer();
	~IndexBuffer();
	void SetSize(int count);
	int GetSize();
	// 添加一个index
	void AppendIndex(int index);
	void SubmitData();
};