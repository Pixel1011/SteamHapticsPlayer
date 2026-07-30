#pragma once
#ifdef _WIN32

#include <string>
#include <string_view>

namespace text
{
    [[nodiscard]] std::wstring to_wide(std::string_view utf8);

    [[nodiscard]] std::string to_utf8(std::wstring_view wide);
}

#endif