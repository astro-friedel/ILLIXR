#pragma once
#include "data_loading.hpp"
#include "illixr/data_format.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/threadloop.hpp"

namespace ILLIXR {
    class offline_imu : public threadloop {
    public:
        offline_imu(const std::string& name_, phonebook* pb_);

    protected:
        skip_option _p_should_skip() override;
        void _p_one_iteration() override;
    private:
        const std::map<ullong, sensor_types>           _m_sensor_data;
        std::map<ullong, sensor_types>::const_iterator _m_sensor_data_it;
        const std::shared_ptr<switchboard>             _m_sb;
        switchboard::writer<imu_type>                  _m_imu;

        // Timestamp of the first IMU value from the dataset
        ullong dataset_first_time;
        // Current IMU timestamp
        ullong dataset_now;

        record_coalescer imu_cam_log;

        std::shared_ptr<RelativeClock> _m_rtc;

    };
}