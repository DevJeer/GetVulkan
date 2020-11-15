#include "BVulkan.h"
#include "scene.h"
#include "XVulkan.h"

XProgram* program = nullptr;
XBufferObject *vbo = nullptr;

void Init() {
	Vertex vertexes[3];
	vertexes[0].SetPosition(-0.5f, 0.5f, 0.0f);
	vertexes[0].SetTexcoord(1.0f, 0.0f, 1.0f, 1.0f);
	vertexes[1].SetPosition(0.5f, 0.5f, 0.0f);
	vertexes[1].SetTexcoord(1.0f, 1.0f, 0.0f, 1.0f);
	vertexes[2].SetPosition(0.0f, -0.5f, 0.0f);
	vertexes[2].SetTexcoord(0.0f, 1.0f, 1.0f, 1.0f);
	// 创建buffer
	vbo = new XBufferObject;
	// 填充buffer
	xglBufferData(vbo, sizeof(Vertex) * 3, vertexes);
	
	// 创建program
	program = new XProgram;
	GLuint vs, fs;
	int file_len = 0;
	unsigned char* file_content = LoadFileContent("Res/test.vsb", file_len);
	// 创建shader
	aCreateShader(vs, file_content, file_len);
	delete[] file_content;

	file_content = LoadFileContent("Res/test.fsb", file_len);
	// 创建fs shader
	aCreateShader(fs, file_content, file_len);
	delete[] file_content;
	// 绑定shader
	aAttachVertexShader(program, vs);
	aAttachFragmentShader(program, fs);
	// 链接shader
	aLinkProgram(program);
}

void Draw(float deltaTime) {
	aClearColor(0.1f, 0.4f, 0.6f, 1.0f);
	VkCommandBuffer commandbuffer = aBeginRendering();
	glUseProgram(program);
	glBindVertexBuffer(vbo);
	glDrawArrays(A_TRIANGLES, 0, 3);
	aEndRenderingCommand();
	// 交换前后缓冲区
	aSwapBuffers();
}

void OnViewportChanged(int width, int height) {
	aViewport(width, height);
}

void OnQuit() {
	if (program != nullptr) {
		delete program;
	}

	if (vbo != nullptr) {
		glDeleteBufferObject(vbo);
	}
}