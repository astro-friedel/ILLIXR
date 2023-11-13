#pragma once
#include "illixr/data_format.hpp"
#include "illixr/plugin.hpp"
#include "illixr/switchboard.hpp"

namespace ILLIXR {

    constexpr duration IMU_SAMPLE_LIFETIME{std::chrono::seconds{5}};

    class rk4_integrator : public plugin {
    public:
        rk4_integrator(const std::string& name_, phonebook* pb_);

        void callback(const switchboard::ptr<const imu_type>& datum);
    private:
        void clean_imu_vec(time_point timestamp);
        void propagate_imu_values(time_point real_time);
        static std::vector<imu_type> select_imu_readings(const std::vector<imu_type>& imu_data,
                                                         time_point time_begin,
                                                         time_point time_end);
        static imu_type interpolate_imu(const imu_type& imu_1, const imu_type& imu_2, time_point timestamp);
        static void predict_mean_rk4(const Eigen::Vector4d& quat, const Eigen::Vector3d& pos,
                                     const Eigen::Vector3d& vel, double dt,
                                     const Eigen::Vector3d& w_hat1, const Eigen::Vector3d& a_hat1,
                                     const Eigen::Vector3d& w_hat2, const Eigen::Vector3d& a_hat2,
                                     Eigen::Vector4d& new_q, Eigen::Vector3d& new_v,
                                     Eigen::Vector3d& new_p);
        /**
     * @brief Integrated quaternion from angular velocity
     *
     * See equation (48) of trawny tech report [Indirect Kalman Filter for 3D Attitude
     * Estimation](http://mars.cs.umn.edu/tr/reports/Trawny05b.pdf).
     *
     */
        static inline Eigen::Matrix<double, 4, 4> Omega(Eigen::Matrix<double, 3, 1> w) {
            Eigen::Matrix<double, 4, 4> mat;
            mat.block(0, 0, 3, 3) = -skew_x(w);
            mat.block(3, 0, 1, 3) = -w.transpose();
            mat.block(0, 3, 3, 1) = w;
            mat(3, 3)             = 0;
            return mat;
        }

        /**
         * @brief Normalizes a quaternion to make sure it is unit norm
         * @param q_t Quaternion to normalized
         * @return Normalized quaterion
         */
        static inline Eigen::Matrix<double, 4, 1> quatnorm(Eigen::Matrix<double, 4, 1> q_t) {
            if (q_t(3, 0) < 0) {
                q_t *= -1;
            }
            return q_t / q_t.norm();
        }

        /**
         * @brief Skew-symmetric matrix from a given 3x1 vector
         *
         * This is based on equation 6 in [Indirect Kalman Filter for 3D Attitude
         * Estimation](http://mars.cs.umn.edu/tr/reports/Trawny05b.pdf): \f{align*}{ \lfloor\mathbf{v}\times\rfloor =
         *  \begin{bmatrix}
         *  0 & -v_3 & v_2 \\ v_3 & 0 & -v_1 \\ -v_2 & v_1 & 0
         *  \end{bmatrix}
         * @f}
         *
         * @param[in] w 3x1 vector to be made a skew-symmetric
         * @return 3x3 skew-symmetric matrix
         */
        static inline Eigen::Matrix<double, 3, 3> skew_x(const Eigen::Matrix<double, 3, 1>& w) {
            Eigen::Matrix<double, 3, 3> w_x;
            w_x << 0, -w(2), w(1), w(2), 0, -w(0), -w(1), w(0), 0;
            return w_x;
        }

        /**
         * @brief Converts JPL quaterion to SO(3) rotation matrix
         *
         * This is based on equation 62 in [Indirect Kalman Filter for 3D Attitude
         * Estimation](http://mars.cs.umn.edu/tr/reports/Trawny05b.pdf): \f{align*}{ \mathbf{R} =
         * (2q_4^2-1)\mathbf{I}_3-2q_4\lfloor\mathbf{q}\times\rfloor+2\mathbf{q}^\top\mathbf{q}
         * @f}
         *
         * @param[in] q JPL quaternion
         * @return 3x3 SO(3) rotation matrix
         */
        static inline Eigen::Matrix<double, 3, 3> quat_2_Rot(const Eigen::Matrix<double, 4, 1>& q) {
            Eigen::Matrix<double, 3, 3> q_x = skew_x(q.block(0, 0, 3, 1));
            Eigen::MatrixXd             Rot = (2 * std::pow(q(3, 0), 2) - 1) * Eigen::MatrixXd::Identity(3, 3) - 2 * q(3, 0) * q_x +
                                              2 * q.block(0, 0, 3, 1) * (q.block(0, 0, 3, 1).transpose());
            return Rot;
        }

        /**
         * @brief Multiply two JPL quaternions
         *
         * This is based on equation 9 in [Indirect Kalman Filter for 3D Attitude
         * Estimation](http://mars.cs.umn.edu/tr/reports/Trawny05b.pdf). We also enforce that the quaternion is unique by having q_4
         * be greater than zero. \f{align*}{ \bar{q}\otimes\bar{p}= \mathcal{L}(\bar{q})\bar{p}= \begin{bmatrix}
         *  q_4\mathbf{I}_3+\lfloor\mathbf{q}\times\rfloor & \mathbf{q} \\
         *  -\mathbf{q}^\top & q_4
         *  \end{bmatrix}
         *  \begin{bmatrix}
         *  \mathbf{p} \\ p_4
         *  \end{bmatrix}
         * @f}
         *
         * @param[in] q First JPL quaternion
         * @param[in] p Second JPL quaternion
         * @return 4x1 resulting p*q quaternion
         */
        static inline Eigen::Matrix<double, 4, 1> quat_multiply(const Eigen::Matrix<double, 4, 1>& q,
                                                                const Eigen::Matrix<double, 4, 1>& p) {
            Eigen::Matrix<double, 4, 1> q_t;
            Eigen::Matrix<double, 4, 4> Qm;
            // create big L matrix
            Qm.block(0, 0, 3, 3) = q(3, 0) * Eigen::MatrixXd::Identity(3, 3) - skew_x(q.block(0, 0, 3, 1));
            Qm.block(0, 3, 3, 1) = q.block(0, 0, 3, 1);
            Qm.block(3, 0, 1, 3) = -q.block(0, 0, 3, 1).transpose();
            Qm(3, 3)             = q(3, 0);
            q_t                  = Qm * p;
            // ensure unique by forcing q_4 to be >0
            if (q_t(3, 0) < 0) {
                q_t *= -1;
            }
            // normalize and return
            return q_t / q_t.norm();
        }


        const std::shared_ptr<switchboard> sb;

        // IMU Data, Sequence Flag, and State Vars Needed
        switchboard::reader<imu_integrator_input> _m_imu_integrator_input;

        // IMU Biases
        switchboard::writer<imu_raw_type> _m_imu_raw;
        std::vector<imu_type>             _imu_vec;
        duration                          last_imu_offset;
        bool                              has_last_offset = false;

        [[maybe_unused]] int    counter       = 0;
        [[maybe_unused]] int    cam_count     = 0;
        [[maybe_unused]] int    total_imu     = 0;
        [[maybe_unused]] double last_cam_time = 0;

    };
}