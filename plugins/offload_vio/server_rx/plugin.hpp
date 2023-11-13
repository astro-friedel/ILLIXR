#pragma once
#include "illixr/data_format.hpp"
#include "illixr/network/socket.hpp"
#include "illixr/opencv_data_types.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/threadloop.hpp"

#include "vio_input.pb.h"

#include "video_decoder.h"

#include <boost/lockfree/spsc_queue.hpp>

namespace ILLIXR {
    class server_reader : public threadloop {
    public:
        server_reader(const std::string& name_, phonebook* pb_);

        skip_option _p_should_skip() override;
        void _p_one_iteration() override;
        ~server_reader() override;
        void start() override;
    private:
        void ReceiveVioInput(const vio_input_proto::IMUCamVec& vio_input);

        std::unique_ptr<video_decoder> decoder;

        boost::lockfree::spsc_queue<uint64_t> queue{1000};
        std::mutex                            mutex;
        std::condition_variable               cv;
        cv::Mat                               img0_dst;
        cv::Mat                               img1_dst;
        bool                                  img_ready = false;
        const std::shared_ptr<switchboard>     sb;
        switchboard::writer<imu_type>          _m_imu;
        switchboard::writer<cam_type>          _m_cam;
        switchboard::writer<connection_signal> _conn_signal;

        TCPSocket   socket;
        TCPSocket*  read_socket = NULL;
        Address     server_addr;
        std::string buffer_str;

    };
}