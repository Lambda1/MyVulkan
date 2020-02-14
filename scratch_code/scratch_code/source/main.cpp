#include <vulkan/vulkan.h>

class TriangleApplication
{
private:
	// Vulkan初期化
	void InitVulkan()
	{

	}

	// プログラム中のMain
	void MainLoop()
	{

	}

	// リソース解放
	void CleanUp()
	{

	}
public:
	// 実行
	// NOTE: 初期化 -> MainLoop -> リソース解放
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

	// レンダリング開始
	my_triangle.Run();

	return 0;
}