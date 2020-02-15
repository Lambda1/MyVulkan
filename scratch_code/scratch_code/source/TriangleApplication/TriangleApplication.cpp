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
	// Check Validation Layer
	if (m_enable_validation_layer && !CheckValidationLayerSupport()) { throw std::runtime_error("VALIDATION LAYER IS REQUESTED, BUT NOT AVAILABLE."); }

	// Display Vulkan Extension
#if DISPLAY_VULKAN_EXTENSION
	CheckExtension(glfw_extension, static_cast<int>(glfw_extension_count));
#endif
	
}
// Vulkan: 拡張機能のチェック
void TriangleApplication::CheckExtension(const char** glfw_extensions, const int& extensions_nums)
{
	// Vulkan拡張機能数の取得
	uint32_t extension_count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
	// Vulkan拡張機能の取得
	std::vector<VkExtensionProperties> extensions(extension_count);
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
	// Vulkan拡張機能表示
	std::cout << "AVAILABLE VULKAN EXTENSIONS:" << std::endl;
	for (const auto& extension : extensions) { std::cout << "\t" << extension.extensionName << std::endl; }
	std::cout << std::endl;
	// GLFW3がサポートするVulkan拡張機能の表示
	std::cout << "AVAILABLE GLFW VULKAN EXTENSIONS:" << std::endl;
	for (int i = 0; i < extensions_nums; ++i) { std::cout << "\t" << glfw_extensions[i] << std::endl; }
	std::cout << std::endl;
}
// Vulkan: Validation Layerのサポート確認
bool TriangleApplication::CheckValidationLayerSupport()
{
	// レイヤー数取得
	uint32_t layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
	// レイヤー取得
	std::vector<VkLayerProperties> layers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, layers.data());

	// 取得したレイヤーにm_validation_layerが含まれているかチェック
	for (const char* layer_name : m_validation_layer)
	{
		bool is_layer_found = false;
		for (const auto& layer_properties : layers)
		{
			// レイヤー詳細の表示
#ifdef DISPLAY_VULKAN_VALIDATION_LAYER_DETAIL
			std::cout << layer_properties.layerName << ": " << layer_properties.description << std::endl;
			std::cout << "IMPLEMENT VERSION: " << layer_properties.implementationVersion << std::endl;
			std::cout << "SPECIFICATION VERSION: " << layer_properties.specVersion << std::endl << std::endl;
#endif
			// レイヤーが存在する場合, 次を走査
			if (std::string(layer_name) == std::string(layer_properties.layerName)) { is_layer_found = true; break; }
		}
		// レイヤーが存在しない場合, レイヤー名を表示して走査を終了
		if (!is_layer_found)
		{
			std::cout << "NOT FOUND VALIDATION LAYER: " << layer_name << std::endl << std::endl;;
			return false;
		}
	}

	return true;
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