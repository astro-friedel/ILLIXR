#pragma once

#include <chrono>
#include <mutex>
#include <string>

#include "illixr/data_format.hpp"
#include "illixr/opencv_data_types.hpp"
#include "illixr/phonebook.hpp"
#include "illixr/plugin.hpp"
#include "illixr/switchboard.hpp"

// Includes common necessary includes for development using depthai library
#include <depthai/depthai.hpp>

namespace ILLIXR {
    class depthai : public plugin {
    public:
        depthai(std::string name_, phonebook* pb_);

        void callback();

        ~depthai() override;

        friend class ILLIXRTest;
    private:
        const std::shared_ptr<switchboard>         sb;
        const std::shared_ptr<const RelativeClock> _m_clock;
        switchboard::writer<imu_type>              _m_imu;
        switchboard::writer<cam_type>              _m_cam;
        switchboard::writer<rgb_depth_type>        _m_rgb_depth;
        std::mutex                                 mutex;

#ifndef NDEBUG
        int imu_packet{0};
        int imu_pub{0};
        int rgbd_pub{0};
        int rgb_count{0};
        int left_count{0};
        int right_count{0};
        int depth_count{0};
        int all_count{0};
#endif
        std::chrono::time_point<std::chrono::steady_clock>                                      first_packet_time;
        std::chrono::time_point<std::chrono::steady_clock, std::chrono::steady_clock::duration> test_time_point;
        bool                                                                                    useRaw = false;
        dai::Device                                                                             device;

        std::shared_ptr<dai::DataOutputQueue> colorQueue;
        std::shared_ptr<dai::DataOutputQueue> depthQueue;
        std::shared_ptr<dai::DataOutputQueue> rectifLeftQueue;
        std::shared_ptr<dai::DataOutputQueue> rectifRightQueue;
        std::shared_ptr<dai::DataOutputQueue> imuQueue;

        std::optional<ullong>     _m_first_imu_time;
        std::optional<time_point> _m_first_real_time;

        std::optional<ullong>     _m_first_cam_time;
        std::optional<time_point> _m_first_real_time_cam;

        dai::Pipeline createCameraPipeline() const;
    };
}