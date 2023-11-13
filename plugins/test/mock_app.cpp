#include <gmock/gmock.h>
#include "illixr/vk_util/render_pass.hpp"
#include "illixr/plugin.hpp"

class mock_app_impl : public app {
public:

    MOCK_METHOD(void, setup, (VkRenderPass render_pass, uint32_t subpass), (override));

    MOCK_METHOD(void, record_command_buffer, (VkCommandBuffer commandBuffer, int eye), (override));

    MOCK_METHOD(void, update_uniforms, (const pose_type& render_pose), (override));

    MOCK_METHOD(void, destroy, (), (override));

};

class mock_app : public plugin {
public:
    mock_app(const std::string& name, phonebook* pb) : plugin{name, pb} {
        pb->register_impl<app>(std::static_pointer_cast<app>(std::make_shared<mock_app_impl>()));
    }
};

PLUGIN_MAIN(mock_app)
