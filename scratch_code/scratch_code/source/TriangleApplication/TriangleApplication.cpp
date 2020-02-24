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
	// 論理デバイス生成
	CreateLogicalDevice();
	// スワップチェーン生成
	CreateSwapChain();
	// イメージビュー生成
	CreateImageViews();
	// レンダーパス
	CreateRenderPass();
	// Graphics Pipelineの組み立て
	CreateGraphicsPipeline();
	// Framebuffer生成
	CreateFrameBuffers();
	// 描画コマンド生成
	CreateCommandPool();
	// コマンドバッファ生成
	CreateCommandBuffers();
	// セマフォ生成
	CreateSemaphores();
}

// メインループ
// NOTE: GLFWの終了, または, エラーの時に終了
void TriangleApplication::MainLoop()
{
	while (!glfwWindowShouldClose(m_window))
	{
		// イベントチェック
		glfwPollEvents();
		// 描画
		DrawFrame();
	}
}

// 終了処理
void TriangleApplication::CleanUp()
{
	/* --Vulkanの終了処理-- */
	// セマフォ破棄
	vkDestroySemaphore(m_logical_device, m_image_available_semaphore, nullptr);
	vkDestroySemaphore(m_logical_device, m_render_finished_semaphore, nullptr);
	// 描画コマンド破棄
	vkDestroyCommandPool(m_logical_device, m_command_pool, nullptr);
	// フレームバッファ破棄
	for (auto frame_buffer : m_swap_chain_frame_buffers) { vkDestroyFramebuffer(m_logical_device, frame_buffer, nullptr); }
	// パイプライン破棄
	vkDestroyPipeline(m_logical_device, m_graphics_pipeline, nullptr);
	// パイプラインレイアウト破棄
	vkDestroyPipelineLayout(m_logical_device, m_pipeline_layout, nullptr);
	// レンダーパス破棄
	vkDestroyRenderPass(m_logical_device, m_render_pass, nullptr);
	// イメージビュー破棄
	for (auto image_view : m_swap_chain_image_views) { vkDestroyImageView(m_logical_device, image_view, nullptr); }
	// スワップチェーン破棄
	vkDestroySwapchainKHR(m_logical_device, m_swap_chain, nullptr);
	// 論理デバイス破棄
	vkDestroyDevice(m_logical_device, nullptr);
	// デバッグメッセンジャ破棄
	if (m_enable_validation_layer) { Destroy_Debug_Utils_Messenger_EXT(m_vk_instance, m_debug_messanger, nullptr); }
	// ウィンドウサーフェス破棄
	vkDestroySurfaceKHR(m_vk_instance, m_surface, nullptr);
	// インスタンス破棄
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
	
	// 拡張機能の確認
	bool is_extension_supported = CheckDeviceExtensionSupport(device);

	// SwapChainサポートの確認
	// NOTE: 拡張機能確認後にクエリの発行
	bool swap_chain_adequate = false;
	if (is_extension_supported)
	{
		SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(device);
		swap_chain_adequate = !swap_chain_support.m_formats.empty() && !swap_chain_support.m_present_modes.empty();
	}

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

	return indices.isComplete() && is_extension_supported && swap_chain_adequate;
}
// Vulkan: キューファミリの検索(graphic, present)
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

		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, idx, m_surface, &present_support);
		// presentチェック
		if (present_support) { indices.present_family = idx; }

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
	
	std::vector<VkDeviceQueueCreateInfo> queue_create_multi_info = {};
	std::set<uint32_t> unique_queue_families = { indices.graphics_family.value(), indices.present_family.value() };
	
	// キューの優先度を設定 (0.0-1.0)
	float queue_priority = 1.0f;
	for (uint32_t queue_family : unique_queue_families)
	{
		VkDeviceQueueCreateInfo queue_create_info = {};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = indices.graphics_family.value();
		queue_create_info.queueCount = 1;
		queue_create_info.pQueuePriorities = &queue_priority;
		queue_create_multi_info.push_back(queue_create_info);
	}

	// デバイス機能の設定
	// NOTE: ジオメトリシェーダ機能の設定などに使用(現時点では, 特に設定はしない).
	VkPhysicalDeviceFeatures device_features = {};

	// 論理デバイスの作成
	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_multi_info.size());
	create_info.pQueueCreateInfos = queue_create_multi_info.data();
	create_info.pEnabledFeatures = &device_features;

	// デバイス拡張機能設定
	create_info.enabledExtensionCount = static_cast<uint32_t>(m_device_extensions.size());
	create_info.ppEnabledExtensionNames = m_device_extensions.data();

	// NOTE: enabledLayerCountとppEnableLayerNamesは必要ないが, 互換性を保つため設定する.
	if (m_enable_validation_layer)
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layer.size());
		create_info.ppEnabledLayerNames = m_validation_layer.data();
	}
	else { create_info.enabledLayerCount = 0; }

	// 論理デバイスのインスタンス化
	if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_logical_device) != VK_SUCCESS) { throw std::runtime_error("FAILED TO CREATE LOGICAL DEVICE."); }

	// キューハンドル取得
	// NOTE: 単一のキューなので, 0を指定.
	vkGetDeviceQueue(m_logical_device, indices.graphics_family.value(), 0, &m_graphics_queue);
	vkGetDeviceQueue(m_logical_device, indices.present_family.value(), 0, &m_present_queue);
}
// Vulkan: スワップチェーン生成
void TriangleApplication::CreateSwapChain()
{
	SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(m_physical_device);

	VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.m_formats);
	VkPresentModeKHR present_mode = ChooseSwapPresentMode(swap_chain_support.m_present_modes);
	VkExtent2D extent = ChooseSwapExtent(swap_chain_support.m_capabillites);

	// swap chainのイメージ数設定
	// NOTE: +1することで, 余裕を持たせる(但し, 最大値を超えないようにする).
	uint32_t image_count = swap_chain_support.m_capabillites.minImageCount + 1;
	// NOTE: 0は最大値が無いことを示す.
	if (swap_chain_support.m_capabillites.maxImageCount > 0 && image_count > swap_chain_support.m_capabillites.maxImageCount) { image_count = swap_chain_support.m_capabillites.maxImageCount; }

	// swap chainオブジェクトの作成
	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = m_surface;
	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageExtent = extent;
	// NOTE: stereoscopic以外は, 常に「1」
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// キューファミリ設定
	// NOTE: グラフィックファミリがプレゼントファミリと異なる時に必要.
	QueueFamilyIndices indices = FindQueueFamilies(m_physical_device);
	uint32_t queue_family_indices[] = { indices.graphics_family.value(), indices.present_family.value() };
	if (indices.graphics_family != indices.present_family)
	{
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indices;
	}
	else
	{
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;     // optional
		create_info.pQueueFamilyIndices = nullptr; // optional
	}

	create_info.preTransform = swap_chain_support.m_capabillites.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	// クリッピング
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	// SwapChain生成
	if (vkCreateSwapchainKHR(m_logical_device, &create_info, nullptr, &m_swap_chain) != VK_SUCCESS) { throw std::runtime_error("FAILED TO CREATE SWAP CHAIN."); }

	// Image数の取得
	vkGetSwapchainImagesKHR(m_logical_device, m_swap_chain, &image_count, nullptr);
	m_swap_chain_image.resize(image_count);
	vkGetSwapchainImagesKHR(m_logical_device, m_swap_chain, &image_count, m_swap_chain_image.data());
	
	m_swap_chain_image_format = surface_format.format;
	m_swap_chain_extent = extent;
}
// Vulkan: スワップチェーンの設定
// NOTE: 拡張機能にスワップチェーンがあるか走査.
// NOTE: 取り除くことでチェックしている.
bool TriangleApplication::CheckDeviceExtensionSupport(const VkPhysicalDevice& device)
{
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

	std::vector<VkExtensionProperties> available_extensions(extension_count);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

	std::set<std::string> required_extensions(m_device_extensions.begin(), m_device_extensions.end());
	for (const auto& extension : available_extensions)
	{
#ifdef DISPLAY_VULKAN_EXTENSION
		std::cout << extension.extensionName << " " << extension.specVersion << std::endl;
#endif
		required_extensions.erase(extension.extensionName);
	}
#ifdef DISPLAY_VULKAN_EXTENSION
	std::cout << std::endl;
#endif

	return required_extensions.empty();
}
// Vulkan: サーフェス設定
SwapChainSupportDetails TriangleApplication::QuerySwapChainSupport(const VkPhysicalDevice& device)
{
	// サポートクエリ
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.m_capabillites);

	// surfaceクエリ
	uint32_t format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, nullptr);
	if (format_count != 0)
	{
		details.m_formats.resize(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, details.m_formats.data());
	}
	// presentクエリ
	uint32_t present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &present_mode_count, nullptr);
	if (present_mode_count != 0)
	{
		details.m_present_modes.resize(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &present_mode_count, details.m_present_modes.data());
	}

	return details;
}
// Vulkan: 色空間の選択
VkSurfaceFormatKHR TriangleApplication::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats)
{
	// 色空間チェック
	for (const auto& available_format : available_formats)
	{
		// 32bitSRGB空間を使用する.
		if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) { return available_format; }
	}
	// SRGB空間がサポートされていない場合は, 先頭の色空間を使用.
	return available_formats[0];
}
// Vulkan: 描画モードの選択
VkPresentModeKHR TriangleApplication::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes)
{
	// トリプルバッファリングが利用可能なら選択.
	for (const auto& available_present_mode : available_present_modes)
	{
		if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) { return available_present_mode; }
	}
	// 垂直同期モード (?)
	// NOTE: サポートされていることが保証されている.
	return VK_PRESENT_MODE_FIFO_KHR;
}
// Vulkan: 解像度の選択
VkExtent2D TriangleApplication::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX) { return capabilities.currentExtent; }
	
	// width, heightを許容範囲に収める.
	VkExtent2D actualExtent = {m_window_width, m_window_height};
	actualExtent.width  = std::max(capabilities.minImageExtent.width,  std::min(capabilities.maxImageExtent.width, actualExtent.width));
	actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

	return actualExtent;
}
// Vulkan: ImageViewsの生成
void TriangleApplication::CreateImageViews()
{
	m_swap_chain_image_views.resize(m_swap_chain_image.size());
	for (size_t i = 0; i < m_swap_chain_image_views.size(); ++i)
	{
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = m_swap_chain_image[i];
		create_info.format = m_swap_chain_image_format;
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;

		create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_logical_device, &create_info, nullptr, &m_swap_chain_image_views[i]) != VK_SUCCESS) { throw std::runtime_error("FAILD TO CREATE IMAGE VIEWS."); }
	}
}

