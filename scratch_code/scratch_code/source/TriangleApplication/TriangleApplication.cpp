#include "./TriangleApplication.hpp"

/* --private-- */

// 初期化処理
void TriangleApplication::InitVulkan()
{
	// インスタンス生成
	CreateInstance();
	// デバッグメッセンジャの設定
	SetupDebugMessanger();
	// ウィンドウサーフェス生成
	CreateSurface();
	// 物理デバイスの設定
	PickUpPhysicalDevice();
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
	// デバッグメッセンジャ破棄
	if (m_enable_validation_layer) { Destroy_Debug_Utils_Messenger_EXT(m_vk_instance, m_debug_messanger, nullptr); }
	// ウィンドウサーフェスの破棄
	vkDestroySurfaceKHR(m_vk_instance, m_surface, nullptr);
	// インスタンス破棄
	vkDestroyInstance(m_vk_instance, nullptr);
	vkDestroyDevice(m_logical_device, nullptr);

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
// 拡張機能設定
std::vector<const char*> TriangleApplication::RequiredExtensionGLFW()
{
	uint32_t glfw_extension_count = 0;
	const char** glfw_extension;
	
	// 拡張機能の取得
	glfw_extension = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	std::vector<const char*> extensions(glfw_extension, glfw_extension + glfw_extension_count);
	if (m_enable_validation_layer) { extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); }

	return extensions;
}

/* --Vulkan-- */
// Vulkanインスタンスを生成
void TriangleApplication::CreateInstance()
{
	// Check Validation Layer
	if (!CheckValidationLayerSupport() && m_enable_validation_layer) { throw std::runtime_error("VALIDATION LAYER IS REQUESTED, BUT NOT AVAILABLE."); }

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
	create_info.pApplicationInfo = &app_info;

	// GLFW
	auto extensions = RequiredExtensionGLFW();
	// Display Vulkan Extension
#if DISPLAY_VULKAN_EXTENSION
	CheckExtension(extensions);
#endif
	// Enable Global Validation Layer
	create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	create_info.ppEnabledExtensionNames = extensions.data();

	// Debug Setting
	VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
	if (m_enable_validation_layer)
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layer.size());
		create_info.ppEnabledLayerNames = m_validation_layer.data();
		DefaultDebugSetting(debug_create_info);
		create_info.pNext = reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debug_create_info);
	}
	else
	{
		create_info.enabledLayerCount = 0;
		create_info.pNext = nullptr;
	}

	// Create Vulkan Instance
	if (vkCreateInstance(&create_info, nullptr, &m_vk_instance) != VK_SUCCESS) { throw std::runtime_error("FAILED TO CREATE VULKAN INSTANCE."); }
}
// Vulkan: デバッグメッセンジャーの設定
// NOTE: ValidationLayerが有効のとき動作
void TriangleApplication::SetupDebugMessanger()
{
	if (!m_enable_validation_layer) { return; }
	
	// Debug Messangerの設定
	VkDebugUtilsMessengerCreateInfoEXT create_info;
	DefaultDebugSetting(create_info);
	// コールバックの設定
	if (Create_Debug_Utils_Messenger_EXT(m_vk_instance, &create_info, nullptr, &m_debug_messanger) != VK_SUCCESS) { throw std::runtime_error("FAILED TO SET UP DEBUG MESSANGER."); }
}
// Vulkan: ウィンドウサーフェスの生成
void TriangleApplication::CreateSurface()
{
	if (glfwCreateWindowSurface(m_vk_instance, m_window, nullptr, &m_surface) != VK_SUCCESS) { throw std::runtime_error("FAILED TO CREATE WINDOW SURFACE."); }
}
// Vulkan: 標準デバッグメッセンジャーの設定
void TriangleApplication::DefaultDebugSetting(VkDebugUtilsMessengerCreateInfoEXT& create_info)
{
	create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	// コールバック時における重要度の設定.
	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	// コールバック時におけるメッセージフィルタ(とりあえず, 全指定)
	create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	// コールバック関数の設定
	create_info.pfnUserCallback = Debug_Call_Back;
	create_info.pUserData = nullptr;
}
// Vulkan: 物理デバイスの設定
void TriangleApplication::PickUpPhysicalDevice()
{
	// デバイス個数のチェック
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(m_vk_instance, &device_count, nullptr);
	if (device_count == 0) { throw std::runtime_error("FAILD TO FIND GPUS WITH VULKAN SUPPORT."); }
	// メモリ割り当て
	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(m_vk_instance, &device_count, devices.data());
	for (const auto& device : devices) { if (isDeviceSuitable(device)) { m_physical_device = device; break; } }
	// 割り当て失敗
	if (m_physical_device == VK_NULL_HANDLE) { throw std::runtime_error("FAILD TO FIND A SUITABLE GPU."); }
}
// Vulkan: グラフィックカードの適合性評価
// NOTE: 一番価値の高いGPUを選択するなど, 実装方法は様々.
bool TriangleApplication::isDeviceSuitable(const VkPhysicalDevice &device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);

	// DEBUG用print
