#include <filesystem>
#include <fstream>

#include "common/threadloop.hpp"
#include "common/plugin.hpp"
#include "common/switchboard.hpp"
#include "common/data_format.hpp"
#include "common/phonebook.hpp"

#include <ecal/ecal.h>
#include <ecal/msg/protobuf/subscriber.h>

#include "vio_input.pb.h"
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/opencv.hpp>

using namespace ILLIXR;

class server_reader : public threadloop {
public:
	server_reader(std::string name_, phonebook* pb_)
		: threadloop{name_, pb_}
		, sb{pb->lookup_impl<switchboard>()}
		, _m_imu_cam{sb->get_writer<imu_cam_type_prof>("imu_cam")}
    {
		eCAL::Initialize(0, NULL, "VIO Server Reader");
		subscriber = eCAL::protobuf::CSubscriber<vio_input_proto::IMUCamVec>("vio_input");
		subscriber.AddReceiveCallback(std::bind(&server_reader::ReceiveVioInput, this, std::placeholders::_2));

		if (!std::filesystem::create_directory(data_path)) {
            std::cerr << "Failed to create data directory.";
		}
		receive_time.open(data_path + "/receive_time.csv");
	}

private:

    boost::lockfree::spsc_queue<switchboard::ptr<imu_cam_type_prof>> _m_imu_cam;

    virtual void _p_one_iteration() {

    }

    // decompress byte array to cv::Mat
    cv::Mat decompress_mat(std::vector<uchar>& compressed_data) {
        cv::Mat uncompressed_data;
        cv::Mat compressed_data_mat(1, compressed_data.size(), CV_8UC1, (void *) compressed_data.data());
        cv::imdecode(compressed_data_mat, CV_LOAD_IMAGE_UNCHANGED, &uncompressed_data);
        return uncompressed_data;
    }

	void ReceiveVioInput(const vio_input_proto::IMUCamVec& vio_input) {	
		unsigned long long curr_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		double sec_to_trans = (curr_time - vio_input.real_timestamp()) / 1e9;
		receive_time << vio_input.frame_id() << "," << vio_input.real_timestamp() << "," << sec_to_trans * 1e3 << std::endl;

		// Loop through all IMU values first then the cam frame	
		for (int i = 0; i < vio_input.imu_cam_data_size(); i++) {
			vio_input_proto::IMUCamData curr_data = vio_input.imu_cam_data(i);

			std::optional<cv::Mat> cam0 = std::nullopt;
			std::optional<cv::Mat> cam1 = std::nullopt;

			if (curr_data.rows() != -1 && curr_data.cols() != -1) {

				// Must do a deep copy of the received data (in the form of a string of bytes)
				auto img0_copy = std::make_shared<std::string>(std::string(curr_data.img0_data()));
				auto img1_copy = std::make_shared<std::string>(std::string(curr_data.img1_data()));
                auto original_img0_copy = std::make_shared<std::string>(std::string(curr_data.original_img0_data()));
                auto original_img1_copy = std::make_shared<std::string>(std::string(curr_data.original_img1_data()));

                auto img0_vec = std::vector<uchar>(img0_copy->begin(), img0_copy->end());
                auto img1_vec = std::vector<uchar>(img1_copy->begin(), img1_copy->end());

                // profile nano time
                auto start_time = std::chrono::high_resolution_clock::now();
                cv::Mat img0 = decompress_mat(img0_vec);
                cv::Mat img1 = decompress_mat(img1_vec);
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
                std::cout << "Decompression time: " << duration << " microseconds" << std::endl;

                cv::Mat original_img0(curr_data.rows(), curr_data.cols(), CV_8UC1, original_img0_copy->data());
                cv::Mat original_img1(curr_data.rows(), curr_data.cols(), CV_8UC1, original_img1_copy->data());

//                imshow("img0", img0);
//                imshow("original_img0", original_img0);
//                while (true) {
//                    char key = cv::waitKey(1);
//                    if (key == 'q') {
//                        break;
//                    }
//                }

//                std::cout << "Received image with size: " << img0.size() << std::endl;
//                std::cout << "Received original image with size: " << original_img0.size() << std::endl;

				cam0 = std::make_optional<cv::Mat>(original_img0);
				cam1 = std::make_optional<cv::Mat>(original_img1);
			}

			_m_imu_cam.put(_m_imu_cam.allocate<imu_cam_type_prof>(
				imu_cam_type_prof {
					vio_input.frame_id(),
					time_point{std::chrono::nanoseconds{curr_data.timestamp()}},
					time_point{std::chrono::nanoseconds{vio_input.real_timestamp()}}, // Timestamp of when the device sent the packet
					time_point{std::chrono::nanoseconds{curr_time}}, // Timestamp of receive time of the packet
					time_point{std::chrono::nanoseconds{vio_input.dataset_timestamp()}}, // Timestamp of the sensor data
					Eigen::Vector3f{curr_data.angular_vel().x(), curr_data.angular_vel().y(), curr_data.angular_vel().z()},
					Eigen::Vector3f{curr_data.linear_accel().x(), curr_data.linear_accel().y(), curr_data.linear_accel().z()},
					cam0,
					cam1
				}
			));
		}

		// unsigned long long after_time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		// double sec_to_push = (after_time - curr_time) / 1e9;
		// std::cout << vio_input.frame_id() << ": Seconds to push data (ms): " << sec_to_push * 1e3 << std::endl;
	}

	const std::string data_path = std::filesystem::current_path().string() + "/recorded_data";
    std::ofstream receive_time;

    const std::shared_ptr<switchboard> sb;
	switchboard::writer<imu_cam_type_prof> _m_imu_cam;

	eCAL::protobuf::CSubscriber<vio_input_proto::IMUCamVec> subscriber;
};

PLUGIN_MAIN(server_reader)