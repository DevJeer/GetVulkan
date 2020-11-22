#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture2D.h"
#include "Material.h"
class Model {
public:
	VertexBuffer* mVertexBuffer;
	IndexBuffer* mIndexBuffer;
	Material* mMaterial;
public:
	~Model();
	// ��ʼ��ģ��
	void Init(const char* path);
	// ����ģ��
	void Draw(VkCommandBuffer commandbuffer);
	// ָ��ģ�͵Ĺ���
	void SetMaterial(Material* material);
};