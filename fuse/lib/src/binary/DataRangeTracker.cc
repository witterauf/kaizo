#include <fuse/binary/DataRangeTracker.h>
#include <fuse/lua/LuaWriter.h>

namespace fuse {

void TagOnlyRangeTracker::track(const binary::DataPath&, const Range& range,
                                const std::optional<std::string>& tag)
{
    if (tag.has_value())
    {
        auto map = m_maps.find(*tag);
        if (map == m_maps.end())
        {
            auto result = m_maps.insert(std::make_pair(*tag, UsageMap{}));
            map = result.first;
        }
        auto end = range.offset + range.size;
        end = end % m_alignment == 0 ? end : end + (m_alignment - (end % m_alignment));
        map->second.allocate(range.offset, end);
    }
}

void TagOnlyRangeTracker::serialize(LuaWriter& writer) const
{
    writer.start();
    for (auto const& mapPair : m_maps)
    {
        writer.startField(mapPair.first).startTable();
        for (auto const& range : mapPair.second)
        {
            writer.startField().startTable();
            writer.startField("start").writeInteger(range.first).finishField();
            writer.startField("size").writeInteger(range.second - range.first).finishField();
            writer.finishTable().finishField();
        }
        writer.finishTable().finishField();
    }
    writer.finish();
}

} // namespace fuse