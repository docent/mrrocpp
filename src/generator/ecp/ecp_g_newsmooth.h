/**
* @file	ecp_g_newsmooth.h
* @brief Smooth class and its methods.
* @author rtulwin
* @date	2010
*/

#if !defined(_ECP_GEN_NEWSMOOTH_H)
# define _ECP_GEN_NEWSMOOTH_H

#include <math.h>

using namespace std;

#include "generator/ecp/ecp_g_multiple_position.h"
#include "lib/trajectory_pose/bang_bang_trajectory_pose.h"
#include "generator/ecp/velocity_profile_calculator/bang_bang_profile.h"
#include "generator/ecp/trajectory_interpolator/bang_bang_interpolator.h"

using namespace std;

namespace mrrocpp {
namespace ecp {
namespace common {
namespace generator {

/**
 * Smooth trajectory generator which has an ability to calculate every trajectory (posiada moce super krowy).
 */
class newsmooth : public multiple_position<ecp_mp::common::trajectory_pose::bang_bang_trajectory_pose,
ecp::common::generator::trajectory_interpolator::bang_bang_interpolator,
ecp::common::generator::velocity_profile_calculator::bang_bang_profile> {

	private:
		/**
		 * Creates the vectors containning the information about the maximal and typical velocities and accelerations for each representation.
		 * @axes_num actual number of axes
		 */
		void create_velocity_vectors(int axes_num);
		/**
		 * Calculates trajectory.
		 * @return true if calculation was successful.
		 */
		bool calculate();
		/**
		 * Loads trajectory pose.
		 * @return true if the addition was successful
		 */
		bool load_trajectory_pose(const vector<double> & coordinates, lib::MOTION_TYPE motion_type, lib::ECP_POSE_SPECIFICATION pose_spec, const vector<double> & v, const vector<double> & a, const vector<double> & v_max, const vector<double> & a_max);
		/**
		 * Method used to print list of positions.
		 */
		void print_pose_vector();
		/**
		 * Prints single pose.
		 */
		void print_pose(vector<ecp_mp::common::trajectory_pose::bang_bang_trajectory_pose>::iterator & it);

	public:
		/**
		 * Constructor.
		 */
		newsmooth(common::task::task& _ecp_task, lib::ECP_POSE_SPECIFICATION pose_spec, int axes_num);
		/**
		 * Destructor.
		 */
		virtual ~newsmooth();
		/**
		 * Loads a single trajectory pose described in joint coordinates (absolute motion) to the list. Maximal velocities and accelerations are set automatically.
		 * @param coordinates desired position
		 * @return true if the addition was succesfull
		 */
		bool load_absolute_joint_trajectory_pose(const vector<double> & coordinates);
		/**
		 * Loads a single trajectory pose described in joint coordinates (relative motion) to the list. Maximal velocities and accelerations are set automatically.
		 * @param coordinates desired position
		 * @return true if the addition was succesfull
		 */
		bool load_relative_joint_trajectory_pose(const vector<double> & coordinates);
		/**
		 * Loads a single trajectory pose described in motor coordinates (absolute motion) to the list. Maximal velocities and accelerations are set automatically.
		 * @param coordinates desired position
		 * @return true if the addition was succesfull
		 */
		bool load_absolute_motor_trajectory_pose(const vector<double> & coordinates);
		/**
		 * Loads a single trajectory pose described in motor coordinates (relative motion) to the list. Maximal velocities and accelerations are set automatically.
		 * @param coordinates desired position
		 * @return true if the addition was succesfull
		 */
		bool load_relative_motor_trajectory_pose(const vector<double> & coordinates);
		/**
		 * Loads a single trajectory pose described in euler zyz coordinates (absolute motion) to the list. Maximal velocities and accelerations are set automatically.
		 * @param coordinates desired position
		 * @return true if the addition was succesfull
		 */
		bool load_absolute_euler_zyz_trajectory_pose(const vector<double> & coordinates);
		/**
		 * Loads a single trajectory pose described in euler zyz coordinates (relative motion) to the list. Maximal velocities and accelerations are set automatically.
		 * @param coordinates desired position
		 * @return true if the addition was succesfull
		 */
		bool load_relative_euler_zyz_trajectory_pose(const vector<double> & coordinates);
		/**
		 * Loads a single trajectory pose described in angle axis coordinates (absolute motion) to the list. Maximal velocities and accelerations are set automatically.
		 * @param coordinates desired position
		 * @return true if the addition was succesfull
		 */
		bool load_absolute_angle_axis_trajectory_pose(const vector<double> & coordinates);
		/**
		 * Loads a single trajectory pose described in angle axis coordinates (relative motion) to the list. Maximal velocities and accelerations are set automatically.
		 * @param coordinates desired position
		 * @return true if the addition was succesfull
		 */
		bool load_relative_angle_axis_trajectory_pose(const vector<double> & coordinates);

};

} // namespace generator
} // namespace common
} // namespace ecp
} // namespace mrrocpp

#endif
