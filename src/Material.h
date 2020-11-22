#pragma once
#include "XVulkan.h"
#include "Texture2D.h"
#include "UniformBuffer.h"
class Material {
public:
	// 渲染用的程序
	XProgram mProgram;
	// 管线
	XFixedPipeline* mFixedPipeline;
	// vs和fs中的 ubo
	UniformBuffer* mVertexVec4UBO, * mVertexMat4UBO, * mFragVec4UBO, * mFragMat4UBO;
	// 纹理对象
	XTexture mTexture[8];
public:
	Material();
	~Material();
	// 初始化shader
	void Init(const char* vs, const char* fs = nullptr);
	// 设置渲染管线
	void SetFixedPipeline(XFixedPipeline* p);
	// 设置texture
	void SetTexture(int binding, XTexture* texture);
	// 设置MVP矩阵
	void SetMVP(const glm::mat4& m, const glm::mat4& v, const glm::mat4& p);
	// 完成渲染管线的创建
	void Finish();
	// 更新uniform buffer数据
	void SubmitUniformBuffers();
	// 管理材质
	static std::set<Material*> mMaterials;
	// 释放内存
	static void CleanUp();
};