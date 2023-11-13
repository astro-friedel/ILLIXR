#pragma once

// clang-format off
#include <GL/glew.h> // GLEW has to be loaded before other GL libraries
// clang-format on

#include "illixr/extended_window.hpp"
#include "illixr/gl_util/obj.hpp"
#include "illixr/pose_prediction.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/threadloop.hpp"

static constexpr std::chrono::milliseconds VSYNC_SAFETY_DELAY{1};

namespace ILLIXR {


    class gldemo : public threadloop {
    public:
        // Public constructor, create_component passes Switchboard handles ("plugs")
        // to this constructor. In turn, the constructor fills in the private
        // references to the switchboard plugs, so the component can read the
        // data whenever it needs to.
        gldemo(const std::string& name_, phonebook* pb_);
        void wait_vsync();
        void _p_thread_setup() override;
        void _p_one_iteration() override;
        void start() override;
    private:
        static void createSharedEyebuffer(GLuint* texture_handle);
        void createFBO(const GLuint* texture_handle, GLuint* fbo, GLuint* depth_target);

        const std::unique_ptr<const xlib_gl_extended_window>              xwin;
        const std::shared_ptr<switchboard>                                sb;
        const std::shared_ptr<pose_prediction>                            pp;
        const std::shared_ptr<const RelativeClock>                        _m_clock;
        const switchboard::reader<switchboard::event_wrapper<time_point>> _m_vsync;

        // Switchboard plug for application eye buffer.
        // We're not "writing" the actual buffer data,
        // we're just atomically writing the handle to the
        // correct eye/framebuffer in the "swapchain".
        switchboard::writer<image_handle>   _m_image_handle;
        switchboard::writer<rendered_frame> _m_eyebuffer;

        GLuint eyeTextures[2]{};
        GLuint eyeTextureFBO{};
        GLuint eyeTextureDepthTarget{};

        unsigned char which_buffer = 0;

        GLuint demo_vao{};
        GLuint demoShaderProgram{};

        GLuint vertexPosAttr{};
        GLuint vertexNormalAttr{};
        GLuint modelViewAttr{};
        GLuint projectionAttr{};

        GLuint colorUniform{};

        ObjScene demoscene;

        Eigen::Matrix4f basicProjection;

        time_point lastTime{};
#ifndef NDEBUG
        size_t log_count  = 0;
        size_t LOG_PERIOD = 20;
#endif

    };
}