#include "./TriangleApplication.hpp"

/* --private-- */

// ����������
void TriangleApplication::InitVulkan()
{
	// �C���X�^���X����
	CreateInstance();
	// �f�o�b�O���b�Z���W���̐ݒ�
	SetupDebugMessanger();
	// �E�B���h�E�T�[�t�F�X����
	CreateSurface();
	// �����f�o�C�X�̐ݒ�
	PickUpPhysicalDevice();
	// �_���f�o�C�X����
	CreateLogicalDevice();
	// �X���b�v�`�F�[������
	CreateSwapChain();
	// �C���[�W�r���[����
	CreateImageViews();

	// Graphics Pipeline�̑g�ݗ���
	CreateGraphicsPipeline();
}

// ���C�����[�v
// NOTE: GLFW�̏I��, �܂���, �G���[�̎��ɏI��
void TriangleApplication::MainLoop()
{
	while (!glfwWindowShouldClose(m_window))
	{
		// �C�x���g�`�F�b�N
		glfwPollEvents();
	}
}

// �I������
void TriangleApplication::CleanUp()
{
	/* --Vulkan�̏I������-- */
	// �C���[�W�r���[�j��
	for (auto image_view : m_swap_chain_image_views) { vkDestroyImageView(m_logical_device, image_view, nullptr); }
	// �X���b�v�`�F�[���j��
	vkDestroySwapchainKHR(m_logical_device, m_swap_chain, nullptr);
	// �_���f�o�C�X�j��
	vkDestroyDevice(m_logical_device, nullptr);
	// �f�o�b�O���b�Z���W���j��
	if (m_enable_validation_layer) { Destroy_Debug_Utils_Messenger_EXT(m_vk_instance, m_debug_messanger, nullptr); }
	// �E�B���h�E�T�[�t�F�X�j��
	vkDestroySurfaceKHR(m_vk_instance, m_surface, nullptr);
	// �C���X�^���X�j��
	vkDestroyInstance(m_vk_instance, nullptr);

	/* --GLFW3�̏I������--*/
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

/* --GLFW-- */
// �E�B���h�E������
void TriangleApplication::InitWindow()
{
	/* --GLFW3�̏���������-- */
	glfwInit();
	// OpenGL�p�̃R���e�L�X�g�𐶐����Ȃ�
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// �����_�ł�, �E�B���h�E�T�C�Y�̕ύX���l�����Ȃ�
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	// �E�B���h�E����
	m_window = glfwCreateWindow(m_window_width, m_window_height, m_window_name.c_str(), nullptr, nullptr);
}
// �g���@�\�ݒ�
std::vector<const char*> TriangleApplication::RequiredExtensionGLFW()
{
	uint32_t glfw_extension_count = 0;
	const char** glfw_extension;
	
	// �g���@�\�̎擾
	glfw_extension = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	std::vector<const char*> extensions(glfw_extension, glfw_extension + glfw_extension_count);
	if (m_enable_validation_layer) { extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); }

	return extensions;
}

