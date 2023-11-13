#pragma once

#include "illixr/data_format.hpp"
#include "illixr/network/socket.hpp"
#include "illixr/plugin.hpp"
#include "illixr/switchboard.hpp"

namespace ILLIXR {
    class server_writer : public plugin {
    public:
        server_writer(const std::string& name_, phonebook* pb_);
        void start() override;
        void start_accepting_connection(switchboard::ptr<const connection_signal>& datum);
        void send_vio_output(const switchboard::ptr<const pose_type>& datum);

    private:
        const std::shared_ptr<switchboard>        sb;
        switchboard::reader<imu_integrator_input> _m_imu_int_input;

        TCPSocket  socket;
        TCPSocket* write_socket = NULL;
        Address    client_addr;
        bool       is_client_connected;

    };
}