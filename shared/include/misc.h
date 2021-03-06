#pragma once

#include <stdexcept>
#include <optional>
#include <stdint.h>
#include <string>

inline std::optional<std::uint64_t> u64_from_hex(const std::string& str_)
{
        std::uint64_t val = 0;
        try
        {
                const std::string str{str_, 2};
                val = std::stoull(str, nullptr, 16);
        }
        catch(const std::exception&)
        {
                return std::nullopt;
        }

        return val;
}

template<typename Cont, typename Key>
bool contains(const Cont& c, const Key& k)
{
        return c.find(k) != c.end();
}
