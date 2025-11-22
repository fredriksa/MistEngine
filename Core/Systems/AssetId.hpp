#pragma once

namespace Core
{
    class AssetId
    {
    public:
        AssetId()
        {
            static int LastId = 0;
            Id = LastId;
            ++LastId;
        }

        int Get() const { return Id; }
        bool IsValid() const { return Id != 0; }

        bool operator==(const AssetId& Other) const { return Id == Other.Id; }
        bool operator!=(const AssetId& Other) const { return !(*this == Other); }

        static const AssetId Zero;

    private:
        int Id;
    };
}

namespace std
{
    template <>
    struct hash<Core::AssetId>
    {
        size_t operator()(const Core::AssetId& AssetId) const noexcept
        {
            return std::hash<int>{}(AssetId.Get());
        }
    };
}
