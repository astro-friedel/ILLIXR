#pragma once

#include "illixr/data_format.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/plugin.hpp"
#include "illixr/phonebook.hpp"

#include "third_party/filter.h"
#include <gtsam/navigation/ImuFactor.h>


using ImuBias = gtsam::imuBias::ConstantBias;
using imu_int_t = ILLIXR::imu_integrator_input;
using bias_t    = ImuBias;
using nav_t     = gtsam::NavState;
using pim_ptr_t = gtsam::PreintegrationType*;
using imu_t     = ILLIXR::imu_type;

// IMU sample time to live in seconds

namespace ILLIXR {
    constexpr duration IMU_TTL{std::chrono::seconds{5}};

    class gtsam_integrator : public plugin {
    public:
        gtsam_integrator(const std::string& name_, phonebook* pb_);
        void callback(const switchboard::ptr<const imu_type>& datum);
    private:
        std::vector<one_euro_filter<Eigen::Array<double, 3, 1>, double>> filters;
        bool                                                             has_prev = false;
        Eigen::Matrix<double, 3, 1>                                      prev_euler_angles;

        const std::shared_ptr<switchboard>   sb;
        const std::shared_ptr<RelativeClock> _m_clock;

        // IMU Data, Sequence Flag, and State Vars Needed
        switchboard::reader<imu_integrator_input> _m_imu_integrator_input;

        // Write IMU Biases for PP
        switchboard::writer<imu_raw_type> _m_imu_raw;

        std::vector<imu_type> _imu_vec;

        // std::vector<pose_type> filtered_poses;

        [[maybe_unused]] time_point last_cam_time{};
        duration                    last_imu_offset{};

        /**
         * @brief Wrapper object protecting the lifetime of IMU integration inputs and biases
         */
        class PimObject {
        public:

            explicit PimObject(const imu_int_t& imu_int_input);
            ~PimObject();
            void resetIntegrationAndSetBias(const imu_int_t& imu_int_input) noexcept;
            void integrateMeasurement(const imu_t& imu_input, const imu_t& imu_input_next) noexcept;
            [[nodiscard]] bias_t biasHat() const noexcept;
            [[nodiscard]] nav_t predict() const noexcept;
        private:
            bias_t    _imu_bias;
            nav_t     _navstate_lkf;
            pim_ptr_t _pim;

        };

        std::unique_ptr<PimObject> _pim_obj;
        void clean_imu_vec(time_point timestamp);
        void propagate_imu_values(time_point real_time);
        static std::vector<imu_type> select_imu_readings(const std::vector<imu_type>& imu_data,
                                                         const time_point& time_begin,
                                                         const time_point& time_end);
        static imu_type interpolate_imu(const imu_type& imu_1, const imu_type& imu_2, time_point timestamp);
    };
}