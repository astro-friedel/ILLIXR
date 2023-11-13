#pragma once
#include "data_loading.hpp"
#include "illixr/plugin.hpp"
#include "illixr/pose_prediction.hpp"

namespace ILLIXR {
    class pose_lookup_impl : public pose_prediction {
    public:
        explicit pose_lookup_impl(const phonebook* const pb);
        fast_pose_type get_fast_pose() const override;
        pose_type get_true_pose() const override;
        bool fast_pose_reliable() const override;
        bool true_pose_reliable() const override;
        Eigen::Quaternionf get_offset() override;
        pose_type correct_pose(const pose_type& pose) const override;
        void set_offset(const Eigen::Quaternionf& raw_o_times_offset) override;
        Eigen::Quaternionf apply_offset(const Eigen::Quaternionf& orientation) const;
        fast_pose_type get_fast_pose(time_point time) const override;
    private:
        const std::shared_ptr<switchboard>         sb;
        const std::shared_ptr<const RelativeClock> _m_clock;
        mutable Eigen::Quaternionf                 offset{Eigen::Quaternionf::Identity()};
        mutable std::shared_mutex                  offset_mutex;

        const std::map<ullong, sensor_types>                        _m_sensor_data;
        std::map<ullong, sensor_types>::const_iterator              _m_sensor_data_it;
        ullong                                                      dataset_first_time;
        switchboard::reader<switchboard::event_wrapper<time_point>> _m_vsync_estimate;

        bool            enable_alignment;
        Eigen::Vector3f init_pos_offset;
        Eigen::Matrix3f align_rot;
        Eigen::Vector3f align_trans;
        Eigen::Vector4f align_quat;
        double          align_scale;

    };

    class pose_lookup_plugin : public plugin {
    public:
        pose_lookup_plugin(const std::string& name_, phonebook* pb_);
    };
}