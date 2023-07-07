#include "common/csv_iterator.hpp"
#include "spdlog/spdlog.h"

#include <eigen3/Eigen/Dense>
#include <fstream>
#include <map>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <optional>
#include <string>

typedef unsigned long long ullong;

/*
 * Uncommenting this preprocessor macro makes the offline_cam load each data from the disk as it is needed.
 * Otherwise, we load all of them at the beginning, hold them in memory, and drop them in the queue as needed.
 * Lazy loading has an artificial negative impact on performance which is absent from an online-sensor system.
 * Eager loading deteriorates the startup time and uses more memory.
 */
//#define LAZY

class lazy_load_image {
public:
    lazy_load_image() { }

    lazy_load_image(const std::string& path)
        : _m_path(path) {
#ifndef LAZY
        _m_mat = cv::imread(_m_path, cv::IMREAD_GRAYSCALE);
#endif
    }

    cv::Mat load() const {
#ifdef LAZY
        cv::Mat _m_mat = cv::imread(_m_path, cv::IMREAD_GRAYSCALE);
    #error "Linux scheduler cannot interrupt IO work, so lazy-loading is unadvisable."
#endif
        assert(!_m_mat.empty());
        return _m_mat;
    }

private:
    std::string _m_path;
    cv::Mat     _m_mat;
};

typedef struct {
    lazy_load_image cam0;
    lazy_load_image cam1;
} sensor_types;

static std::map<ullong, sensor_types> load_data() {
    const char* illixr_data_c_str = std::getenv("ILLIXR_DATA");
    if (!illixr_data_c_str) {
	spdlog::get("illixr_file_log")->error("[OFFLINECAM] Please define ILLIXR_DATA");
        ILLIXR::abort();
    }
    std::string illixr_data = std::string{illixr_data_c_str};

    std::map<ullong, sensor_types> data;

    const std::string cam0_subpath = "/cam0/data.csv";
    std::ifstream     cam0_file{illixr_data + cam0_subpath};
    if (!cam0_file.good()) {
	spdlog::get("illixr_file_log")->error("[OFFLINECAM] ${ILLIXR_DATA} {0} ({1}{0}) is not a good path", cam0_subpath, illixr_data);
        ILLIXR::abort();
    }
    for (CSVIterator row{cam0_file, 1}; row != CSVIterator{}; ++row) {
        ullong t     = std::stoull(row[0]);
        data[t].cam0 = {illixr_data + "/cam0/data/" + row[1]};
    }

    const std::string cam1_subpath = "/cam1/data.csv";
    std::ifstream     cam1_file{illixr_data + cam1_subpath};
    if (!cam1_file.good()) {
	spdlog::get("illixr_file_log")->error("[OFFLINECAM] ${ILLIXR_DATA} {0} ({1}{0}) is not a good path", cam1_subpath, illixr_data);
        ILLIXR::abort();
    }
    for (CSVIterator row{cam1_file, 1}; row != CSVIterator{}; ++row) {
        ullong      t     = std::stoull(row[0]);
        std::string fname = row[1];
        data[t].cam1      = {illixr_data + "/cam1/data/" + row[1]};
    }

    return data;
}
