// -------------------------------------------------------------------------
//                              task/mp_t_haptic.cc
//
// MP task for two robot haptic device
//
// -------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <sstream>

#include "lib/typedefs.h"
#include "lib/impconst.h"
#include "lib/com_buf.h"
#include "lib/srlib.h"
#include "base/mp/mp.h"
#include "ecp_mp_t_swarmitfix.h"
#include "mp_t_swarmitfix.h"
#include "lib/single_thread_port.h"
#include "lib/mrmath/mrmath.h"
#include "lib/data_port_headers/epos.h"
#include "robot/smb/smb_const.h"
#include "robot/spkm/spkm_const.h"
#include "robot/shead/shead_const.h"
#include "generator/ecp/ecp_mp_g_transparent.h"
#include "ecp_mp_g_epos.h"

namespace mrrocpp {
namespace mp {
namespace task {

task* return_created_mp_task(lib::configurator &_config)
{
	return new swarmitfix(_config);
}

swarmitfix::swarmitfix(lib::configurator &_config) :
	task(_config)
{
}

void swarmitfix::main_task_algorithm(void)
{

	sr_ecp_msg->message("New swarmitfix series");

	// wlaczenie generatora transparentnego w obu robotach
	set_next_ecps_state(ecp_mp::common::generator::ECP_GEN_TRANSPARENT, (int) 0, "", 0, 1, lib::ROBOT_SPKM.c_str());
	set_next_ecps_state(ecp_mp::common::generator::ECP_GEN_TRANSPARENT, (int) 0, "", 0, 1, lib::ROBOT_SMB.c_str());
	set_next_ecps_state(ecp_mp::common::generator::ECP_GEN_TRANSPARENT, (int) 0, "", 0, 1, lib::ROBOT_SHEAD.c_str());

	double a = 2.88;

	std::string astring;

	astring = "11";
	std::stringstream ss(std::stringstream::in | std::stringstream::out);
	ss << a;
	astring = ss.str();
	lib::single_thread_port_manager port_manager;

	lib::single_thread_port <int> int_port("int_port_label", port_manager);
	lib::single_thread_port <int>* int_port_from_manager;

	int_port_from_manager = port_manager.get_port <int> ("int_port_label");

	int int_port_data_input = 16;
	int int_port_data_output;

	int_port_from_manager->set(int_port_data_input);
	int_port_from_manager->get(int_port_data_output);

	ss << " " << int_port_data_output;

	sr_ecp_msg->message(ss.str().c_str());

	send_end_motion_to_ecps(1, lib::ROBOT_SPKM.c_str());
	/*
	 sr_ecp_msg->message("2");
	 set_next_ecps_state(ecp_mp::common::generator::ECP_GEN_SLEEP, (int) 5, "",  0,1,
	 lib::ROBOT_SPKM);
	 sr_ecp_msg->message("3");
	 run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots(
	 1, 1, lib::ROBOT_SPKM, lib::ROBOT_SPKM);
	 */
	sr_ecp_msg->message("4");

	char tmp_string[MP_2_ECP_STRING_SIZE];

	lib::epos_cubic_command epos_params;

	epos_params.da[4] = 3.7;

	memcpy(tmp_string, &epos_params, sizeof(epos_params));

	set_next_ecps_state(ecp_mp::common::generator::ECP_GEN_EPOS_CUBIC, (int) 5, tmp_string, sizeof(epos_params), 1, lib::ROBOT_SPKM.c_str());
	sr_ecp_msg->message("5");
	run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots(1, 1, lib::ROBOT_SPKM.c_str(), lib::ROBOT_SPKM.c_str());

	sr_ecp_msg->message("END");

	send_end_motion_to_ecps(2, lib::ROBOT_SMB.c_str(), lib::ROBOT_SHEAD.c_str());

}

} // namespace task
} // namespace mp
} // namespace mrrocpp
