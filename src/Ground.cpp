#include "Ground.h"

Ground::~Ground() {
	delete mVertexBuffer;
	delete mIndexBuffer;
}

void Ground::Init() {
	mVertexBuffer = new VertexBuffer;
	mVertexBuffer->SetSize(1600);
	mIndexBuffer = new IndexBuffer;
	mIndexBuffer->SetSize(2400);
	for (int z = 0; z < 20; ++z) {
		float zStart = 10.0f - z * 1.0f;
		for (int x = 0; x < 20; ++x) {
			int offset = (x + z * 20) * 4;
			float xStart = x * 1.0f - 10.0f;
			mVertexBuffer->SetPosition(offset, xStart, -1.0f, zStart);
			mVertexBuffer->SetPosition(offset + 1, xStart + 1.0f, -1.0f, zStart);
			mVertexBuffer->SetPosition(offset + 2, xStart, -1.0f, zStart - 1.0f);
			mVertexBuffer->SetPosition(offset + 3, xStart + 1.0f, -1.0f, zStart - 1.0f);
			mVertexBuffer->SetNormal(offset, 0.0f, 1.0f, 0.0f);
			mVertexBuffer->SetNormal(offset + 1, 0.0f, 1.0f, 0.0f);
			mVertexBuffer->SetNormal(offset + 2, 0.0f, 1.0f, 0.0f);
			mVertexBuffer->SetNormal(offset + 3, 0.0f, 1.0f, 0.0f);
			if ((x % 2) ^ (z % 2)) {
				mVertexBuffer->SetTexcoord(offset, 0.1f, 0.1f, 0.1f, 0.1f);
				mVertexBuffer->SetTexcoord(offset + 1, 0.1f, 0.1f, 0.1f, 0.1f);
				mVertexBuffer->SetTexcoord(offset + 2, 0.1f, 0.1f, 0.1f, 0.1f);
				mVertexBuffer->SetTexcoord(offset + 3, 0.1f, 0.1f, 0.1f, 0.1f);
			}
			else {
				mVertexBuffer->SetTexcoord(offset, 0.9f, 0.9f, 0.9f, 0.1f);
				mVertexBuffer->SetTexcoord(offset + 1, 0.9f, 0.9f, 0.9f, 0.1f);
				mVertexBuffer->SetTexcoord(offset + 2, 0.9f, 0.9f, 0.9f, 0.1f);
				mVertexBuffer->SetTexcoord(offset + 3, 0.9f, 0.9f, 0.9f, 0.1f);
			}
			mIndexBuffer->AppendIndex(offset);
			mIndexBuffer->AppendIndex(offset + 1);
			mIndexBuffer->AppendIndex(offset + 3);
			mIndexBuffer->AppendIndex(offset);
			mIndexBuffer->AppendIndex(offset + 3);
			mIndexBuffer->AppendIndex(offset + 2);
		}
	}
	mVertexBuffer->SubmitData();
	mIndexBuffer->SubmitData();
}

void Ground::Draw(VkCommandBuffer commandbuffer) {
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

void Ground::SetMaterial(Material* material) {
	mMaterial = material;
}