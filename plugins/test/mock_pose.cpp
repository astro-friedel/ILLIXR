#include <gmock/gmock.h>
#include "illixr/plugin.hpp"
#include "illixr/pose_prediction.hpp"

using namespace ILLIXR;

class mock_pose_impl : public pose_prediction {
public:

    MOCK_METHOD(fast_pose_type, get_fast_pose,(), (const, override));

    MOCK_METHOD(pose_type, get_true_pose, (), (const, override));

    MOCK_METHOD(fast_pose_type, get_fast_pose, (time_point future_time), (const, override));

    MOCK_METHOD(bool, fast_pose_reliable, (), (const, override));

    MOCK_METHOD(bool, true_pose_reliable, (), (const, override));

    MOCK_METHOD(void, set_offset, (const Eigen::Quaternionf& orientation), (override));

    MOCK_METHOD(Eigen::Quaternionf, get_offset, (), (override));

    MOCK_METHOD(pose_type, correct_pose, (const pose_type &pose), (const, override));
};

class mock_pose : public plugin {
public:
    mock_pose(const std::string& name, phonebook* pb) : plugin{name, pb} {
        pb->register_impl<pose_prediction>(std::static_pointer_cast<pose_prediction>(std::make_shared<mock_pose_impl>()));
    }

    ~mock_pose() override = default;
};

PLUGIN_MAIN(mock_pose)
