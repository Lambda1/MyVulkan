#ifndef __SWAP_CHAIN_SUPPORT_DETAILS_HPP__
#define __SWAP_CHAIN_SUPPORT_DETAILS_HPP__

/*
	Swap Chainê›íË
*/

#include <vulkan/vulkan.h>

#include <vector>

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR m_capabillites;
	std::vector<VkSurfaceFormatKHR> m_formats;
	std::vector<VkPresentModeKHR> m_present_modes;

	SwapChainSupportDetails():
	m_capabillites() {}
};

#endif