#ifndef __TRIANGLE_APPLICATION_HPP__
#define __TRIANGLE_APPLICATION_HPP__

/*
	Vulkanチュートリアル: 三角形描画
*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

#include "../QueueFamilies/QueueFamilies.hpp"
#include "../VulkanCallBacks/VulkanDebugCallBack_ValidationLayer.hpp"

// 拡張機能情報の表示
#define DISPLAY_VULKAN_EXTENSION true
// バリデーションレイヤーの詳細表示
#define DISPLAY_VULKAN_VALIDATION_LAYER_DETAIL true
// 物理デバイス情報の表示
#define DISPLAY_VULKAN_PHYSICAL_DEVICE_DETAIL true

#define NDEBUG

class TriangleApplication
{
private:
	// ウィンドウ関係
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
	VkPhysicalDevice m_physical_device; // 自動解放対象
	// Vulkan: Logical Device
	VkDevice m_logical_device;
	VkQueue m_graphics_queue;  // 論理デバイスと共に解放(自動解放)
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
	// デバッグ機能の設定
	void SetupDebugMessanger();
	void DefaultDebugSetting(VkDebugUtilsMessengerCreateInfoEXT &);
	// 物理デバイスの設定
	void PickUpPhysicalDevice();
	bool isDeviceSuitable(const VkPhysicalDevice &device);
	QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device);
	// 論理デバイスの設定
	void CreateLogicalDevice();
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