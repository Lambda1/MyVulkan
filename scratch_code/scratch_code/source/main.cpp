#include <vulkan/vulkan.h>

class TriangleApplication
{
private:
	// Vulkan������
	void InitVulkan()
	{

	}

	// �v���O��������Main
	void MainLoop()
	{

	}

	// ���\�[�X���
	void CleanUp()
	{

	}
public:
	// ���s
	// NOTE: ������ -> MainLoop -> ���\�[�X���
	void Run()
	{
		InitVulkan();
		MainLoop();
		CleanUp();
	}
};

int main(int argc, char* argv[])
{
	TriangleApplication my_triangle;

	// �����_�����O�J�n
	my_triangle.Run();

	return 0;
}