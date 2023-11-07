#pragma once

#include "switchboard.hpp"
#include "data_format.hpp"

#include <opencv2/core/mat.hpp>

namespace ILLIXR {
using ullong = unsigned long long;
struct cam_type : switchboard::event {
    time_point time;
    cv::Mat    img0;
    cv::Mat    img1;

    cam_type(time_point _time, cv::Mat _img0, cv::Mat _img1)
        : time{_time}
        , img0{std::move(_img0)}
        , img1{std::move(_img1)} { }
};

struct rgb_depth_type : public switchboard::event {
    [[maybe_unused]] time_point time;
    cv::Mat                     rgb;
    cv::Mat                     depth;

    rgb_depth_type(time_point _time, cv::Mat _rgb, cv::Mat _depth)
        : time{_time}
        , rgb{std::move(_rgb)}
        , depth{std::move(_depth)} { }
};

//pyh temp format for TUM with groundtruth
struct rgb_depth_pose_type : public switchboard::event {
    std::optional<cv::Mat> rgb;
    std::optional<cv::Mat> depth;
    //storing groundtruth info
    std::optional<Eigen::Vector3f> position;
    std::optional<Eigen::Quaternionf> orientation;
    //std::optional<Eigen::Matrix4f> transformation;
    [[maybe_unused]] ullong timestamp;
    rgb_depth_pose_type(
        std::optional<cv::Mat> _rgb,
        std::optional<cv::Mat> _depth,
        std::optional<Eigen::Vector3f> _pos,
        std::optional<Eigen::Quaternionf> _ori,
        //std::optional<Eigen::Matrix4f> _trans,
        ullong _timestamp
        )
        : rgb{_rgb}
        , depth{_depth}
        , position{_pos}
        , orientation{_ori}
        //, transformation{_trans}
        , timestamp{_timestamp}
        { }
    };

// for offline_scannet
struct scene_recon_type : public switchboard::event {
    [[maybe_unused]] time_point time;
    pose_type pose;
    cv::Mat depth;
    cv::Mat rgb; //rgb is only if you need colored mesh
    bool last_frame;
    scene_recon_type(time_point camera_time, pose_type pose_, cv::Mat depth_, cv::Mat rgb_, bool is_last_frame)
        : time{camera_time}
        , pose{pose_}
        , depth{depth_}
        , rgb{rgb_}
        , last_frame{is_last_frame}
        {}
};

} // namespace ILLIXR
