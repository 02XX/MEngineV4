#pragma once
#include <rfl.hpp>
#include <rfl/Flatten.hpp>
#include <rfl/as.hpp>
#include <rfl/flexbuf.hpp>
#include <rfl/json.hpp>
#include <vector>

#include "Asset.hpp"
#include "AssetManager.hpp"
#include "GraphicPipeline.hpp"
#include "Pipeline.hpp"
#include "Shader.hpp"
#include "UUID.hpp"
namespace rfl
{
template <> struct Reflector<MEngine::Core::UUID>
{
    using ReflType = std::string;
    static MEngine::Core::UUID to(const ReflType &str) noexcept
    {
        return MEngine::Core::UUID(str);
    }
    static ReflType from(const MEngine::Core::UUID &uuid)
    {
        return uuid.ToString();
    }
};
} // namespace rfl
namespace MEngine::Resource
{
struct AssetEntity
{
    Core::UUID id;
    std::string name{"Unnamed"};
};
struct ShaderEntity
{
    rfl::Flatten<AssetEntity> assetEntity;
    vk::ShaderStageFlagBits stage;
    std::vector<uint32_t> spirvCode;
};
struct PipelineEntity
{
    rfl::Flatten<AssetEntity> assetEntity;
    std::vector<std::string> mDescriptorSetLayouts{};
    std::vector<std::string> mPushConstantRanges{};
};
struct GraphicPipelineEntity
{
    rfl::Flatten<PipelineEntity> pipelineEntity;
    std::vector<Core::UUID> shaderIDs;
};
template <typename TAsset, typename TAssetEntity> struct Transformer
{
    static TAssetEntity ToEntity(std::shared_ptr<Asset> asset)
    {
        throw std::runtime_error("Not implemented");
    }
    static std::shared_ptr<Asset> FromEntity(const TAssetEntity &entity)
    {
        throw std::runtime_error("Not implemented");
    }
};
template <> struct Transformer<Asset, AssetEntity>
{
    static AssetEntity ToEntity(std::shared_ptr<Asset> asset)
    {
        AssetEntity entity;
        entity.id = asset->mID;
        entity.name = asset->mName;
        return entity;
    }
    static std::shared_ptr<Asset> FromEntity(const AssetEntity &entity)
    {
        auto asset = std::make_shared<Asset>(entity.name);
        asset->mID = entity.id;
        return asset;
    }
};
template <> struct Transformer<Shader, ShaderEntity>
{
    static ShaderEntity ToEntity(std::shared_ptr<Asset> asset)
    {
        auto shader = std::static_pointer_cast<Shader>(asset);
        ShaderEntity entity{
            .assetEntity = Transformer<Asset, AssetEntity>::ToEntity(asset),
            .stage = shader->mStage,
            .spirvCode = shader->mSPIRVCode,
        };
        return entity;
    }
    static std::shared_ptr<Asset> FromEntity(const ShaderEntity &entity)
    {
        auto shader = std::make_shared<Shader>(entity.assetEntity.value_.name, entity.spirvCode, entity.stage);
        shader->mID = entity.assetEntity.value_.id;
        return shader;
    }
};
template <> struct Transformer<Pipeline, PipelineEntity>
{
    static PipelineEntity ToEntity(std::shared_ptr<Asset> asset)
    {
        auto pipeline = std::static_pointer_cast<Pipeline>(asset);
        PipelineEntity entity{
            .assetEntity = Transformer<Asset, AssetEntity>::ToEntity(asset),
            .mDescriptorSetLayouts = pipeline->mDescriptorSetLayouts,
            .mPushConstantRanges = pipeline->mPushConstantRanges,
        };
        return entity;
    }
    static std::shared_ptr<Asset> FromEntity(const PipelineEntity &entity)
    {
        throw std::runtime_error("Pipeline deserialization not implemented");
    }
};
template <> struct Transformer<GraphicPipeline, GraphicPipelineEntity>
{
    static GraphicPipelineEntity ToEntity(std::shared_ptr<Asset> asset)
    {
        auto graphicPipeline = std::static_pointer_cast<GraphicPipeline>(asset);
        std::vector<Core::UUID> shaderIDs;
        for (const auto &shader : graphicPipeline->mShaders)
        {
            shaderIDs.push_back(shader->mID);
        }
        GraphicPipelineEntity entity{
            .pipelineEntity = Transformer<Pipeline, PipelineEntity>::ToEntity(asset),
            .shaderIDs = shaderIDs,
        };
        return entity;
    }
    static std::shared_ptr<Asset> FromEntity(const GraphicPipelineEntity &entity)
    {
        std::vector<std::shared_ptr<Shader>> shaders;
        for (const auto &shaderID : entity.shaderIDs)
        {
            auto shader = AssetManager::Instance().GetAs<Shader>(shaderID);
            shaders.push_back(shader);
        }
        auto pipeline = std::make_shared<GraphicPipeline>(
            entity.pipelineEntity.value_.assetEntity.value_.name, entity.pipelineEntity.value_.mDescriptorSetLayouts,
            entity.pipelineEntity.value_.mPushConstantRanges, vk::VertexInputBindingDescription{},
            std::vector<vk::VertexInputAttributeDescription>{}, vk::PipelineInputAssemblyStateCreateInfo{}, shaders,
            std::vector<vk::Format>{}, vk::Format{}, std::vector<vk::PipelineColorBlendAttachmentState>{},
            vk::PipelineMultisampleStateCreateInfo{});
        pipeline->mID = entity.pipelineEntity.value_.assetEntity.value_.id;
        return pipeline;
    }
};
} // namespace MEngine::Resource