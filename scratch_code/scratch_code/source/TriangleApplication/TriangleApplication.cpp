#include "./TriangleApplication.hpp"

// private

// ����������
void TriangleApplication::InitVulkan()
{
	/* --GLFW3�̏���������-- */
	// OpenGL�p�̃R���e�L�X�g�𐶐����Ȃ�
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// �����_�ł�, �E�B���h�E�T�C�Y�̕ύX���l�����Ȃ�
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// �E�B���h�E����
	m_window = glfwCreateWindow(m_window_width, m_window_height, m_window_name.c_str(), nullptr, nullptr);
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
	/* --GLFW3�̏I������--*/
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

// public

// �R���X�g���N�^
// NOTE: �f�t�H���g�ݒ�
TriangleApplication::TriangleApplication() :
	m_window(nullptr), m_window_width(800), m_window_height(600), m_window_name("Vulkan")
{

}
// NOTE: �E�B���h�E�ݒ�
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