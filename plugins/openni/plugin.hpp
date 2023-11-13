#pragma once
#include "illixr/opencv_data_types.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/threadloop.hpp"

#include <openni2/OpenNI.h>

namespace ILLIXR {
    class openni_plugin : public threadloop {
    public:
        openni_plugin(const std::string& name_, phonebook* pb_);
        ~openni_plugin() override;

    protected:
        skip_option _p_should_skip() override;
        void _p_one_iteration() override;
        bool camera_initialize();
    private:
        // ILLIXR
        const std::shared_ptr<switchboard>         sb;
        const std::shared_ptr<const RelativeClock> _m_clock;
        switchboard::writer<rgb_depth_type>        _m_rgb_depth;

        // OpenNI
        openni::Status        _device_status = openni::STATUS_OK;
        openni::Device        _device;
        openni::VideoStream   _depth, _color;
        openni::VideoFrameRef _depth_frame, _color_frame;

        // timestamp
        uint64_t   _cam_time;
        uint64_t   _last_ts = 0;
        uint64_t   _m_first_time;
        time_point _m_first_real_time;
        uint64_t   _time_sleep;

    };
}