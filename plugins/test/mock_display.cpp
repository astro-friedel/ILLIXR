#include <gmock/gmock.h>
#include "illixr/vk_util/display_sink.hpp"
#include "illixr/plugin.hpp"
using namespace ILLIXR;

class mock_display_impl : public display_sink {
public:
    MOCK_METHOD(void, poll_window_events, (), (override));

    MOCK_METHOD(void, recreate_swapchain, (), (override));
};

class mock_display : public plugin {
public:
    mock_display(const std::string& name, phonebook* pb) : plugin{name, pb} {
        pb->register_impl<display_sink>(std::static_pointer_cast<display_sink>(std::make_shared<mock_display_impl>()));
    }
};

PLUGIN_MAIN(mock_display)
