#pragma once

#include "illixr/phonebook.hpp"
#include "illixr/pose_prediction.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/threadloop.hpp"
#include "illixr/vk_util/display_sink.hpp"
#include "illixr/vk_util/render_pass.hpp"

namespace ILLIXR {
    class native_renderer : public threadloop {
    public:
        native_renderer(const std::string& name_, phonebook* pb);
        void _p_thread_setup() override;
        void _p_one_iteration() override;
    private:
        [[maybe_unused]] void recreate_swapchain();
        void create_swapchain_framebuffers();
        void record_command_buffer(uint32_t swapchain_image_index);
        void create_sync_objects();
        void create_depth_image(VkImage* depth_image, VmaAllocation* depth_image_allocation, VkImageView* depth_image_view);
        void create_offscreen_target(VkImage* offscreen_image, VmaAllocation* offscreen_image_allocation,
                                     VkImageView* offscreen_image_view,
                                     [[maybe_unused]] VkFramebuffer* offscreen_framebuffer);
        void create_offscreen_framebuffers();
        void create_app_pass();
        void create_timewarp_pass();

        const std::shared_ptr<switchboard>         sb;
        const std::shared_ptr<pose_prediction>     pp;
        const std::shared_ptr<display_sink>        ds;
        const std::shared_ptr<timewarp>            tw;
        const std::shared_ptr<app>                 src;
        const std::shared_ptr<const RelativeClock> _m_clock;

        VkCommandPool   command_pool{};
        VkCommandBuffer app_command_buffer{};
        VkCommandBuffer timewarp_command_buffer{};

        std::array<VkImage, 2>       depth_images{};
        std::array<VmaAllocation, 2> depth_image_allocations{};
        std::array<VkImageView, 2>   depth_image_views{};

        std::array<VkImage, 2>       offscreen_images{};
        std::array<VmaAllocation, 2> offscreen_image_allocations{};
        std::array<VkImageView, 2>   offscreen_image_views{};
        std::array<VkFramebuffer, 2> offscreen_framebuffers{};

        std::vector<VkFramebuffer> swapchain_framebuffers;

        VkRenderPass app_pass{};
        VkRenderPass timewarp_pass{};

        VkSemaphore image_available_semaphore{};
        VkSemaphore app_render_finished_semaphore{};
        VkSemaphore timewarp_render_finished_semaphore{};
        VkFence     frame_fence{};

        uint64_t timeline_semaphore_value = 1;

        int        fps{};
        time_point last_fps_update;

    };
}