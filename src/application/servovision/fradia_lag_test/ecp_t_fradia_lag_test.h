/*
 * $Id: ecp_t_mboryn.h 3480 2010-01-08 18:48:29Z mboryn $
 *
 *  Created on: Dec 11, 2009
 *      Author: mboryn
 */

#ifndef ECP_T_MBORYN_H_
#define ECP_T_MBORYN_H_

#include "base/ecp/ecp_task.h"
<<<<<<< HEAD
#include "robot/irp6ot_m/ecp_r_irp6ot_m.h"
#include "base/ecp_mp/sensor/ecp_mp_s_cvfradia.h"
=======
#include "base/ecp/irp6_on_track/ecp_r_irp6ot.h"
>>>>>>> transmitter_template
#include "generator/ecp/ecp_g_smooth.h"

namespace mrrocpp {

namespace ecp {

namespace irp6ot_m {

namespace task {

/** @addtogroup servovision Visual servomechanism implementation
 *  @ingroup application
 *  Simple visual servo - blue ball follower
 *  @{
 */

/**
 *
 */
class ecp_t_fradia_lag_test: public mrrocpp::ecp::common::task::task
{
public:
	ecp_t_fradia_lag_test(mrrocpp::lib::configurator& _configurator);
	virtual ~ecp_t_fradia_lag_test();
	void main_task_algorithm(void);
protected:
	lib::sensor *vsp_fradia;
	mrrocpp::ecp::common::generator::smooth* smooth_gen;

	/** Moves (using smooth generator) to initial position. */
	void moveToInitialPosition();

	static const double initialPositionJoints[MAX_SERVOS_NR];
};

/** @} */// ecp_t_mboryn

} // namespace task

} // namespace irp6ot

} // namespace ecp

} // namespace mrrocpp

#endif /* ECP_T_MBORYN_H_ */
