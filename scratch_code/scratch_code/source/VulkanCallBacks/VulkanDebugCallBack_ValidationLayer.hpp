#ifndef __VULKAN_DEBUG_CALLBACK_VALIDATION_LAYER__HPP__
#define __VULKAN_DEBUG_CALLBACK_VALIDATION_LAYER__HPP__

/*
	Validation Layer用の関数群
	NOTE: DEBUGで使用
*/

#include <vulkan/vulkan.h>

#include <iostream>

// デバッグ用コールバック関数
// NOTE: VKAPI_ATTRとVKAPI_CALLはVulkanが正常に作動するためのsignature.
// 引数: メッセージ重要度, メッセージ内容, コールバック関数, コールバック関数中の独自データへの参照
static VKAPI_ATTR VkBool32 VKAPI_CALL Debug_Call_Back
(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT *p_call_back_data, void *p_user_data
)
{
	std::cerr << "VALIDATION LAYER: " << p_call_back_data->pMessage << std::endl;
	return VK_FALSE;
}

// vkCreateDebugUtilsMessengerEXTのための処理関数
// NOTE: 拡張機能のため, vkGetInstanceProcAddrを介する必要がある.
inline VkResult Create_Debug_Utils_Messenger_EXT
(
	VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
	const VkAllocationCallbacks *p_allocator, VkDebugUtilsMessengerEXT *p_debug_messanger
)
{
	// vkCreateDebugUtilsMessengerEXT関数
	auto create_func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
	std::cout << create_func << std::endl;
	if (create_func != nullptr) { return create_func(instance, p_create_info, p_allocator, p_debug_messanger); }
	else { return VK_ERROR_EXTENSION_NOT_PRESENT; }
}

// vkDestroyDebugUtilsMessengerEXTのための処理関数
// NOTE: 拡張機能のため, vkGetInstanceProcAddrを介する必要がある.
inline void Destroy_Debug_Utils_Messenger_EXT
(
	VkInstance instance, VkDebugUtilsMessengerEXT debug_messanger,
	const VkAllocationCallbacks *p_call_back
)
{
	// vkDestroyDebugUtilsMessengerEXT関数
	auto destroy_func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (destroy_func != nullptr) { destroy_func(instance, debug_messanger, p_call_back); }
}

#endif