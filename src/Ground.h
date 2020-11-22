#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture2D.h"
#include "Material.h"
class Ground {
public:
	VertexBuffer* mVertexBuffer;
	IndexBuffer* mIndexBuffer;
	Material* mMaterial;
public:
	~Ground();
	void Init();
	// 绘制地面
	void Draw(VkCommandBuffer commandbuffer);
	// 设置地面的材质
	void SetMaterial(Material* material);
};