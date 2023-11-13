#pragma once
#include "illixr/data_format.hpp"
#include "illixr/plugin.hpp"
#include "illixr/switchboard.hpp"

namespace ILLIXR {
    class passthrough_integrator : public plugin {
    public:
        passthrough_integrator(const std::string& name_, phonebook* pb_);
        void callback(const switchboard::ptr<const imu_type>& datum);
    private:
        const std::shared_ptr<switchboard> sb;

        // IMU Data, Sequence Flag, and State Vars Needed
        switchboard::reader<imu_integrator_input> _m_imu_integrator_input;

        // IMU state
        switchboard::writer<imu_raw_type> _m_imu_raw;

    };
}