#pragma once
#include <stack>

#define VMA_IMPLEMENTATION

#include "illixr/pose_prediction.hpp"
#include "illixr/threadloop.hpp"
#include "illixr/vk_util/display_sink.hpp"
#include "illixr/vk_util/render_pass.hpp"

#include "utils/hmd.hpp"

namespace ILLIXR {
    namespace Timewarp_VK {
        struct Vertex {
            glm::vec3 pos;
            glm::vec2 uv0;
            glm::vec2 uv1;
            glm::vec2 uv2;

            static VkVertexInputBindingDescription get_binding_description();

            static std::array<VkVertexInputAttributeDescription, 4> get_attribute_descriptions();

        };

        struct UniformBufferObject {
            glm::mat4 timewarp_start_transform;
            glm::mat4 timewarp_end_transform;
        };
    }

    class timewarp_vk : public timewarp {
    public:
        explicit timewarp_vk(const phonebook* const pb_);

        void destroy() override;
        void record_command_buffer(VkCommandBuffer commandBuffer, int buffer_ind, bool left) override;
        void update_uniforms(const pose_type& render_pose) override;
        void partial_destroy();
        void setup(VkRenderPass render_pass, uint32_t subpass, std::array<std::vector<VkImageView>, 2> buffer_pool_in,
                   bool input_texture_vulkan_coordinates_in) override;
        void initialize();
    private:
        void create_vertex_buffer();
        void create_index_buffer();
        void generate_distortion_data();
        void create_texture_sampler();
        void create_descriptor_set_layout();
        void create_uniform_buffer();
        void create_descriptor_pool();
        void create_descriptor_sets();
        VkPipeline create_pipeline(VkRenderPass render_pass, [[maybe_unused]] uint32_t subpass);
        void build_timewarp(HMD::hmd_info_t& hmdInfo);
        static void calculate_timewarp_transform(Eigen::Matrix4f& transform, const Eigen::Matrix4f& renderProjectionMatrix,
                                                 const Eigen::Matrix4f& renderViewMatrix, const Eigen::Matrix4f& newViewMatrix);

        const phonebook* const                 pb;
        const std::shared_ptr<switchboard>     sb;
        const std::shared_ptr<pose_prediction> pp;
        bool                                   disable_warp = false;
        std::shared_ptr<display_sink>          ds           = nullptr;
        std::mutex                             m_setup;

        bool initialized                      = false;
        bool input_texture_vulkan_coordinates = true;

        // Vulkan resources
        std::stack<std::function<void()>> deletion_queue;
        VmaAllocator                      vma_allocator{};

        std::array<std::vector<VkImageView>, 2> buffer_pool;
        VkSampler                               fb_sampler{};

        VkDescriptorPool                            descriptor_pool{};
        VkDescriptorSetLayout                       descriptor_set_layout{};
        std::array<std::vector<VkDescriptorSet>, 2> descriptor_sets;

        VkPipelineLayout  pipeline_layout{};
        VkBuffer          uniform_buffer{};
        VmaAllocation     uniform_alloc{};
        VmaAllocationInfo uniform_alloc_info{};

        VkCommandPool   command_pool{};
        VkCommandBuffer command_buffer{};

        VkBuffer vertex_buffer{};
        VkBuffer index_buffer{};

        // distortion data
        HMD::hmd_info_t hmd_info{};

        uint32_t                         num_distortion_vertices{};
        uint32_t                         num_distortion_indices{};
        Eigen::Matrix4f                  basicProjection;
        std::vector<HMD::mesh_coord3d_t> distortion_positions;
        std::vector<HMD::uv_coord_t>     distortion_uv0;
        std::vector<HMD::uv_coord_t>     distortion_uv1;
        std::vector<HMD::uv_coord_t>     distortion_uv2;

        std::vector<uint32_t> distortion_indices;

        // metrics
        std::atomic<uint32_t> num_record_calls{0};
        std::atomic<uint32_t> num_update_uniforms_calls{0};

        friend class timewarp_vk_plugin;
    };

    class timewarp_vk_plugin : public threadloop {
    public:
        timewarp_vk_plugin(const std::string& name_, phonebook* pb_);
        skip_option _p_should_skip() override;
        void _p_one_iteration() override;
    private:
        std::shared_ptr<timewarp_vk>  tw;
        std::shared_ptr<display_sink> ds;

        int64_t last_print = 0;

    };
}