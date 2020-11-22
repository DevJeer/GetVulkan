#include "BVulkan.h"
#include "scene.h"
#include "XVulkan.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "Texture2D.h"
#include "Material.h"
#include "FSQ.h"

VertexBuffer *vbo = nullptr;
IndexBuffer* ibo = nullptr;
Texture2D* texture = nullptr;
Material* test_material = nullptr;
XFixedPipeline* test_pipeline = nullptr;
Material* fsq_material = nullptr;
XFixedPipeline* fsq_pipeline = nullptr;
FSQ* fsq = nullptr;

void Init() {
	xInitDefaultTexture();
	vbo = new VertexBuffer;
	vbo->SetSize(3);
	vbo->SetPosition(0, -0.5f, -0.5f, 0.0f);
	vbo->SetTexcoord(0, 0.0f, 0.0f);
	vbo->SetNormal(0, 1.0f, 0.0f, 1.0f, 0.1f);
	vbo->SetPosition(1, 0.5f, -0.5f, 0.0f);
	vbo->SetTexcoord(1, 1.0f, 0.0f);
	vbo->SetNormal(1, 1.0f, 1.0f, 0.0f, 0.1f);
	vbo->SetPosition(2, 0.0f, 0.5f, 0.0f);
	vbo->SetTexcoord(2, 0.5f, 1.0f);
	vbo->SetNormal(2, 0.0f, 1.0f, 1.0f, 1.0f);
	// 更新顶点数据
	vbo->SubmitData();

	// 创建ibo
	ibo = new IndexBuffer;
	ibo->SetSize(3);
	ibo->AppendIndex(0);
	ibo->AppendIndex(1);
	ibo->AppendIndex(2);
	ibo->SubmitData();

	test_material = new Material;
	test_material->Init("Res/test.vsb", "Res/test.fsb");
	glm::mat4 model = glm::translate(0.0f, 0.0f, -2.0f) * glm::rotate(-30.0f, 0.0f, 1.0f, 0.0f);
	glm::mat4 projection = glm::perspective(45.0f, float(GetViewportWidth()) / float(GetViewportHeight()),
		0.1f, 100.0f);
	projection[1][1] *= -1.0f;
	glm::mat4 view;
	test_material->SetMVP(model, view, projection);
	test_material->SubmitUniformBuffers();

	test_pipeline = new XFixedPipeline;
	xSetColorAttachmentCount(test_pipeline, 1);
	test_pipeline->mRenderPass = GetGlobalRenderPass();
	test_material->SetFixedPipeline(test_pipeline);
	test_pipeline->mViewport = { 0.0f,0.0f,float(GetViewportWidth()),float(GetViewportHeight()),0.0f,1.0f };
	test_pipeline->mScissor = { {0,0},{uint32_t(GetViewportWidth()),uint32_t(GetViewportHeight())} };
	test_material->Finish();


	texture = new Texture2D;
	texture->SetImage("Res/textures/test.bmp");
	test_material->SetTexture(0, texture);


	// 全屏四边形的绘制
	fsq_material = new Material;
	fsq_material->Init("Res/fsq.vsb", "Res/fsq.fsb");
	fsq_material->SubmitUniformBuffers();

	fsq_pipeline = new XFixedPipeline;
	xSetColorAttachmentCount(fsq_pipeline, 1);
	fsq_pipeline->mRenderPass = GetGlobalRenderPass();
	fsq_pipeline->mViewport = { 0.0f,0.0f,float(GetViewportWidth()),float(GetViewportHeight()),0.0f,1.0f };
	fsq_pipeline->mScissor = { {0,0},{uint32_t(GetViewportWidth()),uint32_t(GetViewportHeight())} };
	fsq_pipeline->mInputAssetmlyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	fsq_material->SetFixedPipeline(fsq_pipeline);
	fsq_material->Finish();

	fsq_material->SetTexture(0, texture);
	fsq = new FSQ;
	fsq->Init();
	fsq->mMaterial = fsq_material;
}

void Draw(float deltaTime) {
	aClearColor(0.1f, 0.4f, 0.6f, 1.0f);
	VkCommandBuffer commandbuffer = xBeginRendering();
	xSetDynamicState(test_material->mFixedPipeline, commandbuffer);
	VkBuffer vertexbuffers[] = { vbo->mBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindPipeline(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		test_material->mFixedPipeline->mPipeline);
	vkCmdBindVertexBuffers(commandbuffer, 0, 1, vertexbuffers, offsets);
	vkCmdBindIndexBuffer(commandbuffer, ibo->mBuffer, 0, VK_INDEX_TYPE_UINT32);
	if (test_material->mProgram.mDescriptorSet != 0) {
		vkCmdBindDescriptorSets(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			test_material->mFixedPipeline->mPipelineLayout, 0, 1, &test_material->mProgram.mDescriptorSet,
			0, nullptr);
	}
	vkCmdDrawIndexed(commandbuffer, 3, 1, 0, 0, 0);
	fsq->Draw(commandbuffer);
	xEndRendering();
	// 交换前后缓冲区 需要指定哪一个commandBuffer
	xSwapBuffers();
}

void OnViewportChanged(int width, int height) {
	aViewport(width, height);
}

void OnQuit() {
	if (test_pipeline != nullptr) {
		delete test_pipeline;
	}
	if (fsq_pipeline != nullptr) {
		delete fsq_pipeline;
	}
	// 释放texture的资源
	if (texture != nullptr) {
		delete texture;
	}
	// 销毁vbo
	if (vbo != nullptr) {
		delete vbo;
	}
	// 销毁ibo
	if (ibo != nullptr) {
		delete ibo;
	}
	// 销毁fsq
	if (fsq != nullptr) {
		delete fsq;
	}
	Material::CleanUp();
	xVulkanCleanUp();
}