#pragma once
#include "Common.h"
#include "core_vulkan/resource/buffer.h"

#include <vulkan/vulkan.hpp>

namespace Core {
    class RayTracingModel;
    class RayTracingScene;
    class Model;
    class SphereCollect;
    class GLTFScene;
    class GLTFNode;

    class ModelAccelerationStructure {
        default_no_copy_move_construction(ModelAccelerationStructure);
    public:
        ~ModelAccelerationStructure();
    INNER_VISIBLE:
        vk::AccelerationStructureKHR bottom_level_acceleration_structure ;
        std::unique_ptr<Buffer> acceleration_structure_buffer;
    };

    class AccelerationStructureBuilder {
        default_no_copy_move_construction(AccelerationStructureBuilder)
    private:
        struct BuildInfo {
            BuildInfo(ModelAccelerationStructure &model_acceleration_structure) : model_acceleration_structure(model_acceleration_structure) {};
            ModelAccelerationStructure &model_acceleration_structure;
            std::vector<vk::AccelerationStructureGeometryDataKHR> geometry_datas {};
            std::vector<vk::AccelerationStructureGeometryKHR> geometries {};
            vk::AccelerationStructureBuildGeometryInfoKHR build {};
            std::vector<vk::AccelerationStructureBuildRangeInfoKHR> ranges {};
            vk::AccelerationStructureBuildSizesInfoKHR size {};
            vk::AccelerationStructureKHR uncompacted_acceleration_structure {};
            std::unique_ptr<Buffer> uncompacted_acceleration_structure_buffer {};
        };
    public:
        void add_model(std::shared_ptr<RayTracingModel> model);
        void add_scene(std::shared_ptr<RayTracingScene> scene);
        void build(bool allow_update = false) { build_update(false, allow_update); }
        void update() { build_update(true, true); }
        ~AccelerationStructureBuilder();
    private:
        void build_update(bool is_update, bool allow_update);
    INNER_VISIBLE:
        std::unique_ptr<Buffer> staging;
        uint64_t current_staging_size = 0;
        std::unique_ptr<Buffer> scratch;
        uint64_t current_scratch_size = 0;
        std::vector<std::shared_ptr<Model>> models;
        std::vector<std::shared_ptr<SphereCollect>> sphere_collects;
        std::vector<std::shared_ptr<GLTFScene>> gltf_scenes;
    };
}
