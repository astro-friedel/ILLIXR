#pragma once
#include "illixr/opencv_data_types.hpp"
#include "illixr/plugin.hpp"
#include "illixr/switchboard.hpp"

#include <boost/filesystem.hpp>

namespace ILLIXR {
    class record_rgb_depth : public plugin {
    public:
        record_rgb_depth(const std::string& name_, phonebook* pb_);
        ~record_rgb_depth() override;

        void dump_data(switchboard::ptr<const rgb_depth_type>& datum);

    private:
        boost::filesystem::path get_record_data_path();
        std::ofstream                      rgb_wt_file;
        std::ofstream                      depth_wt_file;
        const std::shared_ptr<switchboard> sb;

        const boost::filesystem::path record_data;
        const boost::filesystem::path rgb_data_dir;
        const boost::filesystem::path depth_data_dir;

    };
}