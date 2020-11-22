#pragma once
#include "VertexBuffer.h"
#include "Texture2D.h"
#include "Material.h"
class FSQ {
public:
	// ¥Ê¥¢vbo
	VertexBuffer* mVertexBuffer;
	Material* mMaterial;
public:
	~FSQ();
	// ≥ı ºªØ
	void Init();
	// ªÊ÷∆
	void Draw(VkCommandBuffer commandbuffer);
};