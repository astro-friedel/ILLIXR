#pragma once
#include "illixr/data_format.hpp"
#include "illixr/network/socket.hpp"
#include "illixr/opencv_data_types.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/threadloop.hpp"

#include "video_encoder.h"
#include "vio_input.pb.h"

#include <boost/lockfree/spsc_queue.hpp>

namespace ILLIXR {
    class offload_writer : public threadloop {
    public:
        offload_writer(const std::string& name_, phonebook* pb_);

        void start() override;
        void send_imu_cam_data(std::optional<time_point>& cam_time);
        void prepare_imu_cam_data(const switchboard::ptr<const imu_type>& datum);
    protected:
        void _p_thread_setup() override {}
        void _p_one_iteration() override;
    private:
        boost::lockfree::spsc_queue<uint64_t> queue{1000};
        std::vector<int32_t>                  sizes;
        std::mutex                            mutex;
        std::condition_variable               cv;
        GstMapInfo                            img0;
        GstMapInfo                            img1;
        bool                                  img_ready = false;
        std::unique_ptr<video_encoder>         encoder = nullptr;
        std::optional<time_point>              latest_imu_time;
        std::optional<time_point>              latest_cam_time;
        int                                    frame_id    = 0;
        vio_input_proto::IMUCamVec*            data_buffer = new vio_input_proto::IMUCamVec();
        const std::shared_ptr<switchboard>     sb;
        const std::shared_ptr<RelativeClock>   _m_clock;
        const std::shared_ptr<Stoplight>       _m_stoplight;
        switchboard::buffered_reader<cam_type> _m_cam;

        TCPSocket socket;
        Address   server_addr;
    };
}