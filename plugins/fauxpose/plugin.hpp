#pragma once
#include "illixr/pose_prediction.hpp"
#include "illixr/phonebook.hpp"
#include "illixr/plugin.hpp"

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>
#include <mutex>

/// Create a "pose_prediction" type service
namespace ILLIXR {
    class faux_pose_impl : public pose_prediction {
    public:
        explicit faux_pose_impl(const phonebook* const pb);
        ~faux_pose_impl() override;
        pose_type get_true_pose() const override;
        bool fast_pose_reliable() const override;
        bool true_pose_reliable() const override;
        pose_type correct_pose([[maybe_unused]] const pose_type& pose) const override;
        Eigen::Quaternionf get_offset() override;
        void set_offset(const Eigen::Quaternionf& raw_o_times_offset) override;
        fast_pose_type get_fast_pose() const override;
        fast_pose_type get_fast_pose(time_point time) const override;
    private:
        const std::shared_ptr<switchboard>                          sb;
        const std::shared_ptr<const RelativeClock>                  _m_clock;
        switchboard::reader<switchboard::event_wrapper<time_point>> _m_vsync_estimate;
        mutable Eigen::Quaternionf                                  offset{Eigen::Quaternionf::Identity()};
        mutable std::shared_mutex                                   offset_mutex;

        time_point sim_start_time{}; /* Store the initial time to calculate a known runtime */

        // Parameters
        double          period;          /* The period of the circular movment (in seconds) */
        double          amplitude;       /* The amplitude of the circular movment (in meters) */
        Eigen::Vector3f center_location; /* The location around which the tracking should orbit */
    };

    class faux_pose : public plugin {
    public:
        faux_pose(const std::string& name_, phonebook* pb_);
        ~faux_pose() override;
    };
}