#pragma once
#include "UUID.hpp"
#include "UUIDGenerator.hpp"
#include <gtest/gtest_prod.h>
#include <nlohmann/json.hpp>
#include <string>

using namespace MEngine::Core;
namespace MEngine::Resource
{
class Asset
{
    friend struct nlohmann::adl_serializer<Asset>;
    friend class AssetManager;

  protected:
    UUID mID{};
    std::string mName{"Unnamed"};
    Asset() : mID(UUIDGenerator::Instance().Create()), mName("Unnamed")
    {
    }

  public:
    virtual ~Asset() = default;
    virtual inline const UUID &GetID() const
    {
        return mID;
    }
    virtual inline const std::string &GetName() const
    {
        return mName;
    }
};
} // namespace MEngine::Resource

namespace nlohmann
{
using namespace MEngine::Resource;
template <> struct adl_serializer<UUID>
{
    static void to_json(json &j, const UUID &p)
    {
        j = p.ToString();
    };
    static void from_json(const json &j, UUID &p)
    {
        p = UUID(j.get<std::string>());
    }
};
template <> struct adl_serializer<Asset>
{
    static void to_json(json &j, const Asset &p)
    {
        j["ID"] = p.mID;
        j["Name"] = p.mName;
    };
    static void from_json(const json &j, Asset &p)
    {
        p.mID = j.value("ID", UUID());
        p.mName = j.value("Name", "Unnamed");
    }
};
} // namespace nlohmann