/* --Vulkan-- */
// Vulkan�C���X�^���X�𐶐�
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
// Vulkan: �f�o�b�O���b�Z���W���[�̐ݒ�
// NOTE: ValidationLayer���L���̂Ƃ�����
void TriangleApplication::SetupDebugMessanger()
{
	if (!m_enable_validation_layer) { return; }
	
	// Debug Messanger�̐ݒ�
	VkDebugUtilsMessengerCreateInfoEXT create_info;
	DefaultDebugSetting(create_info);
	// �R�[���o�b�N�̐ݒ�
	if (Create_Debug_Utils_Messenger_EXT(m_vk_instance, &create_info, nullptr, &m_debug_messanger) != VK_SUCCESS) { throw std::runtime_error("FAILED TO SET UP DEBUG MESSANGER."); }
}
// Vulkan: �E�B���h�E�T�[�t�F�X�̐���
void TriangleApplication::CreateSurface()
{
	if (glfwCreateWindowSurface(m_vk_instance, m_window, nullptr, &m_surface) != VK_SUCCESS) { throw std::runtime_error("FAILED TO CREATE WINDOW SURFACE."); }
}
// Vulkan: �W���f�o�b�O���b�Z���W���[�̐ݒ�
void TriangleApplication::DefaultDebugSetting(VkDebugUtilsMessengerCreateInfoEXT& create_info)
{
	create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	// �R�[���o�b�N���ɂ�����d�v�x�̐ݒ�.
	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	// �R�[���o�b�N���ɂ����郁�b�Z�[�W�t�B���^(�Ƃ肠����, �S�w��)
	create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	// �R�[���o�b�N�֐��̐ݒ�
	create_info.pfnUserCallback = Debug_Call_Back;
	create_info.pUserData = nullptr;
}
// Vulkan: �����f�o�C�X�̐ݒ�
void TriangleApplication::PickUpPhysicalDevice()
{
	// �f�o�C�X���̃`�F�b�N
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(m_vk_instance, &device_count, nullptr);
	if (device_count == 0) { throw std::runtime_error("FAILD TO FIND GPUS WITH VULKAN SUPPORT."); }
	// ���������蓖��
	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(m_vk_instance, &device_count, devices.data());
	for (const auto& device : devices) { if (isDeviceSuitable(device)) { m_physical_device = device; break; } }
	// ���蓖�Ď��s
	if (m_physical_device == VK_NULL_HANDLE) { throw std::runtime_error("FAILD TO FIND A SUITABLE GPU."); }
}
// Vulkan: �O���t�B�b�N�J�[�h�̓K�����]��
// NOTE: ��ԉ��l�̍���GPU��I������Ȃ�, �������@�͗l�X.
bool TriangleApplication::isDeviceSuitable(const VkPhysicalDevice &device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);
	
	// �g���@�\�̊m�F
	bool is_extension_supported = CheckDeviceExtensionSupport(device);

	// SwapChain�T�|�[�g�̊m�F
	// NOTE: �g���@�\�m�F��ɃN�G���̔��s
	bool swap_chain_adequate = false;
	if (is_extension_supported)
	{
		SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(device);
		swap_chain_adequate = !swap_chain_support.m_formats.empty() && !swap_chain_support.m_present_modes.empty();
	}

	// DEBUG�pprint
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
// Vulkan: �L���[�t�@�~���̌���(graphic, present)
// MEMO: �L���[�t�@�~�� (GPU�Ɏd�����˗�����R�}���h�Q)
QueueFamilyIndices TriangleApplication::FindQueueFamilies(const VkPhysicalDevice &device)
{
	// �L���[�t�@�~���̃��X�g���擾
	uint32_t queue_families_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_families_count, nullptr);
	std::vector<VkQueueFamilyProperties> queue_families(queue_families_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_families_count, queue_families.data());
	
	// VK_QUEUE_GRAPHICS_BIT���T�|�[�g����L���[��T��
	QueueFamilyIndices indices;
	int idx = 0;
	for (const auto& queue_family : queue_families)
	{
		// �L���[�ԍ���o�^
		if ((queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)) { indices.graphics_family = idx; }

		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, idx, m_surface, &present_support);
		// present�`�F�b�N
		if (present_support) { indices.present_family = idx; }

		// ���ɒl��ێ����Ă���ꍇ��, �T���I��
		if (indices.isComplete()) { break; }
		++idx;
	}
	return indices;
}
// Vulkan: �_���f�o�C�X�̐ݒ�
void TriangleApplication::CreateLogicalDevice()
{
	// �O���t�B�b�N�J�[�h�̃L���[�t�@�~����ݒ�
	QueueFamilyIndices indices = FindQueueFamilies(m_physical_device);
	
	std::vector<VkDeviceQueueCreateInfo> queue_create_multi_info = {};
	std::set<uint32_t> unique_queue_families = { indices.graphics_family.value(), indices.present_family.value() };
	
	// �L���[�̗D��x��ݒ� (0.0-1.0)
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

	// �f�o�C�X�@�\�̐ݒ�
	// NOTE: �W�I���g���V�F�[�_�@�\�̐ݒ�ȂǂɎg�p(�����_�ł�, ���ɐݒ�͂��Ȃ�).
	VkPhysicalDeviceFeatures device_features = {};

	// �_���f�o�C�X�̍쐬
	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_multi_info.size());
	create_info.pQueueCreateInfos = queue_create_multi_info.data();
	create_info.pEnabledFeatures = &device_features;

	// �f�o�C�X�g���@�\�ݒ�
	create_info.enabledExtensionCount = static_cast<uint32_t>(m_device_extensions.size());
	create_info.ppEnabledExtensionNames = m_device_extensions.data();

	// NOTE: enabledLayerCount��ppEnableLayerNames�͕K�v�Ȃ���, �݊�����ۂ��ߐݒ肷��.
	if (m_enable_validation_layer)
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layer.size());
		create_info.ppEnabledLayerNames = m_validation_layer.data();
	}
	else { create_info.enabledLayerCount = 0; }

	// �_���f�o�C�X�̃C���X�^���X��
	if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_logical_device) != VK_SUCCESS) { throw std::runtime_error("FAILED TO CREATE LOGICAL DEVICE."); }

	// �L���[�n���h���擾
	// NOTE: �P��̃L���[�Ȃ̂�, 0���w��.
	vkGetDeviceQueue(m_logical_device, indices.graphics_family.value(), 0, &m_graphics_queue);
	vkGetDeviceQueue(m_logical_device, indices.present_family.value(), 0, &m_present_queue);
}
// Vulkan: �X���b�v�`�F�[������
void TriangleApplication::CreateSwapChain()
{
	SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(m_physical_device);

	VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.m_formats);
	VkPresentModeKHR present_mode = ChooseSwapPresentMode(swap_chain_support.m_present_modes);
	VkExtent2D extent = ChooseSwapExtent(swap_chain_support.m_capabillites);

	// swap chain�̃C���[�W���ݒ�
	// NOTE: +1���邱�Ƃ�, �]�T����������(�A��, �ő�l�𒴂��Ȃ��悤�ɂ���).
	uint32_t image_count = swap_chain_support.m_capabillites.minImageCount + 1;
	// NOTE: 0�͍ő�l���������Ƃ�����.
	if (swap_chain_support.m_capabillites.maxImageCount > 0 && image_count > swap_chain_support.m_capabillites.maxImageCount) { image_count = swap_chain_support.m_capabillites.maxImageCount; }

	// swap chain�I�u�W�F�N�g�̍쐬
	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = m_surface;
	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageExtent = extent;
	// NOTE: stereoscopic�ȊO��, ��Ɂu1�v
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// �L���[�t�@�~���ݒ�
	// NOTE: �O���t�B�b�N�t�@�~�����v���[���g�t�@�~���ƈقȂ鎞�ɕK�v.
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
	// �N���b�s���O
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	// SwapChain����
	if (vkCreateSwapchainKHR(m_logical_device, &create_info, nullptr, &m_swap_chain) != VK_SUCCESS) { throw std::runtime_error("FAILED TO CREATE SWAP CHAIN."); }

	// Image���̎擾
	vkGetSwapchainImagesKHR(m_logical_device, m_swap_chain, &image_count, nullptr);
	m_swap_chain_image.resize(image_count);
	vkGetSwapchainImagesKHR(m_logical_device, m_swap_chain, &image_count, m_swap_chain_image.data());
	
	m_swap_chain_image_format = surface_format.format;
	m_swap_chain_extent = extent;
}
// Vulkan: �X���b�v�`�F�[���̐ݒ�
// NOTE: �g���@�\�ɃX���b�v�`�F�[�������邩����.
// NOTE: ��菜�����ƂŃ`�F�b�N���Ă���.
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
// Vulkan: �T�[�t�F�X�ݒ�
SwapChainSupportDetails TriangleApplication::QuerySwapChainSupport(const VkPhysicalDevice& device)
{
	// �T�|�[�g�N�G��
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.m_capabillites);

	// surface�N�G��
	uint32_t format_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, nullptr);
	if (format_count != 0)
	{
		details.m_formats.resize(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &format_count, details.m_formats.data());
	}
	// present�N�G��
	uint32_t present_mode_count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &present_mode_count, nullptr);
	if (present_mode_count != 0)
	{
		details.m_present_modes.resize(present_mode_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &present_mode_count, details.m_present_modes.data());
	}

	return details;
}
// Vulkan: �F��Ԃ̑I��
VkSurfaceFormatKHR TriangleApplication::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats)
{
	// �F��ԃ`�F�b�N
	for (const auto& available_format : available_formats)
	{
		// 32bitSRGB��Ԃ��g�p����.
		if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) { return available_format; }
	}
	// SRGB��Ԃ��T�|�[�g����Ă��Ȃ��ꍇ��, �擪�̐F��Ԃ��g�p.
	return available_formats[0];
}
// Vulkan: �`�惂�[�h�̑I��
VkPresentModeKHR TriangleApplication::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes)
{
	// �g���v���o�b�t�@�����O�����p�\�Ȃ�I��.
	for (const auto& available_present_mode : available_present_modes)
	{
		if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) { return available_present_mode; }
	}
	// �����������[�h (?)
	// NOTE: �T�|�[�g����Ă��邱�Ƃ��ۏ؂���Ă���.
	return VK_PRESENT_MODE_FIFO_KHR;
}
// Vulkan: �𑜓x�̑I��
VkExtent2D TriangleApplication::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX) { return capabilities.currentExtent; }
	
	// width, height�����e�͈͂Ɏ��߂�.
	VkExtent2D actualExtent = {m_window_width, m_window_height};
	actualExtent.width  = std::max(capabilities.minImageExtent.width,  std::min(capabilities.maxImageExtent.width, actualExtent.width));
	actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

	return actualExtent;
}
// Vulkan: ImageViews�̐���
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
	auto vert_shader_code = ReadFile(VERT_PATH);
	auto frag_shader_code = ReadFile(FRAG_PATH);
}