#if DISPLAY_VULKAN_PHYSICAL_DEVICE_DETAIL
	// Basic Property (name, type, Vulkan version...)
	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(device, &device_properties);
	// Optional Functions (texture compression, 64bit-floats and multi viewport rendering (useful for VR))
	VkPhysicalDeviceFeatures device_features;
	vkGetPhysicalDeviceFeatures(device, &device_features);
	// Display
	CheckPhysicalDeviceInfo(device_properties, device_features);
#endif

	return indices.isComplete();
}
// Vulkan: グラフィックコマンドをサポートするキューファミリの検索
// MEMO: キューファミリ (GPUに仕事を依頼するコマンド群)
QueueFamilyIndices TriangleApplication::FindQueueFamilies(const VkPhysicalDevice &device)
{
	// キューファミリのリストを取得
	uint32_t queue_families_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_families_count, nullptr);
	std::vector<VkQueueFamilyProperties> queue_families(queue_families_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_families_count, queue_families.data());
	
	// VK_QUEUE_GRAPHICS_BITをサポートするキューを探索
	QueueFamilyIndices indices;
	int idx = 0;
	for (const auto& queue_family : queue_families)
	{
		// キュー番号を登録
		if ((queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)) { indices.graphics_family = idx; }
		// 既に値を保持している場合は, 探索終了
		if (indices.isComplete()) { break; }
		++idx;
	}
	return indices;
}
// Vulkan: 論理デバイスの設定
void TriangleApplication::CreateLogicalDevice()
{
	// グラフィックカードのキューファミリを設定
	QueueFamilyIndices indices = FindQueueFamilies(m_physical_device);
	VkDeviceQueueCreateInfo queue_create_info = {};
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueFamilyIndex = indices.graphics_family.value();
	queue_create_info.queueCount = 1;
	// キューの優先度を設定 (0.0-1.0)
	float queue_priority = 1.0f;
	queue_create_info.pQueuePriorities = &queue_priority;

	// デバイス機能の設定
	// NOTE: ジオメトリシェーダ機能の設定などに使用(現時点では, 特に設定はしない).
	VkPhysicalDeviceFeatures device_features = {};

	// 論理デバイスの作成
	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pQueueCreateInfos = &queue_create_info;
	create_info.queueCreateInfoCount = 1;
	create_info.pEnabledFeatures = &device_features;
	// NOTE: enabledLayerCountとppEnableLayerNamesは必要ないが, 互換性を保つため設定する.
	// NOTE: 現時点では, デバイス拡張機能の設定はしない.
	create_info.enabledExtensionCount = 0;
	if (m_enable_validation_layer)
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layer.size());
		create_info.ppEnabledLayerNames = m_validation_layer.data();
	}
	else { create_info.enabledLayerCount = 0; }

	// 論理デバイスのインスタンス化
	if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_logical_device) != VK_SUCCESS) { throw std::runtime_error("FAILED TO CREATE LOGICAL DEVICE."); }

	// 論理デバイスとキューを紐づける(キューハンドル).
	// NOTE: 単一のキューなので, 0を指定.
	vkGetDeviceQueue(m_logical_device, indices.graphics_family.value(), 0, &m_graphics_queue);
}
// Vulkan: 拡張機能のチェック
void TriangleApplication::CheckExtension(const std::vector<const char*>& glfw_extensions)
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
	for (auto ext_name : glfw_extensions) { std::cout << "\t" << ext_name << std::endl; }
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
// Vulkan: 物理デバイス情報の表示
void TriangleApplication::CheckPhysicalDeviceInfo(const VkPhysicalDeviceProperties& prop, const VkPhysicalDeviceFeatures& feature)
{
	// 基本情報
	std::cout << "PHYSICAL DEVICE PROPERTIES" << std::endl;
	std::cout << "\t" << "API VERSION: " << VK_VERSION_MAJOR(prop.apiVersion) << "." << VK_VERSION_MINOR(prop.apiVersion) << "." << VK_VERSION_PATCH(prop.apiVersion) << std::endl;
	std::cout << "\t" << "DEVICE(Name, ID, Type): " << prop.deviceName << " , " << prop.deviceID << " , " << prop.deviceType << std::endl;
	std::cout << "\t" << "DRIVER VERSION: " << prop.driverVersion << std::endl;
	std::cout << "\t" << "VENDER ID: " << prop.vendorID << std::endl << std::endl;
	// オプション機能
	std::cout << "\t" << "GEOMETRY SHADER: " << std::boolalpha << static_cast<bool>(feature.geometryShader) << std::endl;
	std::cout << "\t" << "TESSELLEATION SHADER: " << std::boolalpha << static_cast<bool>(feature.tessellationShader) << std::endl;
}

/* --public-- */

// コンストラクタ
// NOTE: デフォルト設定
TriangleApplication::TriangleApplication() :
	m_window(nullptr), m_window_width(800), m_window_height(600), m_window_name("Vulkan Apps"),
	m_vk_instance(), m_debug_messanger(),
	m_surface(),
	m_physical_device(VK_NULL_HANDLE),
	m_logical_device(), m_graphics_queue()
{

}
// NOTE: ウィンドウ設定
TriangleApplication::TriangleApplication(const int& width, const int& height, const std::string& name):
	m_window(nullptr), m_window_width(width), m_window_height(height), m_window_name(name),
	m_vk_instance(), m_debug_messanger(),
	m_surface(),
	m_physical_device(VK_NULL_HANDLE),
	m_logical_device(), m_graphics_queue()
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