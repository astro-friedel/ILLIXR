#pragma once
#include "illixr/data_format.hpp"
#include "illixr/network/socket.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/threadloop.hpp"

#include "vio_output.pb.h"

namespace ILLIXR {
    class offload_reader : public threadloop {
    public:
        offload_reader(const std::string& name_, phonebook* pb_);

        skip_option _p_should_skip() override;
        void _p_one_iteration() override;

    private:
        void ReceiveVioOutput(const vio_output_proto::VIOOutput& vio_output, const std::string& str_data);

        const std::shared_ptr<switchboard>        sb;
        const std::shared_ptr<RelativeClock>      _m_clock;
        switchboard::writer<pose_type>            _m_pose;
        switchboard::writer<imu_integrator_input> _m_imu_integrator_input;

        TCPSocket   socket;
        bool        is_socket_connected;
        Address     server_addr;
        std::string buffer_str;
    };
}