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

#include "../QueueFamilies/QueueFamilies.hpp"
#include "../VulkanCallBacks/VulkanDebugCallBack_ValidationLayer.hpp"

// �g���@�\���̕\��
#define DISPLAY_VULKAN_EXTENSION true
// �o���f�[�V�������C���[�̏ڍו\��
#define DISPLAY_VULKAN_VALIDATION_LAYER_DETAIL true
// �����f�o�C�X���̕\��
#define DISPLAY_VULKAN_PHYSICAL_DEVICE_DETAIL true

#define NDEBUG

class TriangleApplication
{
private:
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
	// Vulkan: Debug Messanger
	VkDebugUtilsMessengerEXT m_debug_messanger;
	// Vulkan: Physical Device
	VkPhysicalDevice m_physical_device; // ��������Ώ�
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
	// �f�o�b�O�@�\�̐ݒ�
	void SetupDebugMessanger();
	void DefaultDebugSetting(VkDebugUtilsMessengerCreateInfoEXT &);
	// �����f�o�C�X�̐ݒ�
	void PickUpPhysicalDevice();
	bool isDeviceSuitable(const VkPhysicalDevice &device);
	QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device);
	// Debug: �ڍ׏��\���֌W
	// �g���@�\�\��
	void CheckExtension(const std::vector<const char*> &glfw_extensions);
	// Validation Layer�̃T�|�[�g����
	bool CheckValidationLayerSupport();
	// �����f�o�C�X���̕\��
	void CheckPhysicalDeviceInfo(const VkPhysicalDeviceProperties &prop, const VkPhysicalDeviceFeatures &feature);
public:
	TriangleApplication();
	TriangleApplication(const int &window_width, const int &window_height, const std::string &window_name);
	~TriangleApplication();
	// ���s
	// NOTE: ������ -> MainLoop -> ���\�[�X���
	void Run();
};

#endif