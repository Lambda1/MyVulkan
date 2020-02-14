#include "./TriangleApplication.hpp"

// private

// 初期化処理
void TriangleApplication::InitVulkan()
{
	/* --GLFW3の初期化処理-- */
	// OpenGL用のコンテキストを生成しない
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// 現時点では, ウィンドウサイズの変更を考慮しない
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// ウィンドウ生成
	m_window = glfwCreateWindow(m_window_width, m_window_height, m_window_name.c_str(), nullptr, nullptr);
}

// メインループ
// NOTE: GLFWの終了, または, エラーの時に終了
void TriangleApplication::MainLoop()
{
	while (!glfwWindowShouldClose(m_window))
	{
		// イベントチェック
		glfwPollEvents();
	}
}

// 終了処理
void TriangleApplication::CleanUp()
{
	/* --GLFW3の終了処理--*/
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

// public

// コンストラクタ
// NOTE: デフォルト設定
TriangleApplication::TriangleApplication() :
	m_window(nullptr), m_window_width(800), m_window_height(600), m_window_name("Vulkan")
{

}
// NOTE: ウィンドウ設定
TriangleApplication::TriangleApplication(const int& width, const int& height, const std::string& name):
	m_window(nullptr), m_window_width(width), m_window_height(height), m_window_name(name)
{

}

TriangleApplication::~TriangleApplication()
{

}

void TriangleApplication::Run()
{
	InitVulkan();
	MainLoop();
	CleanUp();
}