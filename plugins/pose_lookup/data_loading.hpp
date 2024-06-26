#pragma once
#include "illixr/csv_iterator.hpp"
#include "illixr/data_format.hpp"
#include "illixr/error_util.hpp"

#include <eigen3/Eigen/Dense>
#include <fstream>
#include <iostream>
#include <map>
#include <spdlog/spdlog.h>
#include <string>

// timestamp
// p_RS_R_x [m], p_RS_R_y [m], p_RS_R_z [m]
// q_RS_w [], q_RS_x [], q_RS_y [], q_RS_z []
// v_RS_R_x [m s^-1], v_RS_R_y [m s^-1], v_RS_R_z [m s^-1]
// b_w_RS_S_x [rad s^-1], b_w_RS_S_y [rad s^-1], b_w_RS_S_z [rad s^-1]
// b_a_RS_S_x [m s^-2], b_a_RS_S_y [m s^-2], b_a_RS_S_z [m s^-2]

using namespace ILLIXR;

typedef pose_type sensor_types;

static std::map<ullong, sensor_types> load_data() {
    const char* illixr_data_c_str = std::getenv("ILLIXR_DATA");
    if (!illixr_data_c_str) {
        ILLIXR::abort("Please define ILLIXR_DATA");
    }
    std::string illixr_data = std::string{illixr_data_c_str};

    std::map<ullong, sensor_types> data;

    std::ifstream gt_file{illixr_data + "/state_groundtruth_estimate0/data.csv"};

    if (!gt_file.good()) {
        spdlog::get("illixr")->error("[poselookup] ${ILLIXR_DATA}/state_groundtruth_estimate0/data.csv "
                                     "({})/state_groundtruth_estimate0/data.csv) is not a good path",
                                     illixr_data);
        ILLIXR::abort();
    }

    for (CSVIterator row{gt_file, 1}; row != CSVIterator{}; ++row) {
        ullong             t = std::stoull(row[0]);
        Eigen::Vector3f    av{std::stof(row[1]), std::stof(row[2]), std::stof(row[3])};
        Eigen::Quaternionf la{std::stof(row[4]), std::stof(row[5]), std::stof(row[6]), std::stof(row[7])};
        data[t] = {{}, av, la};
    }

    return data;
}
