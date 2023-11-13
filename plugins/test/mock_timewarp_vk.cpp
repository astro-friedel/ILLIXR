#include <gmock/gmock.h>
#include "illixr/vk_util/render_pass.hpp"
#include "illixr/threadloop.hpp"

using namespace ILLIXR;

class mock_timewarp_vk_impl : public timewarp {
public:

    MOCK_METHOD(void, update_uniforms, (const pose_type& render_pose), (override));
    using arr_vec = std::array<std::vector<VkImageView>, 2>;
    MOCK_METHOD(void, setup, (VkRenderPass render_pass, uint32_t subpass, arr_vec buffer_pool,
            bool input_texture_vulkan_coordinates), (override));

    MOCK_METHOD(void, record_command_buffer, (VkCommandBuffer commandBuffer, int buffer_ind, bool left),
                (override));

    MOCK_METHOD(void, destroy, (), (override));
};

class mock_timewarp_vk : public threadloop {
public:
    mock_timewarp_vk(const std::string& name, phonebook* pb) : threadloop{name, pb} {
        pb->register_impl<timewarp>(std::static_pointer_cast<timewarp>(std::make_shared<mock_timewarp_vk_impl>()));
    }
    MOCK_METHOD(void, _p_one_iteration, (), (override));
    MOCK_METHOD(skip_option, _p_should_skip, (), (override));
};

PLUGIN_MAIN(mock_timewarp_vk)
