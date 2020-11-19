#include "BVulkan.h"
#include "scene.h"
#include "XVulkan.h"

XProgram* program = nullptr;
XBufferObject *vbo = nullptr;
XUniformBuffer* ubo = nullptr;

void Init() {
	xInitDefaultTexture();
	Vertex vertexes[3];
	vertexes[0].SetPosition(-0.5f, -0.5f, -2.0f);
	vertexes[0].SetTexcoord(0.0f, 0.0f);
	vertexes[0].SetNormal(1.0f, 0.0f, 1.0f, 1.0f);
	vertexes[1].SetPosition(0.5f, -0.5f, -2.0f);
	vertexes[1].SetTexcoord(1.0f, 0.0f);
	vertexes[1].SetNormal(1.0f, 1.0f, 0.0f, 1.0f);
	vertexes[2].SetPosition(0.0f, 0.5f, -2.0f);
	vertexes[2].SetTexcoord(0.5f, 1.0f);
	vertexes[2].SetNormal(0.0f, 1.0f, 1.0f, 1.0f);
	// ����buffer
	vbo = new XBufferObject;
	// ���buffer
	xglBufferData(vbo, sizeof(Vertex) * 3, vertexes);
	
	// ����program
	program = new XProgram;
	GLuint vs, fs;
	int file_len = 0;
	unsigned char* file_content = LoadFileContent("Res/test.vsb", file_len);
	// ����shader
	xCreateShader(vs, file_content, file_len);
	delete[] file_content;

	file_content = LoadFileContent("Res/test.fsb", file_len);
	// ����fs shader
	xCreateShader(fs, file_content, file_len);
	delete[] file_content;
	// ��shader
	xAttachVertexShader(program, vs);
	xAttachFragmentShader(program, fs);
	// ����shader
	xLinkProgram(program);

	ubo = new XUniformBuffer;
	ubo->mType = kXUniformBufferTypeMatrix;
	ubo->mMatrices.resize(8);
	glm::mat4 projection = glm::perspective(60.0f, float(GetViewportWidth()) / float(GetViewportHeight()), 0.1f, 100.0f);
	projection[1][1] *= -1.0f;
	memcpy(ubo->mMatrices[2].mData, glm::value_ptr(projection), sizeof(XMatrix4x4f));
	xGenBuffer(ubo->mBuffer, ubo->mMemory, sizeof(XMatrix4x4f) * 8,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	xSubmitUniformBuffer(ubo);
}

void Draw(float deltaTime) {
	static float r = 0.0f;
	static float accTime = 0.0f;
	// �Ƿ���Ҫ�ı�texture
	static bool modifiedTexture = false;
	// �Ƿ���Ҫ����UBO
	static bool modifiedUBO = false;
	r += deltaTime;
	accTime += deltaTime;
	if (r >= 1.0f) {
		r = 0.0f;
	}
	if (accTime > 3.0f) {
		if (modifiedTexture == false) {
			modifiedTexture = true;
			int image_width, image_height, channel;
			// ����ͼƬ
			unsigned char* pixel = LoadImageFromFile("Res/textures/test.bmp", image_width, image_height, channel, 4);
			// ����texture
			xSubmitImage2D(xGetDefaultTexture(), image_width, image_height, pixel);
			// �ͷ�pixel�ڴ�
			delete[] pixel;
		}
	}
	if (accTime > 2.0f) {
		if (modifiedUBO == false) {
			modifiedUBO = true;
			xRebindUniformBuffer(program, 1, ubo);
		}
	}
	float color[] = { r,r,r,1.0f };
	aClearColor(0.1f, 0.4f, 0.6f, 1.0f);
	VkCommandBuffer commandbuffer = aBeginRendering();
	glUseProgram(program);
	glBindVertexBuffer(vbo);
	// ����veretxbuffer �� 2��vec����ɫ
	xUniform4fv(program, 2, color);
	glDrawArrays(A_TRIANGLES, 0, 3);
	aEndRenderingCommand();
	// ����ǰ�󻺳���
	aSwapBuffers();
}

void OnViewportChanged(int width, int height) {
	aViewport(width, height);
}

void OnQuit() {
	if (program != nullptr) {
		delete program;
	}
	// �ͷ�ubo����Դ
	if (ubo != nullptr) {
		delete ubo;
	}

	if (vbo != nullptr) {
		glDeleteBufferObject(vbo);
	}
	xVulkanCleanUp();
}