// Vulkan: �g���@�\�̃`�F�b�N
void TriangleApplication::CheckExtension(const std::vector<const char*>& glfw_extensions)
{
	// Vulkan�g���@�\���̎擾
	uint32_t extension_count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
	// Vulkan�g���@�\�̎擾
	std::vector<VkExtensionProperties> extensions(extension_count);
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
	// Vulkan�g���@�\�\��
	std::cout << "AVAILABLE VULKAN EXTENSIONS:" << std::endl;
	for (const auto& extension : extensions) { std::cout << "\t" << extension.extensionName << std::endl; }
	std::cout << std::endl;
	// GLFW3���T�|�[�g����Vulkan�g���@�\�̕\��
	std::cout << "AVAILABLE GLFW VULKAN EXTENSIONS:" << std::endl;
	for (auto ext_name : glfw_extensions) { std::cout << "\t" << ext_name << std::endl; }
	std::cout << std::endl;
}
// Vulkan: Validation Layer�̃T�|�[�g�m�F
bool TriangleApplication::CheckValidationLayerSupport()
{
	// ���C���[���擾
	uint32_t layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
	// ���C���[�擾
	std::vector<VkLayerProperties> layers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, layers.data());

	// �擾�������C���[��m_validation_layer���܂܂�Ă��邩�`�F�b�N
	for (const char* layer_name : m_validation_layer)
	{
		bool is_layer_found = false;
		for (const auto& layer_properties : layers)
		{
			// ���C���[�ڍׂ̕\��
#ifdef DISPLAY_VULKAN_VALIDATION_LAYER_DETAIL
			std::cout << layer_properties.layerName << ": " << layer_properties.description << std::endl;
			std::cout << "IMPLEMENT VERSION: " << layer_properties.implementationVersion << std::endl;
			std::cout << "SPECIFICATION VERSION: " << layer_properties.specVersion << std::endl << std::endl;
#endif
			// ���C���[�����݂���ꍇ, ���𑖍�
			if (std::string(layer_name) == std::string(layer_properties.layerName)) { is_layer_found = true; break; }
		}
		// ���C���[�����݂��Ȃ��ꍇ, ���C���[����\�����đ������I��
		if (!is_layer_found)
		{
			std::cout << "NOT FOUND VALIDATION LAYER: " << layer_name << std::endl << std::endl;;
			return false;
		}
	}

	return true;
}
// Vulkan: �����f�o�C�X���̕\��
void TriangleApplication::CheckPhysicalDeviceInfo(const VkPhysicalDeviceProperties& prop, const VkPhysicalDeviceFeatures& feature)
{
	// ��{���
	std::cout << "PHYSICAL DEVICE PROPERTIES" << std::endl;
	std::cout << "\t" << "API VERSION: " << VK_VERSION_MAJOR(prop.apiVersion) << "." << VK_VERSION_MINOR(prop.apiVersion) << "." << VK_VERSION_PATCH(prop.apiVersion) << std::endl;
	std::cout << "\t" << "DEVICE(Name, ID, Type): " << prop.deviceName << " , " << prop.deviceID << " , " << prop.deviceType << std::endl;
	std::cout << "\t" << "DRIVER VERSION: " << prop.driverVersion << std::endl;
	std::cout << "\t" << "VENDER ID: " << prop.vendorID << std::endl << std::endl;
	// �I�v�V�����@�\
	std::cout << "\t" << "GEOMETRY SHADER: " << std::boolalpha << static_cast<bool>(feature.geometryShader) << std::endl;
	std::cout << "\t" << "TESSELLEATION SHADER: " << std::boolalpha << static_cast<bool>(feature.tessellationShader) << std::endl;
}

