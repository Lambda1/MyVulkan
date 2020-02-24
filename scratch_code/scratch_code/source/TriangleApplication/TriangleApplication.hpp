#ifndef __TRIANGLE_APPLICATION_HPP__
#define __TRIANGLE_APPLICATION_HPP__

/*
	Vulkan�`���[�g���A��: �O�p�`�`��
*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cstdint>

#include "../FileReader/FileReader.hpp"
#include "../QueueFamilies/QueueFamilies.hpp"
#include "../SwapChainSupportDetails/SwapChainSupportDetails.hpp"
#include "../VulkanCallBacks/VulkanDebugCallBack_ValidationLayer.hpp"

// �g���@�\���̕\��
#define DISPLAY_VULKAN_EXTENSION true
// �o���f�[�V�������C���[�̏ڍו\��
#define DISPLAY_VULKAN_VALIDATION_LAYER_DETAIL true
// �����f�o�C�X���̕\��
#define DISPLAY_VULKAN_PHYSICAL_DEVICE_DETAIL true

// shader�p�X
// NOTE: �Ƃ肠�����n�[�h�R�[�f�B���O
#define VERT_PATH "../../scratch_code/shader/vert.spv"
#define FRAG_PATH "../../scratch_code/shader/frag.spv"

#define NDEBUG

class TriangleApplication
{
private:
	// �E�B���h�E�֌W
	GLFWwindow* m_window;
	const uint32_t m_window_width, m_window_height;
	const std::string m_window_name;	

	// Vulkan: const
	// Vulkan: Validation Layer
	const std::vector<const char*> m_validation_layer = { "VK_LAYER_KHRONOS_validation" };
	// Vulkan: Swap Chain
	const std::vector<const char*> m_device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#ifdef NDEBUG
	inline constexpr static bool m_enable_validation_layer = false;
#else
	inline constexpr static bool m_enable_validation_layer = true;
#endif
	// Vulkan: Instance
	VkInstance m_vk_instance;
	// Vulkan: Debug Messanger
	VkDebugUtilsMessengerEXT m_debug_messanger;
	// Vulkan: Window Surface
	VkSurfaceKHR m_surface;
	// Vulkan: Physical Device
	VkPhysicalDevice m_physical_device; // ��������Ώ�
	// Vulkan: Logical Device
	VkDevice m_logical_device;
	// Vulkan: Queue
	VkQueue m_graphics_queue; // �_���f�o�C�X�Ƌ��ɉ��(�������)
	VkQueue m_present_queue;  // ��������Ώ�
	// Vulkan: SwapChain
	VkSwapchainKHR m_swap_chain;
	std::vector<VkImage> m_swap_chain_image;
	VkFormat m_swap_chain_image_format;
	VkExtent2D m_swap_chain_extent;
	// Vulkan: ImageView
	std::vector<VkImageView> m_swap_chain_image_views;
	// Vulkan: Pipeline
	VkRenderPass m_render_pass;
	VkPipelineLayout m_pipeline_layout;
	VkPipeline m_pipeline;
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
	// �T�[�t�F�X����
	void CreateSurface();
	// �f�o�b�O�@�\�ݒ�
	void SetupDebugMessanger();
	void DefaultDebugSetting(VkDebugUtilsMessengerCreateInfoEXT &);
	// �����f�o�C�X�ݒ�
	void PickUpPhysicalDevice();
	bool isDeviceSuitable(const VkPhysicalDevice &device);
	QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device);
	// �_���f�o�C�X�ݒ�
	void CreateLogicalDevice();
	// �X���b�v�`�F�[���ݒ�
	void CreateSwapChain();
	bool CheckDeviceExtensionSupport(const VkPhysicalDevice &device);
	SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice &device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available_present_modes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	// ImageView�ݒ�
	void CreateImageViews();
	
	// GraphichsPipline
	void CreateGraphicsPipeline();
	VkShaderModule CreateShaderModule(const std::vector<char> &byte_code);
	// RenderPass
	void CreateRenderPass();

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