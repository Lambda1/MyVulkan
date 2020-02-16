#ifndef __QUEUE_FAMILIES_HPP__
#define __QUEUE_FAMILIES_HPP__

#include <optional>
#include <iostream>

// キューファミリインデックス
// NOTE: C++17の機能で, 値が有効か判定している.
struct QueueFamilyIndices
{
	std::optional<uint32_t> graphics_family;

	inline bool isComplete() const { return graphics_family.has_value(); }
};

#endif