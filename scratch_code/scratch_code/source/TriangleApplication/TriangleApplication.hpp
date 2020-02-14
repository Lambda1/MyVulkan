#ifndef __TRIANGLE_APPLICATION_HPP__
#define __TRIANGLE_APPLICATION_HPP__

/*
	Vulkanチュートリアル: 三角形描画
*/

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class TriangleApplication
{
	// ウィンドウ関係
	GLFWwindow* m_window;
	const int m_window_width, m_window_height;
	const std::string m_window_name;
private:
	// Vulkan初期化
	void InitVulkan();
	// プログラム中のMain
	void MainLoop();
	// リソース解放
	void CleanUp();
public:
	TriangleApplication();
	TriangleApplication(const int &window_width, const int &window_height, const std::string &window_name);
	~TriangleApplication();
	// 実行
	// NOTE: 初期化 -> MainLoop -> リソース解放
	void Run();
};

#endif