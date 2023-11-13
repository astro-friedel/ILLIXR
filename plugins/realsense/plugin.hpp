#pragma once
#include "illixr/data_format.hpp"
#include "illixr/opencv_data_types.hpp"
#include "illixr/plugin.hpp"
#include "illixr/switchboard.hpp"

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API

namespace ILLIXR {
    class realsense : public plugin {
    public:
        realsense(const std::string& name_, phonebook* pb_);
        void callback(const rs2::frame& frame);
        ~realsense() override;
    private:
        void find_supported_devices(const rs2::device_list& devices);
        void configure_camera();
        typedef enum { UNSUPPORTED, D4XXI, T26X } cam_enum;

        typedef struct {
            rs2_vector data;
            int        iteration;
        } accel_type;

        const std::shared_ptr<switchboard>         sb;
        const std::shared_ptr<const RelativeClock> _m_clock;
        switchboard::writer<imu_type>              _m_imu;
        switchboard::writer<cam_type>              _m_cam;
        switchboard::writer<rgb_depth_type>        _m_rgb_depth;
        std::mutex                                 mutex;
        rs2::pipeline_profile                      profiles;
        rs2::pipeline                              pipe;
        rs2::config                                cfg;

        cam_enum cam_select{UNSUPPORTED};
        bool     D4XXI_found{false};
        bool     T26X_found{false};

        accel_type  accel_data;
        int         iteration_accel = 0;
        int         last_iteration_accel;
        std::string realsense_cam;

        std::optional<ullong>     _m_first_imu_time;
        std::optional<time_point> _m_first_real_time_imu;

        std::optional<ullong>     _m_first_cam_time;
        std::optional<time_point> _m_first_real_time_cam;

    };
}