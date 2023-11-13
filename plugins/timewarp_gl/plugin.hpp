#pragma once

#include <GL/glew.h> // GLEW has to be loaded before other GL libraries
#include <GL/glx.h>

#include "illixr/pose_prediction.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/threadloop.hpp"

#include "utils/hmd.hpp"


namespace ILLIXR {

    typedef void (*glXSwapIntervalEXTProc)(Display *dpy, GLXDrawable drawable, int interval);

    const record_header timewarp_gpu_record{"timewarp_gpu",
                                            {
                                                    {"iteration_no", typeid(std::size_t)},
                                                    {"wall_time_start", typeid(time_point)},
                                                    {"wall_time_stop", typeid(time_point)},
                                                    {"gpu_time_duration", typeid(std::chrono::nanoseconds)},
                                            }};

    const record_header mtp_record{"mtp_record",
                                   {
                                           {"iteration_no", typeid(std::size_t)},
                                           {"vsync", typeid(time_point)},
                                           {"imu_to_display", typeid(std::chrono::nanoseconds)},
                                           {"predict_to_display", typeid(std::chrono::nanoseconds)},
                                           {"render_to_display", typeid(std::chrono::nanoseconds)},
                                   }};

#ifdef ILLIXR_MONADO
    typedef plugin timewarp_type;
#else
    typedef threadloop timewarp_type;
#endif

    class timewarp_gl : public timewarp_type {
    public:
        timewarp_gl(const std::string& name_, phonebook* pb_);

        void _setup();
        void _prepare_rendering();
        void warp(switchboard::ptr<const rendered_frame>& most_recent_frame);
        skip_option _p_should_skip() override;
        void _p_thread_setup() override;
        void _p_one_iteration() override;

#ifndef NDEBUG
        size_t log_count  = 0;
        size_t LOG_PERIOD = 20;
#endif

    private:
        GLubyte* readTextureImage();
        static GLuint ConvertVkFormatToGL(int64_t vk_format);
        void ImportVulkanImage(const vk_image_handle& vk_handle, swapchain_usage usage);
        void BuildTimewarp(HMD::hmd_info_t& hmdInfo);
        static void CalculateTimeWarpTransform(Eigen::Matrix4f& transform,
                                               const Eigen::Matrix4f& renderProjectionMatrix,
                                               const Eigen::Matrix4f& renderViewMatrix,
                                               const Eigen::Matrix4f& newViewMatrix);

#ifndef ILLIXR_MONADO
        time_point GetNextSwapTimeEstimate();
        duration EstimateTimeToSleep(double framePercentage);
#endif

        const std::shared_ptr<switchboard>         sb;
        const std::shared_ptr<pose_prediction>     pp;
        const std::shared_ptr<const RelativeClock> _m_clock;

        // OpenGL objects
        Display*   dpy;
        Window     root;
        GLXContext glc;

        // Shared objects between ILLIXR and the application (either gldemo or Monado)
        bool              rendering_ready;
        graphics_api      client_backend;
        std::atomic<bool> image_handles_ready;

        // Left and right eye images
        std::array<std::vector<image_handle>, 2> _m_eye_image_handles;
        std::array<std::vector<GLuint>, 2>       _m_eye_swapchains;
        std::array<size_t, 2>                    _m_eye_swapchains_size;

        // Intermediate timewarp framebuffers for left and right eye textures
        std::array<GLuint, 2> _m_eye_output_textures;
        std::array<GLuint, 2> _m_eye_framebuffers;

#ifdef ILLIXR_MONADO
        std::array<image_handle, 2> _m_eye_output_handles;

        // Synchronization helper for Monado
        switchboard::writer<signal_to_quad> _m_signal_quad;

        // When using Monado, timewarp is a plugin and not a threadloop, but we still keep track of the iteration number
        std::size_t iteration_no = 0;
#else
        // Note: 0.9 works fine without hologram, but we need a larger safety net with hologram enabled
        static constexpr double DELAY_FRACTION = 0.9;

        // Switchboard plug for application eye buffer.
        switchboard::reader<rendered_frame> _m_eyebuffer;

        // Switchboard plug for publishing vsync estimates
        switchboard::writer<switchboard::event_wrapper<time_point>> _m_vsync_estimate;

        // Switchboard plug for publishing offloaded data
        switchboard::writer<texture_pose> _m_offload_data;
        // Timewarp only has vsync estimates with native-gl
        record_coalescer mtp_logger;
#endif

        record_coalescer timewarp_gpu_logger;

        // Switchboard plug for sending hologram calls
        switchboard::writer<hologram_input> _m_hologram;

        GLuint timewarpShaderProgram{};

        time_point time_last_swap{};

        HMD::hmd_info_t hmd_info{};

        // Eye sampler array
        GLuint eye_sampler_0{};
        GLuint eye_sampler_1;

        // Eye index uniform
        GLuint tw_eye_index_unif{};

        // VAOs
        GLuint tw_vao{};

        // Position and UV attribute locations
        GLuint distortion_pos_attr{};
        GLuint distortion_uv0_attr{};
        GLuint distortion_uv1_attr{};
        GLuint distortion_uv2_attr{};

        // Distortion mesh information
        GLuint num_distortion_vertices{};
        GLuint num_distortion_indices{};

        // Distortion mesh CPU buffers and GPU VBO handles
        std::vector<HMD::mesh_coord3d_t> distortion_positions;
        GLuint                           distortion_positions_vbo{};
        std::vector<GLuint>              distortion_indices;
        GLuint                           distortion_indices_vbo{};
        std::vector<HMD::uv_coord_t>     distortion_uv0;
        GLuint                           distortion_uv0_vbo{};
        std::vector<HMD::uv_coord_t>     distortion_uv1;
        GLuint                           distortion_uv1_vbo{};
        std::vector<HMD::uv_coord_t>     distortion_uv2;
        GLuint                           distortion_uv2_vbo{};

        // Handles to the start and end timewarp
        // transform matrices (3x4 uniforms)
        GLuint tw_start_transform_unif{};
        GLuint tw_end_transform_unif{};
        // bool uniform to check if the Y axis needs to be inverted
        GLuint flip_y_unif;
        // Basic perspective projection matrix
        Eigen::Matrix4f basicProjection;

        // Hologram call data
        ullong _hologram_seq{0};
        ullong _signal_quad_seq{0};

        bool disable_warp;

        bool enable_offload;

        // PBO buffer for reading texture image
        GLuint PBO_buffer{};

        duration offload_duration{};
    };
}