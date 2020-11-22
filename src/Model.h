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
	// 初始化模型
	void Init(const char* path);
	// 绘制模型
	void Draw(VkCommandBuffer commandbuffer);
	// 指定模型的管线
	void SetMaterial(Material* material);
};