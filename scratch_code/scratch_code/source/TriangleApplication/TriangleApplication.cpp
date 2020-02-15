#include "./TriangleApplication.hpp"

/* --private-- */

// 初期化処理
void TriangleApplication::InitVulkan()
{
	// インスタンス生成
	CreateInstance();
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
	/* --Vulkanの終了処理-- */
	vkDestroyInstance(m_vk_instance, nullptr);

	/* --GLFW3の終了処理--*/
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

/* --GLFW-- */
// ウィンドウ初期化
void TriangleApplication::InitWindow()
{
	/* --GLFW3の初期化処理-- */
	glfwInit();
	// OpenGL用のコンテキストを生成しない
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// 現時点では, ウィンドウサイズの変更を考慮しない
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	// ウィンドウ生成
	m_window = glfwCreateWindow(m_window_width, m_window_height, m_window_name.c_str(), nullptr, nullptr);
}

/* --Vulkan-- */
// Vulkanインスタンスを生成
void TriangleApplication::CreateInstance()
{
	// Vulkan Application
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = nullptr;
	app_info.pApplicationName = "Hello Triangle";
	app_info.pEngineName = "No Engine";
	app_info.apiVersion = VK_API_VERSION_1_0;
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	// Vulkan Instance Create
	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pNext = nullptr;
	create_info.pApplicationInfo = &app_info;

	// GLFW
	uint32_t glfw_extension_count = 0;
	const char** glfw_extension;
	glfw_extension = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	// Enable Global Validation Layer
	create_info.enabledExtensionCount = glfw_extension_count;
	create_info.ppEnabledExtensionNames = glfw_extension;
	create_info.enabledLayerCount = 0;

	// Create Vulkan Instance
	if (vkCreateInstance(&create_info, nullptr, &m_vk_instance) != VK_SUCCESS) { throw std::runtime_error("FAILED TO CREATE VULKAN INSTANCE."); }
}

/* --public-- */

// コンストラクタ
// NOTE: デフォルト設定
TriangleApplication::TriangleApplication() :
	m_window(nullptr), m_window_width(800), m_window_height(600), m_window_name("Vulkan"),
	m_vk_instance()
{

}
// NOTE: ウィンドウ設定
TriangleApplication::TriangleApplication(const int& width, const int& height, const std::string& name):
	m_window(nullptr), m_window_width(width), m_window_height(height), m_window_name(name),
	m_vk_instance()
{

}

TriangleApplication::~TriangleApplication()
{

}

void TriangleApplication::Run()
{
	// GLFW3によるウィンドウ生成
	InitWindow();
	// Vulkan初期化
	InitVulkan();
	// メイン処理
	MainLoop();
	// リソース解放
	CleanUp();
}