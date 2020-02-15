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

// 拡張機能情報の表示
#define DISPLAY_VULKAN_EXTENSION true
// バリデーションレイヤーの詳細表示
#define DISPLAY_VULKAN_VALIDATION_LAYER_DETAIL true

class TriangleApplication
{
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
	// 拡張機能表示
	void CheckExtension(const std::vector<const char*> &glfw_extensions);
	// Validation Layerのサポート判定
	bool CheckValidationLayerSupport();
public:
	TriangleApplication();
	TriangleApplication(const int &window_width, const int &window_height, const std::string &window_name);
	~TriangleApplication();
	// 実行
	// NOTE: 初期化 -> MainLoop -> リソース解放
	void Run();
};

#endif