#include "illixr/vk_util/render_pass.hpp"
#include "illixr/plugin.hpp"

class mock_app_impl : public app {
public:
    explicit mock_app_impl(const phonebook* const pb) : sb{pb->lookup_impl<switchboard>()}{
    }

    void setup(VkRenderPass render_pass, uint32_t subpass) override {

    }

    void record_command_buffer(VkCommandBuffer commandBuffer, int eye) override {

    }

    void update_uniforms(const pose_type& render_pose) override {

    }

    void destroy() override {

    }

private:
    const std::shared_ptr<switchboard> sb;
};

class mock_app : public plugin {
public:
    mock_app(const std::string& name, phonebook* pb) : plugin{name, pb} {
        pb->register_impl<app>(std::static_pointer_cast<app>(std::make_shared<mock_app_impl>(pb)));
    }
};

PLUGIN_MAIN(mock_app)
