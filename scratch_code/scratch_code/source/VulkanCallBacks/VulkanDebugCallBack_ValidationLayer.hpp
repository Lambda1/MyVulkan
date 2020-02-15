#ifndef __VULKAN_DEBUG_CALLBACK_VALIDATION_LAYER__
#define __VULKAN_DEBUG_CALLBACK_VALIDATION_LAYER__

/*
	Validation Layer�p�̃R�[���o�b�N�֐��Q
	NOTE: DEBUG�Ŏg�p
*/

#include <vulkan/vulkan.h>

#include <iostream>

// NOTE: VKAPI_ATTR��VKAPI_CALL��Vulkan������ɍ쓮���邽�߂�signature.
// ����: ���b�Z�[�W�d�v�x, ���b�Z�[�W���e, �R�[���o�b�N�֐�, �R�[���o�b�N�֐����̓Ǝ��f�[�^�ւ̎Q��
static VKAPI_ATTR VkBool32 VKAPI_CALL Debug_Call_Back
(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT *p_call_back_data, void *p_user_data
)
{
	std::cerr << "VALIDATION LAYER: " << p_call_back_data->pMessage << std::endl;
	return VK_FALSE;
}

#endif