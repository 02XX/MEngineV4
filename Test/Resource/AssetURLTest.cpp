#include "AssetURL.hpp"
#include <gtest/gtest.h>
using namespace MEngine::Resource;

TEST(AssetURLTest, ParseValidAssetURL)
{
    AssetURL url("asset://path/to/asset");
    EXPECT_EQ(url.GetProtocol(), Protocol::ASSET);
    EXPECT_EQ(url.GetPath(), AssetURL::GetBasePath(Protocol::ASSET) / "path/to/asset");
}
TEST(AssetURLTest, ParseValidHttpURL)
{
    AssetURL url("http://example.com/asset");
    EXPECT_EQ(url.GetProtocol(), Protocol::HTTP);
    EXPECT_EQ(url.GetPath(), AssetURL::GetBasePath(Protocol::HTTP) / "example.com/asset");
}
TEST(AssetURLTest, ParseValidHttpsURL)
{
    AssetURL url("https://example.com/asset");
    EXPECT_EQ(url.GetProtocol(), Protocol::HTTPS);
    EXPECT_EQ(url.GetPath(), AssetURL::GetBasePath(Protocol::HTTPS) / "example.com/asset");
}