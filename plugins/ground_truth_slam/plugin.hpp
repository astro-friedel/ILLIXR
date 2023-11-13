#pragma once

#include "data_loading.hpp"
#include "illixr/plugin.hpp"
#include "illixr/phonebook.hpp"
#include "illixr/switchboard.hpp"

// These are the first IMU timestamp of the datasets. See line#31 for more info.
#define ViconRoom1Easy      1403715273262142976
#define ViconRoom1Medium    1403715523912143104
#define ViconRoom1Difficult 1403715886544058112
#define ViconRoom2Easy      1413393212225760512
#define ViconRoom2Medium    1413393885975760384
#define ViconRoom2Hard      1413394881555760384

namespace ILLIXR {
    class ground_truth_slam : public plugin {
    public:
        ground_truth_slam(const std::string & name_, phonebook* pb_);
        void start() override;
        void feed_ground_truth(const switchboard::ptr<const imu_type>& datum);
    private:
        const std::shared_ptr<switchboard> sb;
        switchboard::writer<pose_type>     _m_true_pose;

        switchboard::writer<switchboard::event_wrapper<Eigen::Vector3f>> _m_ground_truth_offset;
        const std::map<ullong, sensor_types>                             _m_sensor_data;
        ullong                                                           _m_dataset_first_time;
        bool                                                             _m_first_time;

    };
}