#include "illixr/vk_util/display_sink.hpp"
#include "illixr/plugin.hpp"
using namespace ILLIXR;

class mock_display_impl : public display_sink {
public:
    explicit mock_display_impl(const phonebook* const pb) : sb{pb->lookup_impl<switchboard>()} {

    }
    void poll_window_events() override {

    }

    void recreate_swapchain() override {

    }
    ~mock_display_impl() override = default;
private:
    const std::shared_ptr<switchboard> sb;
};

class mock_display : public plugin {
public:
    mock_display(const std::string& name, phonebook* pb) : plugin{name, pb} {
        pb->register_impl<display_sink>(std::static_pointer_cast<display_sink>(std::make_shared<mock_display_impl>(pb)));
    }
};

PLUGIN_MAIN(mock_display)
