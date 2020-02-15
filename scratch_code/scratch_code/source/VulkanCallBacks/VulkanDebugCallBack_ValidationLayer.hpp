#ifndef __VULKAN_DEBUG_CALLBACK_VALIDATION_LAYER__
#define __VULKAN_DEBUG_CALLBACK_VALIDATION_LAYER__

/*
	Validation Layer用のコールバック関数群
	NOTE: DEBUGで使用
*/

#include <vulkan/vulkan.h>

#include <iostream>

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

#endif