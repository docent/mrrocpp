/*!
 * \file kinematic_parameters_bird_hand.cc
 * \brief File containing definitions of kinematic_parameters class methods.
 *
 * \author kczajkowski
 * \date May 28, 2010
 */

#include <math.h>

#include "kinematic_parameters_bird_hand.h"

namespace mrrocpp {
namespace kinematics {
namespace bird_hand {

kinematic_parameters_bird_hand::kinematic_parameters_bird_hand() {
	gear[0] = 	//index_f[2]
	gear[1] = 	//index_f[1]
	gear[2] = 	//thumb_f[0]
	gear[3] = 	//thumb_f[1]
	gear[4] = 													//ring_f[1]
	gear[5] = 1024.0 * 275.0 * (11.3/3.1 * 10.95/5.1) /2.0/M_PI;//ring_f[2]
	gear[6] = 						  							//index_f[0] (rotation)
	gear[7] = 1024.0 * 275.0 * 2.1585 /2.0/M_PI; 				//index_f[0] (rotation);

	lower_limit_joint[0] = 0.0 * M_PI / 180.0;
	lower_limit_joint[1] = 0.0 * M_PI / 180.0;
	lower_limit_joint[2] = 0.0 * M_PI / 180.0;
	lower_limit_joint[3] = 0.0 * M_PI / 180.0;
	lower_limit_joint[4] = 0.0 * M_PI / 180.0;
	lower_limit_joint[5] = 0.0 * M_PI / 180.0;
	lower_limit_joint[6] = 0.0 * M_PI / 180.0;
	lower_limit_joint[7] = 0.0 * M_PI / 180.0;

	upper_limit_joint[0] = 0.45;//25.0 * M_PI / 180.0;
	upper_limit_joint[1] = 0.55;//30.0 * M_PI / 180.0;
	upper_limit_joint[2] = 0.55;//30.0 * M_PI / 180.0;
	upper_limit_joint[3] = 0.45;//25.0 * M_PI / 180.0;
	upper_limit_joint[4] = 0.55;//30.0 * M_PI / 180.0;
	upper_limit_joint[5] = 0.45;//25.0 * M_PI / 180.0;
	upper_limit_joint[6] = 90.0 * M_PI / 180.0;
	upper_limit_joint[7] = 90.0 * M_PI / 180.0;

	synchro_joint_position[0] = -1.0362;
	synchro_joint_position[1] = -2.5380;
	synchro_joint_position[2] = -2.0149;
	synchro_joint_position[3] = -2.0302;
	synchro_joint_position[4] = -0.7992;
	synchro_joint_position[5] = -1.6360;
	synchro_joint_position[6] = -1.6360;
	synchro_joint_position[7] = -1.6360;

//	synchro_joint_position[0] = 0.0;
//	synchro_joint_position[1] = 0.0;
//	synchro_joint_position[2] = 0.0;
//	synchro_joint_position[3] = 0.0;
//	synchro_joint_position[4] = 0.0;
//	synchro_joint_position[5] = 0.0;
//	synchro_joint_position[6] = 0.0;
//	synchro_joint_position[7] = 0.0;
}

} // namespace bird_hand
} // namespace kinematic
} // namespace mrrocpp
