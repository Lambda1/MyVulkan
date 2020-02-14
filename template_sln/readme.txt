Vulkan + GLFW + GLMの最小プログラム構成

1. プロパティを自分の環境に合わせて変更すること.
(GLFWとGLMは./my_vulkan/Libraries に配置済みのため, VulkanSDKだけの変更で行けるはず)

追記
	.gitignoreの設定で, glfwの.libや.dllが設置されていないため, 自身の環境に合わせて配置すること
	document/3_development_environment.txtに設定方法を記述してある.

環境
OS:  Windows10
IDE: Visual Studio 2019
ライブラリ:
	Vulkan 1.2.131.2
	GLFW 3.3.2
	GLM 0.9.9.2
