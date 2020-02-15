#ifndef __VULKAN_DEBUG_CALLBACK_VALIDATION_LAYER__HPP__
#define __VULKAN_DEBUG_CALLBACK_VALIDATION_LAYER__HPP__

/*
	Validation Layer�p�̊֐��Q
	NOTE: DEBUG�Ŏg�p
*/

#include <vulkan/vulkan.h>

#include <iostream>

// �f�o�b�O�p�R�[���o�b�N�֐�
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

// vkCreateDebugUtilsMessengerEXT�̂��߂̏����֐�
// NOTE: �g���@�\�̂���, vkGetInstanceProcAddr�����K�v������.
inline VkResult Create_Debug_Utils_Messenger_EXT
(
	VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
	const VkAllocationCallbacks *p_allocator, VkDebugUtilsMessengerEXT *p_debug_messanger
)
{
	// vkCreateDebugUtilsMessengerEXT�֐�
	auto create_func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
	std::cout << create_func << std::endl;
	if (create_func != nullptr) { return create_func(instance, p_create_info, p_allocator, p_debug_messanger); }
	else { return VK_ERROR_EXTENSION_NOT_PRESENT; }
}

// vkDestroyDebugUtilsMessengerEXT�̂��߂̏����֐�
// NOTE: �g���@�\�̂���, vkGetInstanceProcAddr�����K�v������.
inline void Destroy_Debug_Utils_Messenger_EXT
(
	VkInstance instance, VkDebugUtilsMessengerEXT debug_messanger,
	const VkAllocationCallbacks *p_call_back
)
{
	// vkDestroyDebugUtilsMessengerEXT�֐�
	auto destroy_func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (destroy_func != nullptr) { destroy_func(instance, debug_messanger, p_call_back); }
}

#endif