#include "Model.h"
Model::~Model() {
	delete mVertexBuffer;
	delete mIndexBuffer;
}
void Model::Init(const char* path) {
	mVertexBuffer = new VertexBuffer;
	mIndexBuffer = new IndexBuffer;
	FILE* file = fopen(path, "rb");
	fread(&mVertexBuffer->mVertexCount, 1, sizeof(int), file);
	mVertexBuffer->SetSize(mVertexBuffer->mVertexCount);
	fread(mVertexBuffer->mVertexes, sizeof(XVertexData), mVertexBuffer->mVertexCount, file);
	fread(&mIndexBuffer->mIndexCount, 1, sizeof(int), file);
	mIndexBuffer->SetSize(mIndexBuffer->mIndexCount);
	fread(mIndexBuffer->mIndexes, sizeof(unsigned int), mIndexBuffer->mIndexCount, file);
	fclose(file);
	mVertexBuffer->SubmitData();
	mIndexBuffer->SubmitData();
}
void Model::Draw(VkCommandBuffer commandbuffer) {
	xSetDynamicState(mMaterial->mFixedPipeline, commandbuffer);
	VkBuffer vertexbuffers[] = { mVertexBuffer->mBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindPipeline(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		mMaterial->mFixedPipeline->mPipeline);
	vkCmdBindVertexBuffers(commandbuffer, 0, 1, vertexbuffers, offsets);
	vkCmdBindIndexBuffer(commandbuffer, mIndexBuffer->mBuffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		mMaterial->mFixedPipeline->mPipelineLayout, 0, 1, &mMaterial->mProgram.mDescriptorSet,
		0, nullptr);
	vkCmdDrawIndexed(commandbuffer, mIndexBuffer->mIndexCount, 1, 0, 0, 0);
}
void Model::SetMaterial(Material* material) {
	mMaterial = material;
}