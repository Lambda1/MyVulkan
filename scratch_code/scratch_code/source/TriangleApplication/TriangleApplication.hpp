#ifndef __TRIANGLE_APPLICATION_HPP__
#define __TRIANGLE_APPLICATION_HPP__

/*
	Vulkanチュートリアル: 三角形描画
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

// 拡張機能情報の表示
#define DISPLAY_VULKAN_EXTENSION true
// バリデーションレイヤーの詳細表示
#define DISPLAY_VULKAN_VALIDATION_LAYER_DETAIL true
// 物理デバイス情報の表示
#define DISPLAY_VULKAN_PHYSICAL_DEVICE_DETAIL true

// shaderパス
// NOTE: とりあえずハードコーディング
#define VERT_PATH "../../scratch_code/shader/vert.spv"
#define FRAG_PATH "../../scratch_code/shader/frag.spv"

#define NDEBUG

class TriangleApplication
{
private:
	// ウィンドウ関係
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
	VkPhysicalDevice m_physical_device; // 自動解放対象
	// Vulkan: Logical Device
	VkDevice m_logical_device;
	// Vulkan: Queue
	VkQueue m_graphics_queue; // 論理デバイスと共に解放(自動解放)
	VkQueue m_present_queue;  // 自動解放対象
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
	// Vulkan初期化
	void InitVulkan();
	// プログラム中のMain
	void MainLoop();
	// リソース解放
	void CleanUp();

	/*--GLFW関係--*/
	// ウィンドウ初期化
	void InitWindow();
	// 拡張機能設定
	std::vector<const char*> RequiredExtensionGLFW();

	/*--Vulkan関係-*/
	// インスタンス生成
	void CreateInstance();
	// サーフェス生成
	void CreateSurface();
	// デバッグ機能設定
	void SetupDebugMessanger();
	void DefaultDebugSetting(VkDebugUtilsMessengerCreateInfoEXT &);
	// 物理デバイス設定
	void PickUpPhysicalDevice();
	bool isDeviceSuitable(const VkPhysicalDevice &device);
	QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device);
	// 論理デバイス設定
	void CreateLogicalDevice();
	// スワップチェーン設定
	void CreateSwapChain();
	bool CheckDeviceExtensionSupport(const VkPhysicalDevice &device);
	SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice &device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available_formats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &available_present_modes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	// ImageView設定
	void CreateImageViews();
	
	// GraphichsPipline
	void CreateGraphicsPipeline();
	VkShaderModule CreateShaderModule(const std::vector<char> &byte_code);
	// RenderPass
	void CreateRenderPass();

	// Debug: 詳細情報表示関係
	// 拡張機能表示
	void CheckExtension(const std::vector<const char*> &glfw_extensions);
	// Validation Layerのサポート判定
	bool CheckValidationLayerSupport();
	// 物理デバイス情報の表示
	void CheckPhysicalDeviceInfo(const VkPhysicalDeviceProperties &prop, const VkPhysicalDeviceFeatures &feature);
public:
	TriangleApplication();
	TriangleApplication(const int &window_width, const int &window_height, const std::string &window_name);
	~TriangleApplication();
	// 実行
	// NOTE: 初期化 -> MainLoop -> リソース解放
	void Run();
};

#endif