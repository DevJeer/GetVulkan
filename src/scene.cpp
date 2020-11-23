#include "BVulkan.h"
#include "scene.h"
#include "XVulkan.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "Texture2D.h"
#include "Material.h"
#include "FSQ.h"
#include "Ground.h"
#include "Model.h"

Texture2D* texture = nullptr;
TextureCube* skybox = nullptr;
Material* test_material = nullptr;
XFixedPipeline* test_pipeline = nullptr;
Material* ground_material = nullptr;
XFixedPipeline* ground_pipeline = nullptr;
Material* fsq_material = nullptr;
XFixedPipeline* fsq_pipeline = nullptr;
FSQ* fsq = nullptr;
Ground* ground = nullptr;
Model* sphere = nullptr;

void Init() {
	xInitDefaultTexture();

	// 初始化地面
	ground = new Ground;
	ground->Init();
	glm::vec3 camera_pos(0.0f, 5.0f, 15.0f);
	test_material = new Material;
	test_material->Init("Res/test.vsb", "Res/test.fsb");
	glm::mat4 model;
	glm::mat4 projection = glm::perspective(45.0f, float(GetViewportWidth()) / float(GetViewportHeight()),
		0.1f, 100.0f);
	projection[1][1] *= -1.0f;
	glm::mat4 view = glm::lookAt(camera_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	test_material->SetMVP(model, view, projection);
	test_material->SubmitUniformBuffers();


	test_pipeline = new XFixedPipeline;
	xSetColorAttachmentCount(test_pipeline, 1);
	test_pipeline->mRenderPass = GetGlobalRenderPass();
	test_material->SetFixedPipeline(test_pipeline);
	test_pipeline->mViewport = { 0.0f,0.0f,float(GetViewportWidth()),float(GetViewportHeight()),0.0f,1.0f };
	test_pipeline->mScissor = { {0,0},{uint32_t(GetViewportWidth()),uint32_t(GetViewportHeight())} };
	test_material->Finish();

	ground->SetMaterial(test_material);

	// 绘制球
	sphere = new Model;
	sphere->Init("Res/Sphere.raw");
	sphere->SetMaterial(test_material);

	// 绘制地面
	ground_material = new Material;
	ground_material->Init("Res/ground.vsb", "Res/ground.fsb");
	ground_material->SetMVP(model, view, projection);
	ground_material->mFragVec4UBO->SetVector4(0, 0.0f, 5.0f, 0.0f, 1.0f);
	ground_material->mFragVec4UBO->SetVector4(1, 10.0f, 10.0f, 10.0f, 1.0f);
	ground_material->mFragVec4UBO->SetVector4(2, camera_pos.x, camera_pos.y, camera_pos.z, 1.0f);
	ground_material->SubmitUniformBuffers();

	ground_pipeline = new XFixedPipeline;
	xSetColorAttachmentCount(ground_pipeline, 1);
	ground_pipeline->mRenderPass = GetGlobalRenderPass();
	ground_material->SetFixedPipeline(ground_pipeline);
	ground_pipeline->mViewport = { 0.0f,0.0f,float(GetViewportWidth()),float(GetViewportHeight()),0.0f,1.0f };
	ground_pipeline->mScissor = { {0,0},{uint32_t(GetViewportWidth()),uint32_t(GetViewportHeight())} };
	ground_material->Finish();
	ground->SetMaterial(ground_material);


	texture = new Texture2D;
	texture->SetImage("Res/textures/test.bmp");
	test_material->SetTexture(0, texture);

	skybox = new TextureCube;
	skybox->Init("");


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
	ground->Draw(commandbuffer);
	sphere->Draw(commandbuffer);
	//fsq->Draw(commandbuffer);
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
	if (ground_pipeline != nullptr) {
		delete ground_pipeline;
	}
	// 释放texture的资源
	if (texture != nullptr) {
		delete texture;
	}
	// 销毁fsq
	if (fsq != nullptr) {
		delete fsq;
	}
	// 销毁ground
	if (ground != nullptr) {
		delete ground;
	}
	// 销毁球
	if (sphere != nullptr) {
		delete sphere;
	}
	// 销毁天空盒
	if (skybox != nullptr) {
		delete skybox;
	}
	Material::CleanUp();
	xVulkanCleanUp();
}