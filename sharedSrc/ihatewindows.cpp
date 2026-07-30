
#ifdef _WIN32
#include "ihatewindows.h"


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include <Windows.h>

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <system_error>

namespace
{
    int checked_size(std::size_t size)
    {
        if (size > static_cast<std::size_t>(std::numeric_limits<int>::max()))
            throw std::length_error("String is too large to convert");

        return static_cast<int>(size);
    }

    [[noreturn]] void throw_last_error(const char* message)
    {
        const DWORD error = ::GetLastError();
        throw std::system_error(
            static_cast<int>(error),
            std::system_category(),
            message
        );
    }
}

std::wstring text::to_wide(std::string_view utf8)
{
    if (utf8.empty())
        return {};

    const int input_size = checked_size(utf8.size());

    const int output_size = ::MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        utf8.data(),
        input_size,
        nullptr,
        0
    );

    if (output_size == 0)
        throw_last_error("MultiByteToWideChar failed");

    std::wstring output(static_cast<std::size_t>(output_size), L'\0');

    if (::MultiByteToWideChar(
            CP_UTF8,
            MB_ERR_INVALID_CHARS,
            utf8.data(),
            input_size,
            output.data(),
            output_size
        ) == 0)
    {
        throw_last_error("MultiByteToWideChar failed");
    }

    return output;
}

std::string text::to_utf8(std::wstring_view wide)
{
    if (wide.empty())
        return {};

    const int input_size = checked_size(wide.size());

    const int output_size = ::WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        wide.data(),
        input_size,
        nullptr,
        0,
        nullptr,
        nullptr
    );

    if (output_size == 0)
        throw_last_error("WideCharToMultiByte failed");

    std::string output(static_cast<std::size_t>(output_size), '\0');

    if (::WideCharToMultiByte(
            CP_UTF8,
            WC_ERR_INVALID_CHARS,
            wide.data(),
            input_size,
            output.data(),
            output_size,
            nullptr,
            nullptr
        ) == 0)
    {
        throw_last_error("WideCharToMultiByte failed");
    }

    return output;
}

#endif