#pragma once
#include "data_loading.hpp"
#include "illixr/opencv_data_types.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/threadloop.hpp"

namespace ILLIXR {
    class offline_cam : public threadloop {
    public:
        offline_cam(const std::string& name_, phonebook* pb_);

        skip_option _p_should_skip() override;
        void _p_one_iteration() override;
    private:
        const std::shared_ptr<switchboard>             sb;
        switchboard::writer<cam_type>                  _m_cam_publisher;
        const std::map<ullong, sensor_types>           _m_sensor_data;
        ullong                                         dataset_first_time;
        ullong                                         last_ts;
        std::shared_ptr<RelativeClock>                 _m_rtc;
        std::map<ullong, sensor_types>::const_iterator next_row;

    };
}