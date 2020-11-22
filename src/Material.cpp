#include "Material.h"
#include "BVulkan.h"

Material::Material() {
	mVertexMat4UBO = new UniformBuffer(kXUniformBufferTypeMatrix);
	mVertexVec4UBO = new UniformBuffer(kXUniformBufferTypeVector);
	mFragMat4UBO = new UniformBuffer(kXUniformBufferTypeMatrix);
	mFragVec4UBO = new UniformBuffer(kXUniformBufferTypeVector);
}

Material::~Material() {
	delete mVertexMat4UBO;
	delete mVertexVec4UBO;
	delete mFragMat4UBO;
	delete mFragVec4UBO;
}

void Material::Init(const char* vertex, const char* fragment /* = nullptr */) {
	VkShaderModule vs, fs;
	int file_len = 0;
	unsigned char* file_content = LoadFileContent(vertex, file_len);
	xCreateShader(vs, file_content, file_len);
	delete[]file_content;
	mProgram.mShaderStagetCount = 1;
	if (fragment != nullptr) {
		file_content = LoadFileContent(fragment, file_len);
		xCreateShader(fs, file_content, file_len);
		delete[]file_content;
		mProgram.mShaderStagetCount = 2;
	}
	xAttachVertexShader(&mProgram, vs);
	xAttachFragmentShader(&mProgram, fs);
	mVertexMat4UBO->SetSize(8);
	mVertexVec4UBO->SetSize(8);
	mFragMat4UBO->SetSize(8);
	mFragVec4UBO->SetSize(8);
	xConfigUniformBuffer(&mProgram, 0, mVertexVec4UBO, VK_SHADER_STAGE_VERTEX_BIT);
	xConfigUniformBuffer(&mProgram, 1, mVertexMat4UBO, VK_SHADER_STAGE_VERTEX_BIT);
	xConfigUniformBuffer(&mProgram, 2, mFragVec4UBO, VK_SHADER_STAGE_FRAGMENT_BIT);
	xConfigUniformBuffer(&mProgram, 3, mFragMat4UBO, VK_SHADER_STAGE_FRAGMENT_BIT);
	xConfigSampler2D(&mProgram, 4, xGetDefaultTexture()->mImageView, xGetDefaultTexture()->mSampler);
	xConfigSampler2D(&mProgram, 5, xGetDefaultTexture()->mImageView, xGetDefaultTexture()->mSampler);
	xConfigSampler2D(&mProgram, 6, xGetDefaultTexture()->mImageView, xGetDefaultTexture()->mSampler);
	xConfigSampler2D(&mProgram, 7, xGetDefaultTexture()->mImageView, xGetDefaultTexture()->mSampler);
	xConfigSampler2D(&mProgram, 8, xGetDefaultTexture()->mImageView, xGetDefaultTexture()->mSampler);
	xConfigSampler2D(&mProgram, 9, xGetDefaultTexture()->mImageView, xGetDefaultTexture()->mSampler);
	xConfigSampler2D(&mProgram, 10, xGetDefaultTexture()->mImageView, xGetDefaultTexture()->mSampler);
	xConfigSampler2D(&mProgram, 11, xGetDefaultTexture()->mImageView, xGetDefaultTexture()->mSampler);
	xInitDescriptorSetLayout(&mProgram);
	xInitDescriptorPool(&mProgram);
	xInitDescriptorSet(&mProgram);

	// ÔÚmMaterialsÖĞ²åÈë
	mMaterials.insert(this);
}

void Material::SetFixedPipeline(XFixedPipeline* p) {
	mFixedPipeline = p;
}

void Material::SetTexture(int binding, XTexture* texture) {
	xRebindSampler(&mProgram, binding + 4, texture->mImageView, texture->mSampler);
}

void Material::SetMVP(const glm::mat4& m, const glm::mat4& v, const glm::mat4& p) {
	glm::mat4 it_m = glm::inverseTranspose(m);
	mVertexMat4UBO->SetMatrix(0, m);
	mVertexMat4UBO->SetMatrix(1, v);
	mVertexMat4UBO->SetMatrix(2, p);
	mVertexMat4UBO->SetMatrix(3, it_m);
}

void Material::Finish() {
	mFixedPipeline->mDescriptorSetLayout = &mProgram.mDescriptorSetLayout;
	mFixedPipeline->mDescriptorSetLayoutCount = 1;
	mFixedPipeline->mShaderStages = mProgram.mShaderStage;
	mFixedPipeline->mShaderStageCount = mProgram.mShaderStagetCount;
	xInitPipelineLayout(mFixedPipeline);
	xCreateFixedPipeline(mFixedPipeline);
}

void Material::SubmitUniformBuffers() {
	mVertexMat4UBO->SubmitData();
	mVertexVec4UBO->SubmitData();
	mFragMat4UBO->SubmitData();
	mFragVec4UBO->SubmitData();
}

std::set<Material*> Material::mMaterials;

void Material::CleanUp() {
	for (auto iter = mMaterials.begin(); iter != mMaterials.end(); ++iter) {
		delete (*iter);
	}
}