#pragma once
#include "XVulkan.h"

class VertexBuffer : public XBufferObject {
public:
	int mVertexCount;
	XVertexData* mVertexes;
public:
	VertexBuffer();
	~VertexBuffer();
	// ����buffer�Ĵ�С
	void SetSize(int count);
	// ��ȡbuffer�Ĵ�С�����ֽڣ�
	int GetSize();
	// ����λ��
	void SetPosition(int index, float x, float y, float z, float w = 1.0f);
	// ������������
	void SetTexcoord(int index, float x, float y, float z = 0.0f, float w = 0.0f);
	// ���÷���
	void SetNormal(int index, float x, float y, float z, float w = 0.0f);
	// ���þ���
	void SetTangent(int index, float x, float y, float z, float w = 0.0f);
	// ��������
	void SubmitData();
};