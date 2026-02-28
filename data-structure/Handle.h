#pragma once
namespace HBE
{
    struct Handle
    {
        uint32_t index = std::numeric_limits<uint32_t>::max();
        uint32_t version = std::numeric_limits<uint32_t>::max();

        bool operator==(Handle other) const
        {
            return index == other.index && version == other.version;
        }
    };
}
