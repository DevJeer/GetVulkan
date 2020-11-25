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
#include "FrameBuffer.h"

Texture2D* texture = nullptr;
TextureCube* skybox = nullptr;

Material* sphere_material = nullptr;
XFixedPipeline* sphere_pipeline = nullptr;

// 深度图渲染用到的管线
Material* depthrender_material = nullptr;
XFixedPipeline* depthrender_pipeline = nullptr;

Material* ground_material = nullptr;
XFixedPipeline* ground_pipeline = nullptr;

Material* fsq_material = nullptr;
XFixedPipeline* fsq_pipeline = nullptr;

FSQ* fsq = nullptr;
Ground* ground = nullptr;
Model* sphere = nullptr;
FrameBuffer* fbo = nullptr;

void Init() {
	xInitDefaultTexture();
	fbo = new FrameBuffer;
	fbo->SetSize(GetViewportWidth(), GetViewportHeight());
	fbo->AttachColorBuffer();
	fbo->AttachDepthBuffer();
	fbo->Finish();

	glm::vec3 camera_pos(0.0f, 5.0f, 10.0f);
	glm::vec3 light_pos(0.0f, 5.0f, 0.0f);
	glm::mat4 light_view = glm::lookAt(
		light_pos,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)
	);
	
	// 准备球的资源
	sphere_material = new Material;
	sphere_material->Init("Res/Sphere.vsb", "Res/Sphere.fsb");
	glm::mat4 model;
	glm::mat4 projection = glm::perspective(45.0f, float(GetViewportWidth()) / float(GetViewportHeight()),
		0.1f, 100.0f);
	projection[1][1] *= -1.0f;
	glm::mat4 view = glm::lookAt(camera_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	sphere_material->SetMVP(model, view, projection);
	sphere_material->mFragVec4UBO->SetVector4(0, light_pos.x, light_pos.y, light_pos.z, 1.0f);
	sphere_material->mFragVec4UBO->SetVector4(1, 10.0f, 10.0f, 10.0f, 1.0f);
	sphere_material->mFragVec4UBO->SetVector4(2, camera_pos.x, camera_pos.y, camera_pos.z, 1.0f);
	sphere_material->SubmitUniformBuffers();

	sphere_pipeline = new XFixedPipeline;
	xSetColorAttachmentCount(sphere_pipeline, 1);
	// 这块使用我们自己的fbo
	sphere_pipeline->mRenderPass = GetGlobalRenderPass();
	sphere_material->SetFixedPipeline(sphere_pipeline);
	sphere_pipeline->mViewport = { 0.0f,0.0f,float(GetViewportWidth()),float(GetViewportHeight()),0.0f,1.0f };
	sphere_pipeline->mScissor = { {0,0},{uint32_t(GetViewportWidth()),uint32_t(GetViewportHeight())} };
	sphere_material->Finish();

	texture = new Texture2D;
	texture->SetImage("Res/textures/test.bmp");
	//sphere_material->SetTexture(0, texture);

	skybox = new TextureCube;
	skybox->Init("");

	// 绘制球
	sphere = new Model;
	sphere->Init("Res/Sphere.raw");
	sphere_material->SetTexture(0, skybox);
	sphere->SetMaterial(sphere_material);

	// 初始化地面
	ground = new Ground;
	ground->Init();
	// 绘制地面
	ground_material = new Material;
	ground_material->Init("Res/ground.vsb", "Res/ground.fsb");
	ground_material->SetMVP(model, view, projection);
	ground_material->mFragVec4UBO->SetVector4(0, light_pos.x, light_pos.y, light_pos.z, 1.0f);
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


	depthrender_material = new Material;
	depthrender_material->Init("Res/depthrender.vsb", "Res/depthrender.fsb");
	depthrender_material->SetMVP(model, light_view, projection);
	depthrender_material->SubmitUniformBuffers();

	depthrender_pipeline = new XFixedPipeline;
	xSetColorAttachmentCount(depthrender_pipeline, 1);
	depthrender_pipeline->mRenderPass = fbo->mRenderPass;
	depthrender_pipeline->mViewport = { 0.0f,0.0f,float(GetViewportWidth()),float(GetViewportHeight()),0.0f,1.0f };
	depthrender_pipeline->mScissor = { {0,0},{uint32_t(GetViewportWidth()),uint32_t(GetViewportHeight())} };
	depthrender_material->SetFixedPipeline(depthrender_pipeline);
	depthrender_material->Finish();

	// 全屏四边形的绘制
	fsq_material = new Material;
	fsq_material->Init("Res/renderdepth.vsb", "Res/renderdepth.fsb");
	fsq_material->SubmitUniformBuffers();

	fsq_pipeline = new XFixedPipeline;
	xSetColorAttachmentCount(fsq_pipeline, 1);
	fsq_pipeline->mRenderPass = GetGlobalRenderPass();
	fsq_pipeline->mViewport = { 0.0f,0.0f,float(GetViewportWidth()),float(GetViewportHeight()),0.0f,1.0f };
	fsq_pipeline->mScissor = { {0,0},{uint32_t(GetViewportWidth()),uint32_t(GetViewportHeight())} };
	fsq_pipeline->mInputAssetmlyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	fsq_material->SetFixedPipeline(fsq_pipeline);
	fsq_material->Finish();

	//fsq_material->SetTexture(0, texture);
	// 将fbo的颜色图片 当做texture
	fsq_material->SetTexture(0, fbo->mAttachments[1]);
	fsq = new FSQ;
	fsq->Init();
	fsq->mMaterial = fsq_material;
}

void Draw(float deltaTime) {
	aClearColor(0.1f, 0.4f, 0.6f, 1.0f);
	// 将图像渲染到我们创建的vbo上
	VkCommandBuffer commandbuffer = fbo->BeginRendering();
	ground->SetMaterial(depthrender_material);
	sphere->SetMaterial(depthrender_material);
	ground->Draw(commandbuffer);
	sphere->Draw(commandbuffer);
	
	vkCmdEndRenderPass(commandbuffer);
	
	xBeginRendering(commandbuffer);
	ground->SetMaterial(ground_material);
	sphere->SetMaterial(sphere_material);
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
	if (sphere_pipeline != nullptr) {
		delete sphere_pipeline;
	}
	if (fsq_pipeline != nullptr) {
		delete fsq_pipeline;
	}
	if (ground_pipeline != nullptr) {
		delete ground_pipeline;
	}
	if (depthrender_pipeline != nullptr) {
		delete depthrender_pipeline;
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
	// 销毁fbo
	if (fbo != nullptr) {
		delete fbo;
	}
	Material::CleanUp();
	xVulkanCleanUp();
}