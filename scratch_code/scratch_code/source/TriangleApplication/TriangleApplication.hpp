#ifndef __TRIANGLE_APPLICATION_HPP__
#define __TRIANGLE_APPLICATION_HPP__

/*
	Vulkan�`���[�g���A��: �O�p�`�`��
*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <stdexcept>

class TriangleApplication
{
	// �E�B���h�E�֌W
	GLFWwindow* m_window;
	const int m_window_width, m_window_height;
	const std::string m_window_name;
	// Vulkan�֌W
	VkInstance m_vk_instance;
private:
	// Vulkan������
	void InitVulkan();
	// �v���O��������Main
	void MainLoop();
	// ���\�[�X���
	void CleanUp();

	/*--GLFW�֌W--*/
	void InitWindow();
	/*--Vulkan�֌W-*/
	void CreateInstance();
public:
	TriangleApplication();
	TriangleApplication(const int &window_width, const int &window_height, const std::string &window_name);
	~TriangleApplication();
	// ���s
	// NOTE: ������ -> MainLoop -> ���\�[�X���
	void Run();
};

#endif