#include "illixr/vk_util/render_pass.hpp"
#include "illixr/threadloop.hpp"

using namespace ILLIXR;

class mock_timewarp_vk_impl : public timewarp {
public:
    explicit mock_timewarp_vk_impl(const phonebook* const pb) : sb{pb->lookup_impl<switchboard>()} {

    }

    void update_uniforms(const pose_type& render_pose) override {

    }

    void setup(VkRenderPass render_pass, uint32_t subpass, std::array<std::vector<VkImageView>, 2> buffer_pool,
               bool input_texture_vulkan_coordinates) override {

    }

    void record_command_buffer(VkCommandBuffer commandBuffer, int buffer_ind, bool left) override {

    }

    void destroy() override {

    }

    ~mock_timewarp_vk_impl() override = default;
private:
    const std::shared_ptr<switchboard> sb;
};

class mock_timewarp_vk : public threadloop {
public:
    mock_timewarp_vk(const std::string& name, phonebook* pb) : threadloop{name, pb} {
        pb->register_impl<timewarp>(std::static_pointer_cast<timewarp>(std::make_shared<mock_timewarp_vk_impl>(pb)));
    }

    void _p_one_iteration() override {

    }

    skip_option _p_should_skip() override {

    }
};

PLUGIN_MAIN(mock_timewarp_vk)
