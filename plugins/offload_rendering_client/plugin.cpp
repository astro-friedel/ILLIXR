#include "illixr/data_format.hpp"
#include "illixr/phonebook.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/threadloop.hpp"
#include "illixr/vk/display_provider.hpp"
#include "illixr/vk/vk_extension_request.h"
#include "illixr/vk/vulkan_utils.hpp"
#include "illixr/vk/render_pass.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/hwcontext.h>
#include <libavutil/hwcontext_vulkan.h>
}
#include <cstdlib>
#include <set>

using namespace ILLIXR;

void AV_ASSERT_SUCCESS(int ret) {
    if (ret < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, errbuf, AV_ERROR_MAX_STRING_SIZE);
        throw std::runtime_error{std::string{"FFmpeg error: "} + errbuf};
    }
}

class offload_rendering_client_service : public vulkan::app {
public:
    void update_uniforms(const pose_type& render_pose) override { }

    void setup(VkRenderPass render_pass, uint32_t subpass, std::shared_ptr<vulkan::buffer_pool<pose_type>> buffer_pool) override { }

    void record_command_buffer(VkCommandBuffer commandBuffer, int eye) override { }

    void destroy() override { }
};

class offload_rendering_client
    : public threadloop
    , public vulkan::vk_extension_request {
public:
    offload_rendering_client(const std::string& name, phonebook* pb)
        : threadloop{name, pb}
        , dp{pb->lookup_impl<vulkan::display_provider>()}
        , log{spdlogger(nullptr)} {

    }

    void start() override {
        auto ref              = av_hwdevice_ctx_alloc(AV_HWDEVICE_TYPE_VULKAN);
        auto hwdev_ctx        = reinterpret_cast<AVHWDeviceContext*>(ref->data);
        auto vulkan_hwdev_ctx = reinterpret_cast<AVVulkanDeviceContext*>(hwdev_ctx->hwctx);

        vulkan_hwdev_ctx->inst            = dp->vk_instance;
        vulkan_hwdev_ctx->phys_dev        = dp->vk_physical_device;
        vulkan_hwdev_ctx->act_dev         = dp->vk_device;
        vulkan_hwdev_ctx->device_features = dp->features;
        for (auto& queue : dp->queues) {
            switch (queue.first) {
            case vulkan::vulkan_utils::queue::GRAPHICS:
                vulkan_hwdev_ctx->queue_family_index      = queue.second.family;
                vulkan_hwdev_ctx->nb_graphics_queues      = 1;
                vulkan_hwdev_ctx->queue_family_tx_index   = queue.second.family;
                vulkan_hwdev_ctx->nb_tx_queues            = 1;
                vulkan_hwdev_ctx->queue_family_comp_index = queue.second.family;
                vulkan_hwdev_ctx->nb_comp_queues          = 1;
                break;
            default:
                break;
            }
        }

        // not using Vulkan Video for now
        vulkan_hwdev_ctx->nb_encode_queues          = 0;
        vulkan_hwdev_ctx->nb_decode_queues          = 0;
        vulkan_hwdev_ctx->queue_family_encode_index = -1;
        vulkan_hwdev_ctx->queue_family_decode_index = -1;

        vulkan_hwdev_ctx->alloc         = nullptr;
        vulkan_hwdev_ctx->get_proc_addr = vkGetInstanceProcAddr;

        vulkan_hwdev_ctx->enabled_inst_extensions    = dp->enabled_instance_extensions.data();
        vulkan_hwdev_ctx->nb_enabled_inst_extensions = dp->enabled_instance_extensions.size();
        vulkan_hwdev_ctx->enabled_dev_extensions     = dp->enabled_device_extensions.data();
        vulkan_hwdev_ctx->nb_enabled_dev_extensions  = dp->enabled_device_extensions.size();

        AV_ASSERT_SUCCESS(av_hwdevice_ctx_init(ref));
        log->info("FFmpeg Vulkan hwdevice context initialized");
    }

    std::vector<const char*> get_required_instance_extensions() override {
        return {};
    }

    std::vector<const char*> get_required_devices_extensions() override {
        std::vector<const char*> device_extensions;
        device_extensions.push_back(VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME);
        device_extensions.push_back(VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME);
        return device_extensions;
    }

protected:
    skip_option _p_should_skip() override {
        return threadloop::_p_should_skip();
    }

    void _p_thread_setup() override {
        AVFormatContext* pFormatCtx = nullptr;
    }

    void _p_one_iteration() override { }

private:
    std::array<std::vector<AVVkFrame>, 2> buffer_pool;
    std::shared_ptr<spdlog::logger>           log;
    std::shared_ptr<vulkan::display_provider> dp;

    void create_avvkframe(VkImageView image_view) {

    }
};

PLUGIN_MAIN(offload_rendering_client)