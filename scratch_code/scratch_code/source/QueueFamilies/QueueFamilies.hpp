#ifndef __QUEUE_FAMILIES_HPP__
#define __QUEUE_FAMILIES_HPP__

#include <optional>
#include <iostream>

// キューファミリインデックス
// NOTE: C++17の機能で, 値が有効か判定している.
struct QueueFamilyIndices
{
	// NOTE: キューファミリインデックスは同じ値になる可能性が高い.
	std::optional<uint32_t> graphics_family;
	std::optional<uint32_t> present_family;

	inline bool isComplete() const { return graphics_family.has_value() && present_family.has_value(); }
};

#endif