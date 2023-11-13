#pragma once
#include "illixr/data_format.hpp"
#include "illixr/opencv_data_types.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/threadloop.hpp"

// ZED includes
#include "zed_opencv.hpp"

namespace ILLIXR {
    static constexpr unsigned EXPOSURE_TIME_PERCENT = 8;

    const record_header __imu_cam_record{"imu_cam",
                                         {
                                                 {"iteration_no", typeid(std::size_t)},
                                                 {"has_camera", typeid(bool)},
                                         }};

    struct cam_type_zed : public switchboard::event {
        cam_type_zed(cv::Mat _img0, cv::Mat _img1, cv::Mat _rgb, cv::Mat _depth, std::size_t _serial_no)
                : img0{std::move(_img0)}
                , img1{std::move(_img1)}
                , rgb{std::move(_rgb)}
                , depth{std::move(_depth)}
                , serial_no{_serial_no} { }

        cv::Mat     img0;
        cv::Mat     img1;
        cv::Mat     rgb;
        cv::Mat     depth;
        std::size_t serial_no;
    };
    std::shared_ptr<Camera> start_camera();

    class zed_camera_thread : public threadloop {
    public:
        zed_camera_thread(const std::string& name_, phonebook* pb_, std::shared_ptr<Camera> zedm_);

    protected:
        skip_option _p_should_skip() override;
        void _p_one_iteration() override;

    private:
        const std::shared_ptr<switchboard>         sb;
        const std::shared_ptr<const RelativeClock> _m_clock;
        switchboard::writer<cam_type_zed>          _m_cam;
        std::shared_ptr<Camera>                    zedm;
        Resolution                                 image_size;
        RuntimeParameters                          runtime_parameters;
        std::size_t                                serial_no{0};

        Mat imageL_zed;
        Mat imageR_zed;
        Mat depth_zed;
        Mat rgb_zed;

        cv::Mat imageL_ocv;
        cv::Mat imageR_ocv;
        cv::Mat depth_ocv;
        cv::Mat rgb_ocv;

        std::optional<ullong> _m_first_imu_time;

    };

    class zed_imu_thread : public threadloop {
    public:
        zed_imu_thread(const std::string& name_, phonebook* pb_);
        ~zed_imu_thread() override;

        void stop() override;

    protected:
        skip_option _p_should_skip() override;
        void _p_one_iteration() override;

    private:
        std::shared_ptr<Camera> zedm;
        zed_camera_thread       camera_thread_;

        const std::shared_ptr<switchboard>         sb;
        const std::shared_ptr<const RelativeClock> _m_clock;
        switchboard::writer<imu_type>              _m_imu;
        switchboard::reader<cam_type_zed>          _m_cam_reader;
        switchboard::writer<cam_type>              _m_cam_publisher;
        switchboard::writer<rgb_depth_type>        _m_rgb_depth;

        // IMU
        SensorsData sensors_data;
        Timestamp   last_imu_ts    = 0;
        std::size_t last_serial_no = 0;

        // Logger
        record_coalescer it_log;

        std::optional<ullong>     _m_first_imu_time;
        std::optional<time_point> _m_first_real_time;
    };
}