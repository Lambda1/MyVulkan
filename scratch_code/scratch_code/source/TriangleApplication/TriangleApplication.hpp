#ifndef __TRIANGLE_APPLICATION_HPP__
#define __TRIANGLE_APPLICATION_HPP__

/*
	Vulkan�`���[�g���A��: �O�p�`�`��
*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

// �g���@�\���̕\��
#define DISPLAY_VULKAN_EXTENSION true
// �o���f�[�V�������C���[�̏ڍו\��
#define DISPLAY_VULKAN_VALIDATION_LAYER_DETAIL true

class TriangleApplication
{
	// �E�B���h�E�֌W
	GLFWwindow* m_window;
	const int m_window_width, m_window_height;
	const std::string m_window_name;
	
	// Vulkan: Instance
	VkInstance m_vk_instance;
	// Vulkan: Validation Layer
	const std::vector<const char*> m_validation_layer = { "VK_LAYER_KHRONOS_validation" };
#ifdef NDEBUG
	inline constexpr static bool m_enable_validation_layer = false;
#else
	inline constexpr static bool m_enable_validation_layer = true;
#endif

private:
	// Vulkan������
	void InitVulkan();
	// �v���O��������Main
	void MainLoop();
	// ���\�[�X���
	void CleanUp();

	/*--GLFW�֌W--*/
	// �E�B���h�E������
	void InitWindow();
	// �g���@�\�ݒ�
	std::vector<const char*> RequiredExtensionGLFW();

	/*--Vulkan�֌W-*/
	// �C���X�^���X����
	void CreateInstance();
	// �g���@�\�\��
	void CheckExtension(const std::vector<const char*> &glfw_extensions);
	// Validation Layer�̃T�|�[�g����
	bool CheckValidationLayerSupport();
public:
	TriangleApplication();
	TriangleApplication(const int &window_width, const int &window_height, const std::string &window_name);
	~TriangleApplication();
	// ���s
	// NOTE: ������ -> MainLoop -> ���\�[�X���
	void Run();
};

#endif