// Vulkan: Graphics Pipeline
void TriangleApplication::CreateGraphicsPipeline()
{
	// SPIR-V読み取り
	auto vert_shader_code = ReadFile(VERT_PATH);
	auto frag_shader_code = ReadFile(FRAG_PATH);

	// シェーダモジュール生成
	VkShaderModule vert_shader_module = CreateShaderModule(vert_shader_code);
	VkShaderModule frag_shader_module = CreateShaderModule(frag_shader_code);

	// パイプラインステージに割り当て
	// VERT
	VkPipelineShaderStageCreateInfo vert_shader_stage_info = {};
	vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vert_shader_stage_info.module = vert_shader_module;
	vert_shader_stage_info.pName = "main";
	// FRAG
	VkPipelineShaderStageCreateInfo frag_shader_stage_info = {};
	frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	frag_shader_stage_info.module = frag_shader_module;
	frag_shader_stage_info.pName = "main";

	// struct
	VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };

	// VertexInput
	VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = 0;
	vertex_input_info.pVertexBindingDescriptions = nullptr;   // Optional
	vertex_input_info.vertexAttributeDescriptionCount = 0;
	vertex_input_info.pVertexAttributeDescriptions = nullptr; // Optional

	// InputAssembly
	VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
	input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly.primitiveRestartEnable = VK_FALSE;

	// ViewPort
	VkViewport view_port = {};
	view_port.x = 0.0f;
	view_port.y = 0.0f;
	view_port.width = static_cast<float>(m_swap_chain_extent.width);
	view_port.height = static_cast<float>(m_swap_chain_extent.height);
	view_port.minDepth = 0.0f;
	view_port.maxDepth = 1.0f;

	// Sissor
	// NOTE: ViewPortの切り取り範囲
	VkRect2D sissor = {};
	sissor.offset = { 0, 0 };
	sissor.extent = m_swap_chain_extent;

	// Viewport,Sissorの反映
	VkPipelineViewportStateCreateInfo viewport_info = {};
	viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_info.viewportCount = 1;
	viewport_info.pViewports = &view_port;
	viewport_info.scissorCount = 1;
	viewport_info.pScissors = &sissor;

	// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f;          // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

	// MultiSampling
	VkPipelineMultisampleStateCreateInfo multi_sampling_info = {};
	multi_sampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multi_sampling_info.sampleShadingEnable = VK_FALSE;
	multi_sampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multi_sampling_info.minSampleShading = 1.0f;          // Optional
	multi_sampling_info.pSampleMask = nullptr;            // Optional
	multi_sampling_info.alphaToCoverageEnable = VK_FALSE; // Optional
	multi_sampling_info.alphaToOneEnable = VK_FALSE;      // Optional

	// Depth And Stencil Testing
	// NOTE: 今は設定しない

	// Color blending
	// NOTE: Alphaチャネルオン
	VkPipelineColorBlendAttachmentState color_blend_attachment = {};
	color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	color_blend_attachment.blendEnable = VK_TRUE;
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;           // Optional
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // Optional
	color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

	VkPipelineColorBlendStateCreateInfo color_blending = {};
	color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blending.logicOpEnable = VK_FALSE;
	color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
	color_blending.attachmentCount = 1;
	color_blending.pAttachments = &color_blend_attachment;
	color_blending.blendConstants[0] = 0.0f; // Optional
	color_blending.blendConstants[1] = 0.0f; // Optional
	color_blending.blendConstants[2] = 0.0f; // Optional
	color_blending.blendConstants[3] = 0.0f; // Optional

	// Dynamic State
	VkDynamicState dynamic_states[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };
	VkPipelineDynamicStateCreateInfo dynamic_state_info = {};
	dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_info.dynamicStateCount = 2;
	dynamic_state_info.pDynamicStates = dynamic_states;

	// PipelineLayout
	VkPipelineLayoutCreateInfo pipeline_layout_info = {};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = 0;            // Optional
	pipeline_layout_info.pSetLayouts = nullptr;         // Optional
	pipeline_layout_info.pushConstantRangeCount = 0;    // Optional
	pipeline_layout_info.pPushConstantRanges = nullptr; // Optional
	// パイプラインレイアウト生成
	if (vkCreatePipelineLayout(m_logical_device, &pipeline_layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS) { throw std::runtime_error("FAILED TO CREATE PIPELINE LAYOUT."); }

	// パイプライン結合
	// 固定機能
	VkGraphicsPipelineCreateInfo pipeline_info = {};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = 2;
	pipeline_info.pStages = shader_stages;
	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly;
	pipeline_info.pViewportState = &viewport_info;
	pipeline_info.pRasterizationState = &rasterizer;
	pipeline_info.pMultisampleState = &multi_sampling_info;
	pipeline_info.pDepthStencilState = nullptr; // Optional
	pipeline_info.pColorBlendState = &color_blending;
	pipeline_info.pDynamicState = nullptr;      // Optional
	// パイプラインレイアウト
	pipeline_info.layout = m_pipeline_layout;
	// レンダーパス, サブパス
	pipeline_info.renderPass = m_render_pass;
	pipeline_info.subpass = 0;
	// 既存パイプライン設定
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipeline_info.basePipelineIndex = -1;              // Optional
	
	// パイプライン生成
	if (vkCreateGraphicsPipelines(m_logical_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_graphics_pipeline) != VK_SUCCESS) { throw std::runtime_error("FAILED TO CRATE GRAPHICS PIPELINE."); }

	// シェーダモジュール破棄
	vkDestroyShaderModule(m_logical_device, frag_shader_module, nullptr);
	vkDestroyShaderModule(m_logical_device, vert_shader_module, nullptr);
}
// Vulkan: RenderPass
void TriangleApplication::CreateRenderPass()
{
	VkAttachmentDescription color_attachment = {};
	color_attachment.format = m_swap_chain_image_format;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Subpass
	VkAttachmentReference color_attachment_ref = {};
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;

	// Render Pass
	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = &color_attachment;
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	if (vkCreateRenderPass(m_logical_device, &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS) { throw std::runtime_error("FAILED TO CREATE REDER PASS."); }
}
// Vulkan: Framebuffer
void TriangleApplication::CreateFrameBuffers()
{
	// Create Framebuffer
	m_swap_chain_frame_buffers.resize(m_swap_chain_image_views.size());
	for (size_t i = 0; i < m_swap_chain_image_views.size(); ++i)
	{
		VkImageView attachments[] = { m_swap_chain_image_views[i] };

		VkFramebufferCreateInfo frame_buffer_info = {};
		frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frame_buffer_info.renderPass = m_render_pass;
		frame_buffer_info.attachmentCount = 1;
		frame_buffer_info.pAttachments = attachments;
		frame_buffer_info.width = m_swap_chain_extent.width;
		frame_buffer_info.height = m_swap_chain_extent.height;
		frame_buffer_info.layers = 1;

		if (vkCreateFramebuffer(m_logical_device, &frame_buffer_info, nullptr, &m_swap_chain_frame_buffers[i]) != VK_SUCCESS) { throw std::runtime_error("FAILED TO CREATE FRAME BUFFERS."); }
	}
}
// Vulkan: CommandPool
void TriangleApplication::CreateCommandPool()
{
	// CommandPool
	QueueFamilyIndices queue_families_indices = FindQueueFamilies(m_physical_device);
	VkCommandPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_info.queueFamilyIndex = queue_families_indices.graphics_family.value();
	pool_info.flags = 0;
	if (vkCreateCommandPool(m_logical_device, &pool_info, nullptr, &m_command_pool) != VK_SUCCESS) { throw std::runtime_error("FAILED TO CREATE COMMAND POOL."); }
}
// Vulkan: コマンドバッファ生成
void TriangleApplication::CreateCommandBuffers()
{
	m_command_buffers.resize(m_swap_chain_frame_buffers.size());
	// コマンドバッファ生成
	VkCommandBufferAllocateInfo allocate_info = {};
	allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	allocate_info.commandPool = m_command_pool;
	allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocate_info.commandBufferCount = static_cast<uint32_t>(m_command_buffers.size());
	if (vkAllocateCommandBuffers(m_logical_device, &allocate_info, m_command_buffers.data()) != VK_SUCCESS) { throw std::runtime_error("FAIELD TO CREATE COMMAND BUFFERS."); }

	// コマンドバッファの記録
	for (size_t i = 0; i < m_command_buffers.size(); ++i)
	{
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = 0;                  // Optional
		begin_info.pInheritanceInfo = nullptr; // Optional

		// recording
		if (vkBeginCommandBuffer(m_command_buffers[i], &begin_info) != VK_SUCCESS) { throw std::runtime_error("FAIELD TO BEGIN RECORDING COMMAND BUFFERS."); }
	
		// Starting a render pass
		VkRenderPassBeginInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = m_render_pass;
		render_pass_info.framebuffer = m_swap_chain_frame_buffers[i];

		render_pass_info.renderArea.offset = { 0,0 };
		render_pass_info.renderArea.extent = m_swap_chain_extent;

		// VK_ATTACHMENT_LOAD_OP_CLEARに使用する設定
		VkClearValue clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clear_color;

		// コマンド記録
		vkCmdBeginRenderPass(m_command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		// Graphics Pipelineにバインド
		vkCmdBindPipeline(m_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);

		// 描画コマンド
		// buffer, 頂点数, インスタンスレンダリング時に使用, 頂点バッファオフセット(gl_VertexIndex), レンダリングオフセット(gl_InstanceIndex) 
		vkCmdDraw(m_command_buffers[i], 3, 1, 0, 0);

		// Finishing Up
		vkCmdEndRenderPass(m_command_buffers[i]);
		if (vkEndCommandBuffer(m_command_buffers[i]) != VK_SUCCESS) { throw std::runtime_error("FAILED TO RECORD COMMAND BUFFER."); }
	}
}

// Vulkan: シェーダ
VkShaderModule TriangleApplication::CreateShaderModule(const std::vector<char>& byte_code)
{
	// パラメータ設定
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = byte_code.size();
	create_info.pCode = reinterpret_cast<const uint32_t*>(byte_code.data());

	// シェーダモジュール生成
	VkShaderModule shader_module;
	if (vkCreateShaderModule(m_logical_device, &create_info, nullptr, &shader_module) != VK_SUCCESS) { throw std::runtime_error("FAILED TO CREATE SHADER MODULE."); }

	return shader_module;
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

// MainLoop
// Frame描画
// NOTE: スワップチェーンから取得 -> コマンドバッファ実行 -> スワップチェーンに画像を戻す
void TriangleApplication::DrawFrame()
{
	// スワップチェーン設定
	// NOTE: 論理デバイス, スワップチェーン, タイムアウト(ns), 同期オブジェクト
	uint32_t image_index;
	vkAcquireNextImageKHR(m_logical_device, m_swap_chain, UINT64_MAX, m_image_available_semaphore, VK_NULL_HANDLE, &image_index);

	// Submitting the command buffers
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	// 待機するセマフォとパイプラインステージ
	VkSemaphore wait_semaphores[] = { m_image_available_semaphore };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_semaphores;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &m_command_buffers[image_index];

	VkSemaphore signal_semaphores[] = { m_render_finished_semaphore };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal_semaphores;

	if (vkQueueSubmit(m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) { throw std::runtime_error("FAILED TO SUBMIT DRAW COMMAND BUFFER."); }
}
// Vulkan: セマフォ生成
void TriangleApplication::CreateSemaphores()
{
	// NOTE: 現段階では, sTypeのみ設定(将来的に増えるかも)
	VkSemaphoreCreateInfo semaphore_info = {};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (
		vkCreateSemaphore(m_logical_device, &semaphore_info, nullptr, &m_image_available_semaphore) != VK_SUCCESS ||
		vkCreateSemaphore(m_logical_device, &semaphore_info, nullptr, &m_render_finished_semaphore) != VK_SUCCESS
	   ) { throw std::runtime_error("FAIELD TO CREATE SEMAPHORES."); }
}

/* --public-- */

// コンストラクタ
// NOTE: デフォルト設定
TriangleApplication::TriangleApplication() :
	m_window(nullptr), m_window_width(800), m_window_height(600), m_window_name("Vulkan Apps"),
	m_vk_instance(), m_debug_messanger(),
	m_surface(),
	m_physical_device(VK_NULL_HANDLE),
	m_logical_device(),
	m_graphics_queue(), m_present_queue(),
	m_swap_chain(), m_swap_chain_image_format(), m_swap_chain_extent(),
	m_render_pass(), m_pipeline_layout(), m_graphics_pipeline(),
	m_command_pool(),
	m_image_available_semaphore(), m_render_finished_semaphore()
{

}
// NOTE: ウィンドウ設定
TriangleApplication::TriangleApplication(const int& width, const int& height, const std::string& name):
	m_window(nullptr), m_window_width(width), m_window_height(height), m_window_name(name),
	m_vk_instance(), m_debug_messanger(),
	m_surface(),
	m_physical_device(VK_NULL_HANDLE),
	m_logical_device(),
	m_graphics_queue(), m_present_queue(),
	m_swap_chain(), m_swap_chain_image_format(), m_swap_chain_extent(),
	m_render_pass(), m_pipeline_layout(), m_graphics_pipeline(),
	m_command_pool(),
	m_image_available_semaphore(), m_render_finished_semaphore()
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