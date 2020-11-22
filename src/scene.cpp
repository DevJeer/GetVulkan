#include "BVulkan.h"
#include "scene.h"
#include "XVulkan.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"

XProgram* program = nullptr;
VertexBuffer *vbo = nullptr;
IndexBuffer* ibo = nullptr;
UniformBuffer* ubo = nullptr;
XTexture* texture = nullptr;

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
	
	// 创建program
	program = new XProgram;
	GLuint vs, fs;
	int file_len = 0;
	unsigned char* file_content = LoadFileContent("Res/test.vsb", file_len);
	// 创建shader
	xCreateShader(vs, file_content, file_len);
	delete[] file_content;

	file_content = LoadFileContent("Res/test.fsb", file_len);
	// 创建fs shader
	xCreateShader(fs, file_content, file_len);
	delete[] file_content;
	// 绑定shader
	xAttachVertexShader(program, vs);
	xAttachFragmentShader(program, fs);
	// 链接shader
	xLinkProgram(program);

	ubo = new UniformBuffer(kXUniformBufferTypeMatrix);
	ubo->SetSize(8);
	glm::mat4 projection = glm::perspective(60.0f, float(GetViewportWidth()) / float(GetViewportHeight()), 0.1f, 100.0f);
	projection[1][1] *= -1.0f;
	ubo->SetMatrix(2, projection);
	ubo->SubmitData();

	texture = new XTexture;
	texture->mFormat = VK_FORMAT_R8G8B8A8_UNORM;
	int image_width, image_height, channel;
	unsigned char* pixel = LoadImageFromFile("Res/textures/test.bmp", image_width, image_height, channel, 4);
	xGenImage(texture, image_width, image_height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT);
	xSubmitImage2D(texture, image_width, image_height, pixel);
	xGenImageView2D(texture);
	xGenSampler(texture);
	delete[]pixel;
}

void Draw(float deltaTime) {
	static float r = 0.0f;
	static float accTime = 0.0f;
	// 是否需要改变texture
	static bool modifiedTexture = false;
	// 是否需要更新UBO
	static bool modifiedUBO = false;
	r += deltaTime;
	accTime += deltaTime;
	if (r >= 1.0f) {
		r = 0.0f;
	}
	if (accTime > 3.0f) {
		if (modifiedTexture == false) {
			modifiedTexture = true;
			xRebindSampler(program, 4, texture->mImageView, texture->mSampler);
		}
	}
	if (accTime > 2.0f) {
		if (modifiedUBO == false) {
			modifiedUBO = true;
			//xRebindUniformBuffer(program, 1, ubo);
		}
	}
	float color[] = { r,r,r,1.0f };
	aClearColor(0.1f, 0.4f, 0.6f, 1.0f);
	VkCommandBuffer commandbuffer = xBeginRendering();
	xUseProgram(program);
	xBindVertexBuffer(vbo);
	xBindElementBuffer(ibo);
	// 更新veretxbuffer 中 2号vec的颜色
	xUniform4fv(program, 2, color);
	//xDrawArrays(commandbuffer, 0, 3);
	xDrawElements(commandbuffer, 0, 3);
	xEndRendering();
	// 交换前后缓冲区 需要指定哪一个commandBuffer
	xSwapBuffers();
}

void OnViewportChanged(int width, int height) {
	aViewport(width, height);
}

void OnQuit() {
	if (program != nullptr) {
		delete program;
	}
	// 释放ubo的资源
	if (ubo != nullptr) {
		delete ubo;
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
	xVulkanCleanUp();
}