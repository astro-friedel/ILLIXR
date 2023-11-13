#pragma once
#include "illixr/data_format.hpp"
#include "illixr/plugin.hpp"
#include "illixr/switchboard.hpp"

namespace ILLIXR {
    class offload_data : public plugin {
    public:
        offload_data(const std::string& name_, phonebook* pb_);
        ~offload_data() override;
        void callback(const switchboard::ptr<const texture_pose>& datum);
    private:
        void writeMetadata();
        void writeDataToDisk();
        const std::shared_ptr<switchboard>                sb;
        std::vector<long>                                 _time_seq;
        std::vector<switchboard::ptr<const texture_pose>> _offload_data_container;

        int         percent;
        int         img_idx;
        bool        enable_offload;
        bool        is_success;
        std::string obj_dir;

    };
}