#include "plugin.hpp"

#include <opencv2/opencv.hpp>

#define RGB_MODE   0
#define DEPTH_MODE 0

using namespace ILLIXR;

openni_plugin::openni_plugin(const std::string& name_, phonebook* pb_)
        : threadloop{name_, pb_}
        , sb{pb->lookup_impl<switchboard>()}
        , _m_clock{pb->lookup_impl<RelativeClock>()}
        , _m_rgb_depth{sb->get_writer<rgb_depth_type>("rgb_depth")} {
    spdlogger(std::getenv("OPENNI_LOG_LEVEL"));
    if (!camera_initialize()) {
        spdlog::get(name)->error("Initialization failed");
        exit(0);
    }
}

openni_plugin::~openni_plugin() {
    _color.destroy();
    _depth.destroy();
}


threadloop::skip_option openni_plugin::_p_should_skip() {
    auto now  = std::chrono::steady_clock::now();
    _cam_time = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
    if (_cam_time > _last_ts) {
        std::this_thread::sleep_for(std::chrono::milliseconds{_time_sleep});
        return skip_option::run;
    } else {
        return skip_option::skip_and_yield;
    }
}

void openni_plugin::_p_one_iteration() {
    RAC_ERRNO_MSG("openni at start of _p_one_iteration");

    // read cam data
    _color.readFrame(&_color_frame);
    _depth.readFrame(&_depth_frame);
    // get timestamp
    assert(_color_frame.getTimestamp() != _depth_frame.getTimestamp());

    // convert to cv format
    cv::Mat colorMat;
    colorMat.create(_color_frame.getHeight(), _color_frame.getWidth(), CV_8UC3);
    auto* colorBuffer = (const openni::RGB888Pixel*) _color_frame.getData();
    memcpy(colorMat.data, colorBuffer, 3 * _color_frame.getHeight() * _color_frame.getWidth() * sizeof(uint8_t));
    cv::cvtColor(colorMat, colorMat, cv::COLOR_BGR2BGRA);
    colorMat.convertTo(colorMat, CV_8UC4);

    cv::Mat depthMat;
    depthMat.create(_depth_frame.getHeight(), _depth_frame.getWidth(), CV_16UC1);
    auto* depthBuffer = (const openni::DepthPixel*) _depth_frame.getData();
    memcpy(depthMat.data, depthBuffer, _depth_frame.getHeight() * _depth_frame.getWidth() * sizeof(uint16_t));

    assert(_cam_time);
    if (_m_first_time == 0) {
        _m_first_time      = _cam_time;
        _m_first_real_time = _m_clock->now();
    }
    time_point _cam_time_point{_m_first_real_time + std::chrono::nanoseconds(_cam_time - _m_first_time)};
    _m_rgb_depth.put(_m_rgb_depth.allocate(_cam_time_point, colorMat, depthMat));

    _last_ts = _cam_time;
    RAC_ERRNO_MSG("openni at end of _p_one_iteration");
}

bool openni_plugin::camera_initialize() {
    // initialize openni
    _device_status = openni::OpenNI::initialize();
    if (_device_status != openni::STATUS_OK)
        spdlog::get(name)->error("Initialize failed: {}", openni::OpenNI::getExtendedError());

    // open _device
    _device_status = _device.open(openni::ANY_DEVICE);
    if (_device_status != openni::STATUS_OK)
        spdlog::get(name)->error("Device open failed: {}", openni::OpenNI::getExtendedError());

    /*_____________________________ DEPTH ___________________________*/
    // create _depth channel
    _device_status = _depth.create(_device, openni::SENSOR_DEPTH);
    if (_device_status != openni::STATUS_OK)
        spdlog::get(name)->warn("Couldn't find depth stream:\n{}", openni::OpenNI::getExtendedError());

    // get _depth options
    const openni::SensorInfo*               depthInfo  = _device.getSensorInfo(openni::SENSOR_DEPTH);
    const openni::Array<openni::VideoMode>& modesDepth = depthInfo->getSupportedVideoModes();
#ifndef NDEBUG
    for (int i = 0; i < modesDepth.getSize(); i++) {
        spdlog::get(name)->debug("Depth Mode {}: {}x{}, {} fps, {} format", i, modesDepth[i].getResolutionX(),
                                 modesDepth[i].getResolutionY(), modesDepth[i].getFps(), modesDepth[i].getPixelFormat());
    }
#endif
    _device_status = _depth.setVideoMode(modesDepth[DEPTH_MODE]);
    if (openni::STATUS_OK != _device_status)
        spdlog::get(name)->error("error: depth fromat not supprted...");
    // start _depth stream
    _device_status = _depth.start();
    if (_device_status != openni::STATUS_OK)
        spdlog::get(name)->error("Couldn't start the _depth stream {}", openni::OpenNI::getExtendedError());

    /*_____________________________ COLOR ___________________________*/
    // create _color channel
    _device_status = _color.create(_device, openni::SENSOR_COLOR);
#ifndef NDEBUG
    if (_device_status != openni::STATUS_OK)
        spdlog::get(name)->debug("Couldn't find color stream:\n{}", openni::OpenNI::getExtendedError());
#endif

    // get _color options
    const openni::SensorInfo*               colorInfo  = _device.getSensorInfo(openni::SENSOR_COLOR);
    const openni::Array<openni::VideoMode>& modesColor = colorInfo->getSupportedVideoModes();
#ifndef NDEBUG
    for (int i = 0; i < modesColor.getSize(); i++) {
        spdlog::get(name)->debug("Color Mode {}: {}x{}, {} fps, {} format", i, modesColor[i].getResolutionX(),
                                 modesColor[i].getResolutionY(), modesColor[i].getFps(), modesColor[i].getPixelFormat());
    }
#endif
    _device_status = _color.setVideoMode(modesColor[RGB_MODE]);
    if (openni::STATUS_OK != _device_status)
        spdlog::get(name)->error("error: color format not supprted...");
    // start _color stream
    _device_status = _color.start();
#ifndef NDEBUG
    if (_device_status != openni::STATUS_OK)
        spdlog::get(name)->debug("Couldn't start color stream:\n{}", openni::OpenNI::getExtendedError());
#endif
    int min_fps = std::min(modesColor[RGB_MODE].getFps(), modesDepth[DEPTH_MODE].getFps());
    _time_sleep = static_cast<uint64_t>((1.0f / static_cast<float>(min_fps)) * 1000);

    return _depth.isValid() && _color.isValid();
}

PLUGIN_MAIN(openni_plugin)
