#pragma once
#include "illixr/data_format.hpp"
#include "illixr/opencv_data_types.hpp"
#include "illixr/plugin.hpp"
#include "illixr/switchboard.hpp"

#include <boost/filesystem.hpp>


namespace ILLIXR {
    class record_imu_cam : public plugin {
    public:
        record_imu_cam(const std::string& name_, phonebook* pb_);
        ~record_imu_cam() override;

        void dump_data(const switchboard::ptr<const imu_type>& datum);
    private:
        static boost::filesystem::path get_record_data_path();
        std::ofstream                      imu_wt_file;
        std::ofstream                      cam0_wt_file;
        std::ofstream                      cam1_wt_file;
        const std::shared_ptr<switchboard> sb;

        switchboard::buffered_reader<cam_type> _m_cam;

        const boost::filesystem::path record_data;
        const boost::filesystem::path cam0_data_dir;
        const boost::filesystem::path cam1_data_dir;

    };
}