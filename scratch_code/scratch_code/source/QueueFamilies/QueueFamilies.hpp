#ifndef __QUEUE_FAMILIES_HPP__
#define __QUEUE_FAMILIES_HPP__

#include <iostream>

// �L���[�t�@�~���C���f�b�N�X
struct QueueFamilyIndices
{
	uint32_t graphics_family;
	QueueFamilyIndices() : graphics_family() {}
	~QueueFamilyIndices() {};
};

#endif