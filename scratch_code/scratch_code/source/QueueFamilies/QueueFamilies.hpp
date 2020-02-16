#ifndef __QUEUE_FAMILIES_HPP__
#define __QUEUE_FAMILIES_HPP__

#include <optional>
#include <iostream>

// �L���[�t�@�~���C���f�b�N�X
// NOTE: C++17�̋@�\��, �l���L�������肵�Ă���.
struct QueueFamilyIndices
{
	std::optional<uint32_t> graphics_family;

	inline bool isComplete() const { return graphics_family.has_value(); }
};

#endif