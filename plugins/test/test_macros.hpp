#pragma once

#include "illixr/dynamic_lib.hpp"
#ifdef USE_EXTEND
#include "illixr/extended_window.hpp"
#endif
#include "illixr/phonebook.hpp"
#include "illixr/plugin.hpp"
#include "illixr/record_logger.hpp"
#include "illixr/stoplight.hpp"
#include "illixr/switchboard.hpp"
#include "illixr/sqlite_record_logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

using namespace ILLIXR;

typedef plugin* (*plugin_factory)(phonebook*);

#define START_SETUP \
class ILLIXRTest : public testing::Test { \
protected:                                 \
    void SetUp() override {               \
        std::vector<spdlog::sink_ptr> sinks; \
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>()); \
        auto logger = std::make_shared<spdlog::logger>("illixr", begin(sinks), end(sinks)); \
        logger->set_level(spdlog::level::from_str("error"));                       \
        spdlog::register_logger(logger); \
        pb = new phonebook();              \
        pb->register_impl<record_logger>(std::make_shared<sqlite_record_logger>()); \
        pb->register_impl<gen_guid>(std::make_shared<gen_guid>());         \
        pb->register_impl<switchboard>(std::make_shared<switchboard>(pb)); \
        pb->register_impl<Stoplight>(std::make_shared<Stoplight>());       \
        pb->register_impl<RelativeClock>(std::make_shared<RelativeClock>());

#define WITH_GL \
        pb->register_impl<xlib_gl_extended_window>(std::make_shared<xlib_gl_extended_window>(display_params::width_pixels, display_params::height_pixels, nullptr));

#define ADD_DEPENDENCY(D) \
        dependencies.emplace_back("lib" #D ".so");

#define END_SETUP \
        std::transform(dependencies.cbegin(), dependencies.cend(), std::back_inserter(dynamic_libs), [](const auto& dependency) { \
             return dynamic_lib::create(dependency);   \
        });       \
        std::transform(dynamic_libs.cbegin(), dynamic_libs.cend(), std::back_inserter(plugin_factories), [](const auto& lib) {   \
            return lib.template get<plugin* (*) (phonebook*)>("this_plugin_factory");                                           \
        });\
        std::transform(plugin_factories.cbegin(), plugin_factories.cend(), std::back_inserter(plugins), [this](const auto& pf) {  \
            return std::unique_ptr<plugin>{pf(pb)};  \
        });\
        std::for_each(plugins.cbegin(), plugins.cend(), [](const auto& plugin) { \
            plugin->start(); \
        });       \
                  \
        pb->lookup_impl<RelativeClock>()->start();                               \
        pb->lookup_impl<Stoplight>()->signal_ready();                            \
    }             \
    void TearDown() override { \
        if (!pb->lookup_impl<Stoplight>()->check_shutdown_complete()) {                                                           \
            pb->lookup_impl<Stoplight>()->signal_should_stop();                                                                   \
            pb->lookup_impl<switchboard>()->stop();                                                                              \
            for (const auto& plugin : plugins) {                                                                                  \
                plugin->stop();                                                                                                   \
            }     \
            pb->lookup_impl<Stoplight>()->signal_shutdown_complete();                                                             \
        }\
        delete sb;\
        delete pb;\
    }             \
    switchboard* sb = nullptr;                                                                                                    \
    phonebook* pb = nullptr;                                                                                                      \
    std::vector<std::string> dependencies;                                                                                        \
    std::vector<dynamic_lib> dynamic_libs;                                                                                        \
    std::vector<plugin_factory> plugin_factories;                                                                                 \
    std::vector<std::unique_ptr<plugin> > plugins;                                                                                \
};
