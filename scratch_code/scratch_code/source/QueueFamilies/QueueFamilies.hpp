#ifndef __QUEUE_FAMILIES_HPP__
#define __QUEUE_FAMILIES_HPP__

#include <optional>
#include <iostream>

// �L���[�t�@�~���C���f�b�N�X
// NOTE: C++17�̋@�\��, �l���L�������肵�Ă���.
struct QueueFamilyIndices
{
	// NOTE: �L���[�t�@�~���C���f�b�N�X�͓����l�ɂȂ�\��������.
	std::optional<uint32_t> graphics_family;
	std::optional<uint32_t> present_family;

	inline bool isComplete() const { return graphics_family.has_value() && present_family.has_value(); }
};

#endif