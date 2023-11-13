#pragma once
#include <eigen3/Eigen/Core>
// clang-format off
#include <GL/glew.h>    // GLEW has to be loaded before other GL libraries
#include <GLFW/glfw3.h> // Also loading first, just to be safe
// clang-format on

#include "illixr/opencv_data_types.hpp"
#include "illixr/pose_prediction.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/threadloop.hpp"
#include "illixr/gl_util/obj.hpp"


constexpr size_t TEST_PATTERN_WIDTH  = 256;
constexpr size_t TEST_PATTERN_HEIGHT = 256;

namespace ILLIXR {
    namespace Debugview {
        Eigen::Matrix4f lookAt(const Eigen::Vector3f &eye, const Eigen::Vector3f &target, const Eigen::Vector3f &up);
        /**
         * @brief Callback function to handle glfw errors
         */
        static void glfw_error_callback(int error, const char* description) {
            spdlog::get("illixr")->error("|| glfw error_callback: {}\n|> {}", error, description);
            ILLIXR::abort();
        }
    }

    class debugview : public threadloop {
    public:
        // Public constructor, Spindle passes the phonebook to this
        // constructor. In turn, the constructor fills in the private
        // references to the switchboard plugs, so the plugin can read
        // the data whenever it needs to.

        debugview(const std::string& name_, phonebook* pb_);

        void draw_GUI();

        bool load_camera_images();

        bool load_rgb_depth();

        Eigen::Matrix4f generateHeadsetTransform(const Eigen::Vector3f& position, const Eigen::Quaternionf& rotation,
                                                 const Eigen::Vector3f& positionOffset);

        void _p_thread_setup() override;

        void _p_one_iteration() override;

        void start() override;

        ~debugview() override;

#ifdef BUILD_TEST
        friend class ILLIXRTest;
#endif
    private:
        // GLFWwindow * const glfw_context;
        const std::shared_ptr<switchboard>     sb;
        const std::shared_ptr<pose_prediction> pp;

        switchboard::reader<pose_type>         _m_slow_pose;
        switchboard::reader<imu_raw_type>      _m_fast_pose;
        switchboard::reader<rgb_depth_type>    _m_rgb_depth;
        switchboard::buffered_reader<cam_type> _m_cam;
        GLFWwindow*                            gui_window{};

        uint8_t test_pattern[TEST_PATTERN_WIDTH][TEST_PATTERN_HEIGHT]{};

        Eigen::Vector3d view_euler     = Eigen::Vector3d::Zero();
        Eigen::Vector2d last_mouse_pos = Eigen::Vector2d::Zero();
        Eigen::Vector2d mouse_velocity = Eigen::Vector2d::Zero();
        bool            beingDragged   = false;

        float view_dist = 2.0;

        bool follow_headset = true;

        Eigen::Vector3f tracking_position_offset = Eigen::Vector3f{0.0f, 0.0f, 0.0f};

        switchboard::ptr<const cam_type>       cam;
        switchboard::ptr<const rgb_depth_type> rgbd;
        bool                                   use_cam  = false;
        bool                                   use_rgbd = false;
        // std::vector<std::optional<cv::Mat>> camera_data = {std::nullopt, std::nullopt};
        GLuint          camera_textures[2];
        Eigen::Vector2i camera_texture_sizes[2] = {Eigen::Vector2i::Zero(), Eigen::Vector2i::Zero()};
        GLuint          rgbd_textures[2];
        Eigen::Vector2i rgbd_texture_sizes[2] = {Eigen::Vector2i::Zero(), Eigen::Vector2i::Zero()};

        GLuint demo_vao;
        GLuint demoShaderProgram;

        GLuint vertexPosAttr;
        GLuint vertexNormalAttr;
        GLuint modelViewAttr;
        GLuint projectionAttr;

        GLuint colorUniform;

        ObjScene demoscene;
        ObjScene headset;

        Eigen::Matrix4f basicProjection;

    };
}