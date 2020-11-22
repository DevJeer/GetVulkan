#pragma once
#include "XVulkan.h"
#include "Texture2D.h"
#include "UniformBuffer.h"
class Material {
public:
	// ��Ⱦ�õĳ���
	XProgram mProgram;
	// ����
	XFixedPipeline* mFixedPipeline;
	// vs��fs�е� ubo
	UniformBuffer* mVertexVec4UBO, * mVertexMat4UBO, * mFragVec4UBO, * mFragMat4UBO;
	// �������
	XTexture mTexture[8];
public:
	Material();
	~Material();
	// ��ʼ��shader
	void Init(const char* vs, const char* fs = nullptr);
	// ������Ⱦ����
	void SetFixedPipeline(XFixedPipeline* p);
	// ����texture
	void SetTexture(int binding, XTexture* texture);
	// ����MVP����
	void SetMVP(const glm::mat4& m, const glm::mat4& v, const glm::mat4& p);
	// �����Ⱦ���ߵĴ���
	void Finish();
	// ����uniform buffer����
	void SubmitUniformBuffers();
	// �������
	static std::set<Material*> mMaterials;
	// �ͷ��ڴ�
	static void CleanUp();
};