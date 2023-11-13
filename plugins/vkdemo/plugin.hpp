#pragma once
#include "illixr/data_format.hpp"
#include "illixr/plugin.hpp"
#include "illixr/pose_prediction.hpp"
#include "illixr/switchboard.hpp"

#include "illixr/vk_util/render_pass.hpp"
#include "illixr/vk_util/display_sink.hpp"

#include <map>

namespace ILLIXR::VKdemo {
        struct Vertex {
            glm::vec3 pos;
            glm::vec2 uv;

            static VkVertexInputBindingDescription get_binding_description() {
                VkVertexInputBindingDescription binding_description{
                        0,                          // binding
                        sizeof(Vertex),             // stride
                        VK_VERTEX_INPUT_RATE_VERTEX // inputRate
                };
                return binding_description;
            }

            static std::array<VkVertexInputAttributeDescription, 2> get_attribute_descriptions() {
                std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions{{{
                                                                                                 0,                          // location
                                                                                                 0,                          // binding
                                                                                                 VK_FORMAT_R32G32B32_SFLOAT, // format
                                                                                                 offsetof(Vertex,
                                                                                                          pos)       // offset
                                                                                         },
                                                                                         {
                                                                                                 1,                       // location
                                                                                                 0,                       // binding
                                                                                                 VK_FORMAT_R32G32_SFLOAT, // format
                                                                                                 offsetof(Vertex,
                                                                                                          uv)     // offset
                                                                                         }}};

                return attribute_descriptions;
            }

            bool operator==(const Vertex &other) const {
                return pos == other.pos && uv == other.uv;
            }
        };

        struct Texture {
            VkImage image;
            VmaAllocation image_memory;
            VkImageView image_view;
        };

        struct Model {
            int texture_index;
            uint32_t vertex_offset;
            uint32_t index_offset;
            uint32_t index_count;
        };

        struct ModelPushConstant {
            int texture_index;
        };


        struct UniformBufferObject {
            glm::mat4 model_view;
            glm::mat4 proj;
        };
    }
namespace std {
    template<>
    struct hash<ILLIXR::VKdemo::Vertex> {
        size_t operator()(ILLIXR::VKdemo::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec2>()(vertex.uv) << 1)) >> 1);
        }
    };
} // namespace std

namespace ILLIXR {
    class vkdemo : public app {
    public:
        explicit vkdemo(const phonebook* const pb_);

        void initialize();
        void setup(VkRenderPass render_pass, uint32_t subpass) override;
        void update_uniforms(const pose_type& fp) override;
        void record_command_buffer(VkCommandBuffer commandBuffer, int eye) override;
        void destroy() override {}
    private:
        void update_uniform(const pose_type& fp, int eye);
        void bake_models();
        void create_descriptor_set_layout();
        void create_uniform_buffers();
        void create_descriptor_pool();
        void create_texture_sampler();
        void create_descriptor_set();
        void load_texture(const std::string& path, int i);
        void image_layout_transition(VkImage image, [[maybe_unused]] VkFormat format, VkImageLayout old_layout,
                                     VkImageLayout new_layout);
        void load_model();
        void create_vertex_buffer();
        void create_index_buffer();
        void create_pipeline(VkRenderPass render_pass, uint32_t subpass);

        const std::shared_ptr<switchboard>         sb;
        const std::shared_ptr<pose_prediction>     pp;
        const std::shared_ptr<display_sink>        ds = nullptr;
        const std::shared_ptr<const RelativeClock> _m_clock;

        Eigen::Matrix4f       basic_projection;
        std::vector<VKdemo::Model>    models;
        std::vector<VKdemo::Vertex>   vertices;
        std::vector<uint32_t> indices;

        std::array<std::vector<VkImageView>, 2> buffer_pool;

        VmaAllocator    vma_allocator{};
        VkCommandPool   command_pool{};
        VkCommandBuffer command_buffer{};

        VkDescriptorSetLayout          descriptor_set_layout{};
        VkDescriptorPool               descriptor_pool{};
        std::array<VkDescriptorSet, 2> descriptor_sets{};

        std::array<VkBuffer, 2>          uniform_buffers{};
        std::array<VmaAllocation, 2>     uniform_buffer_allocations{};
        std::array<VmaAllocationInfo, 2> uniform_buffer_allocation_infos{};

        VkBuffer vertex_buffer{};
        VkBuffer index_buffer{};

        VkPipelineLayout pipeline_layout{};

        std::vector<VkDescriptorImageInfo> image_infos;
        std::vector<VKdemo::Texture>               textures;
        VkSampler                          texture_sampler{};
        std::map<uint32_t, uint32_t>       texture_map;

    };

    class vkdemo_plugin : public plugin {
    public:
        vkdemo_plugin(const std::string& name_, phonebook* pb_);
        void start() override;
    private:
        std::shared_ptr<vkdemo> vkd;
    };
}