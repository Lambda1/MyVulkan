#include "./TriangleApplication.hpp"

/* --private-- */

// ����������
void TriangleApplication::InitVulkan()
{
	// �C���X�^���X����
	CreateInstance();
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

/* --Vulkan-- */
// Vulkan�C���X�^���X�𐶐�
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

// �R���X�g���N�^
// NOTE: �f�t�H���g�ݒ�
TriangleApplication::TriangleApplication() :
	m_window(nullptr), m_window_width(800), m_window_height(600), m_window_name("Vulkan"),
	m_vk_instance()
{

}
// NOTE: �E�B���h�E�ݒ�
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
	// GLFW3�ɂ��E�B���h�E����
	InitWindow();
	// Vulkan������
	InitVulkan();
	// ���C������
	MainLoop();
	// ���\�[�X���
	CleanUp();
}