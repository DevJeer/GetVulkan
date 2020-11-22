#pragma once
#include "VertexBuffer.h"
#include "Texture2D.h"
#include "Material.h"
class FSQ {
public:
	// �洢vbo
	VertexBuffer* mVertexBuffer;
	Material* mMaterial;
public:
	~FSQ();
	// ��ʼ��
	void Init();
	// ����
	void Draw(VkCommandBuffer commandbuffer);
};