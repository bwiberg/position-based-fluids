#pragma once

template <typename T>
inline const T &clamp(const T &value, const T &min, const T &max) {
    return std::max(min, std::min(max, value));
}
