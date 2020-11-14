#include<windows.h>
#include "BVulkan.h"
#include "scene.h"
#pragma comment(lib, "winmm.lib")

LRESULT CALLBACK LearnWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_SIZE: {
		RECT rect;
		GetClientRect(hwnd, &rect);
		// ȡ����vulkan��gdc����
		OnViewportChanged(rect.right - rect.left, rect.bottom - rect.top);
	}
				break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	// ע�ᴰ��
	// ��������
	// ��ʾ����
	// ��Ϣ���� ���û�������

	WNDCLASSEX wndclassex;
	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.cbClsExtra = 0;
	wndclassex.cbWndExtra = 0;
	wndclassex.hbrBackground = nullptr;
	wndclassex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndclassex.hIcon = nullptr;
	wndclassex.hIconSm = nullptr;
	wndclassex.hInstance = hInstance;
	wndclassex.lpfnWndProc = LearnWindowProc;
	wndclassex.lpszMenuName = nullptr;
	wndclassex.lpszClassName = L"LearnVulkanWindow";
	wndclassex.style = CS_VREDRAW | CS_HREDRAW;
	ATOM atom = RegisterClassEx(&wndclassex);
	if (atom == 0) {
		MessageBox(NULL, L"ע�ᴰ��ʧ��", L"����", MB_OK);
		return -1;
	}
	RECT rect = { 0,0,1280,720 };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	HWND hwnd = CreateWindowEx(NULL, L"LearnVulkanWindow", L"GetVulkanWindow", WS_OVERLAPPEDWINDOW,
		0, 0, rect.right - rect.left, rect.bottom - rect.top,
		nullptr, nullptr, hInstance, nullptr);
	// ��ʼ��vulkan
	InitVulkan(hwnd, 1280, 720);
	// ��ʼ������
	Init();
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	MSG msg;
	float last_time = timeGetTime() / 1000.0f;
	while (true) {
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		float current_time = timeGetTime() / 1000.0f;
		float deltaTime = current_time - last_time;
		last_time = current_time;
		// ���Ƴ���
		Draw(deltaTime);
	}
	// �˳�
	OnQuit();
	// ����vulkan��ص���Դ
	VulkanCleanUp();
	return 0;
}