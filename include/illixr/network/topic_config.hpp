#ifndef ILLIXR_TOPIC_CONFIG_HPP
#define ILLIXR_TOPIC_CONFIG_HPP

#include <chrono>
#include <optional>

struct topic_config {
    enum priority_type {
        LOWEST, LOW, MEDIUM, HIGH, HIGHEST
    };
    enum packetization_type {
        IMMEDIATE, DEFAULT, SUGGEST_LATENCY
    };
    priority_type priority = MEDIUM;
    bool retransmit = false;
    bool allow_out_of_order = false;
    packetization_type packetization = DEFAULT;
    std::optional<std::chrono::duration<long, std::nano>> latency;
};

#endif // ILLIXR_TOPIC_CONFIG_HPP
