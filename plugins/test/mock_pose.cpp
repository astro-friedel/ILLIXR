#include "illixr/plugin.hpp"
#include "illixr/pose_prediction.hpp"

using namespace ILLIXR;

class mock_pose_impl : public pose_prediction {
public:
    explicit mock_pose_impl(const phonebook* const pb) :
    sb{pb->lookup_impl<switchboard>()} {}

    fast_pose_type get_fast_pose() const override {

    }

    pose_type get_true_pose() const override {

    }

    fast_pose_type get_fast_pose(time_point future_time) const override {

    }

    bool fast_pose_reliable() const override {
        return false;
    }

    bool true_pose_reliable() const override {
        return false;
    }

    void set_offset(const Eigen::Quaternionf& orientation) override {

    }

    Eigen::Quaternionf get_offset() override {

    }

    pose_type correct_pose(const pose_type &pose) const override {

    }
    ~mock_pose_impl() override = default;

private:
    const std::shared_ptr<switchboard> sb;
};

class mock_pose : public plugin {
public:
    mock_pose(const std::string& name, phonebook* pb) : plugin{name, pb} {
        pb->register_impl<pose_prediction>(std::static_pointer_cast<pose_prediction>(std::make_shared<mock_pose_impl>(pb)));
    }

    ~mock_pose() override = default;
};

PLUGIN_MAIN(mock_pose)
