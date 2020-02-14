#include "./TriangleApplication/TriangleApplication.hpp"

#include <iostream>
#include <stdexcept>

#define RETURN_MAIN_FAILED 1
#define RETURN_MAIN_SUCCESS 0

int main(int argc, char* argv[])
{
	TriangleApplication my_triangle;

	try
	{
		// レンダリング開始
		my_triangle.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return RETURN_MAIN_FAILED;
	}

	return RETURN_MAIN_SUCCESS;
}