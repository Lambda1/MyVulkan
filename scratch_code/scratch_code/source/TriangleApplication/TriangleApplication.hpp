#ifndef __TRIANGLE_APPLICATION_HPP__
#define __TRIANGLE_APPLICATION_HPP__

/*
	Vulkan�`���[�g���A��: �O�p�`�`��
*/

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class TriangleApplication
{
	// �E�B���h�E�֌W
	GLFWwindow* m_window;
	const int m_window_width, m_window_height;
	const std::string m_window_name;
private:
	// Vulkan������
	void InitVulkan();
	// �v���O��������Main
	void MainLoop();
	// ���\�[�X���
	void CleanUp();
public:
	TriangleApplication();
	TriangleApplication(const int &window_width, const int &window_height, const std::string &window_name);
	~TriangleApplication();
	// ���s
	// NOTE: ������ -> MainLoop -> ���\�[�X���
	void Run();
};

#endif