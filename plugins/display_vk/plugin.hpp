#pragma once
#include "illixr/vk_util/display_sink.hpp"
#include "illixr/phonebook.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/threadloop.hpp"

namespace ILLIXR {
    class display_vk : public display_sink {
    public:
        explicit display_vk(const phonebook* const pb);
        void setup();
        void recreate_swapchain() override;
        void poll_window_events() override;
    private:
        void setup_glfw();
        void setup_vk();

        const std::shared_ptr<switchboard> sb;
        vkb::Instance                      vkb_instance;
        vkb::PhysicalDevice                physical_device;
        vkb::Device                        vkb_device;
        vkb::Swapchain                     vkb_swapchain;

        std::atomic<bool> should_poll{true};

        friend class display_vk_plugin;
    };

    class display_vk_plugin : public plugin {
    public:
        display_vk_plugin(const std::string& name, phonebook* pb);
        void start();
        void stop() override;
    private:
        std::thread                 main_thread;
        std::atomic<bool>           ready{false};
        std::shared_ptr<display_vk> _dvk;
        std::atomic<bool>           running{true};
        phonebook*                  _pb;

        void main_loop();
    };

}