/* --public-- */

// �R���X�g���N�^
// NOTE: �f�t�H���g�ݒ�
TriangleApplication::TriangleApplication() :
	m_window(nullptr), m_window_width(800), m_window_height(600), m_window_name("Vulkan Apps"),
	m_vk_instance(), m_debug_messanger(),
	m_surface(),
	m_physical_device(VK_NULL_HANDLE),
	m_logical_device(),
	m_graphics_queue(), m_present_queue(),
	m_swap_chain(), m_swap_chain_image_format(), m_swap_chain_extent()
{

}
// NOTE: �E�B���h�E�ݒ�
TriangleApplication::TriangleApplication(const int& width, const int& height, const std::string& name):
	m_window(nullptr), m_window_width(width), m_window_height(height), m_window_name(name),
	m_vk_instance(), m_debug_messanger(),
	m_surface(),
	m_physical_device(VK_NULL_HANDLE),
	m_logical_device(),
	m_graphics_queue(), m_present_queue(),
	m_swap_chain(), m_swap_chain_image_format(), m_swap_chain_extent()
{

}

TriangleApplication::~TriangleApplication()
{

}

void TriangleApplication::Run()
{
	// GLFW3�ɂ��E�B���h�E����
	InitWindow();
	// Vulkan������
	InitVulkan();
	// ���C������
	MainLoop();
	// ���\�[�X���
	CleanUp();
}