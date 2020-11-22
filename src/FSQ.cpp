#include "FSQ.h"

FSQ::~FSQ() {
	delete mVertexBuffer;
}

void FSQ::Init() {
	mVertexBuffer = new VertexBuffer;
	mVertexBuffer->SetSize(4);
	mVertexBuffer->SetPosition(0, -1.0f, 1.0f, 0.0f);
	mVertexBuffer->SetTexcoord(0, 0.0f, 0.0f);
	mVertexBuffer->SetPosition(1, 1.0f, 1.0f, 0.0f);
	mVertexBuffer->SetTexcoord(1, 1.0f, 0.0f);
	mVertexBuffer->SetPosition(2, -1.0f, -1.0f, 0.0f);
	mVertexBuffer->SetTexcoord(2, 0.0f, 1.0f);
	mVertexBuffer->SetPosition(3, 1.0f, -1.0f, 0.0f);
	mVertexBuffer->SetTexcoord(3, 1.0f, 1.0f);
	mVertexBuffer->SubmitData();
}

void FSQ::Draw(VkCommandBuffer commandbuffer) {
	xSetDynamicState(mMaterial->mFixedPipeline, commandbuffer);
	VkBuffer vertexbuffers[] = { mVertexBuffer->mBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindPipeline(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		mMaterial->mFixedPipeline->mPipeline);
	vkCmdBindVertexBuffers(commandbuffer, 0, 1, vertexbuffers, offsets);
	vkCmdBindDescriptorSets(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		mMaterial->mFixedPipeline->mPipelineLayout, 0, 1, &mMaterial->mProgram.mDescriptorSet,
		0, nullptr);
	vkCmdDraw(commandbuffer, 4, 1, 0, 0);
}