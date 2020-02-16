#ifndef __QUEUE_FAMILIES_HPP__
#define __QUEUE_FAMILIES_HPP__

#include <iostream>

// キューファミリインデックス
struct QueueFamilyIndices
{
	uint32_t graphics_family;
	QueueFamilyIndices() : graphics_family() {}
	~QueueFamilyIndices() {};
};

#endif