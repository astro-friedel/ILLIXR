#include "plugin.hpp"

using namespace ILLIXR;

offline_imu::offline_imu(const std::string& name_, phonebook* pb_)
        : threadloop{name_, pb_}
        , _m_sensor_data{load_data()}
        , _m_sensor_data_it{_m_sensor_data.cbegin()}
        , _m_sb{pb->lookup_impl<switchboard>()}
        , _m_imu{_m_sb->get_writer<imu_type>("imu")}
        , dataset_first_time{_m_sensor_data_it->first}
        , dataset_now{0}
        , imu_cam_log{record_logger_}
        , _m_rtc{pb->lookup_impl<RelativeClock>()} { }

threadloop::skip_option offline_imu::_p_should_skip() {
    if (_m_sensor_data_it != _m_sensor_data.end()) {
        assert(dataset_now < _m_sensor_data_it->first);
        dataset_now = _m_sensor_data_it->first;
        // Sleep for the difference between the current IMU vs 1st IMU and current UNIX time vs UNIX time the component was
        // init
        std::this_thread::sleep_for(std::chrono::nanoseconds{dataset_now - dataset_first_time} -
                                    _m_rtc->now().time_since_epoch());

        return skip_option::run;

    } else {
        return skip_option::stop;
    }
}

void offline_imu::_p_one_iteration() {
    assert(_m_sensor_data_it != _m_sensor_data.end());
    time_point          real_now(std::chrono::duration<long, std::nano>{dataset_now - dataset_first_time});
    const sensor_types& sensor_datum = _m_sensor_data_it->second;

    _m_imu.put(_m_imu.allocate<imu_type>(imu_type{real_now, (sensor_datum.imu0.angular_v), (sensor_datum.imu0.linear_a)}));
    ++_m_sensor_data_it;
}

PLUGIN_MAIN(offline_imu)
