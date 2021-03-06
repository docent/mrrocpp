/*
 * ib_eih_visual_servo.h
 *
 *  Created on: Apr 21, 2010
 *      Author: mboryn
 */

#ifndef IB_EIH_VISUAL_SERVO_H_
#define IB_EIH_VISUAL_SERVO_H_

#include "visual_servo.h"
#include "visual_servo_types.h"

using visual_servo_types::image_based_reading;
using visual_servo_types::image_based_configuration;

typedef ecp_mp::sensor::fradia_sensor <image_based_configuration, image_based_reading> ib_fradia_sensor;

namespace mrrocpp {

namespace ecp {

namespace common {

namespace generator {

/** @addtogroup servovision
 *  @{
 */

/**
 *
 */
class ib_eih_visual_servo : public mrrocpp::ecp::common::generator::visual_servo
{
public:
			ib_eih_visual_servo(boost::shared_ptr <visual_servo_regulator> regulator, const std::string & section_name, mrrocpp::lib::configurator& configurator);
	virtual ~ib_eih_visual_servo();

	virtual lib::Homog_matrix get_position_change(const lib::Homog_matrix& current_position, double dt);
	virtual boost::shared_ptr <ecp_mp::sensor::sensor_interface> get_vsp_fradia();
protected:
	boost::shared_ptr <ib_fradia_sensor> vsp_fradia;

	lib::Homog_matrix e_T_c_position;
private:
};

/** @} */

}//namespace generator

}

}

}

#endif /* IB_EIH_VISUAL_SERVO_H_ */
