// -------------------------------------------------------------------------
//                            ecp.cc
//            Effector Control Process (lib::ECP) - methods
// Funkcje do tworzenia procesow ECP
// robot - irp6_mechatronika
//
// -------------------------------------------------------------------------

#include "lib/impconst.h"
#include "lib/com_buf.h"
#include "lib/mis_fun.h"

#include "robot/smb/ecp_r_smb.h"

namespace mrrocpp {
namespace ecp {
namespace smb {

robot::robot(lib::configurator &_config, lib::sr_ecp &_sr_ecp) :
	ecp_robot(lib::ROBOT_SMB, SMB_NUM_OF_SERVOS, EDP_SMB_SECTION, _config, _sr_ecp), kinematics_manager(),
			epos_cubic_command_data_port(lib::EPOS_CUBIC_COMMAND_DATA_PORT, port_manager),
			epos_trapezoidal_command_data_port(lib::EPOS_TRAPEZOIDAL_COMMAND_DATA_PORT, port_manager),
			smb_multi_pin_insertion_data_port(lib::SMB_MULTI_PIN_INSERTION_DATA_PORT, port_manager),
			smb_multi_pin_locking_data_port(lib::SMB_MULTI_PIN_LOCKING_DATA_PORT, port_manager),
			epos_reply_data_request_port(lib::EPOS_REPLY_DATA_REQUEST_PORT, port_manager),
			smb_multi_leg_reply_data_request_port(lib::SMB_MULTI_LEG_REPLY_DATA_REQUEST_PORT, port_manager)
{

	create_kinematic_models_for_given_robot();
}

robot::robot(common::task::task& _ecp_object) :
	ecp_robot(lib::ROBOT_SMB, SMB_NUM_OF_SERVOS, EDP_SMB_SECTION, _ecp_object), kinematics_manager(),
			epos_cubic_command_data_port(lib::EPOS_CUBIC_COMMAND_DATA_PORT, port_manager),
			epos_trapezoidal_command_data_port(lib::EPOS_TRAPEZOIDAL_COMMAND_DATA_PORT, port_manager),
			smb_multi_pin_insertion_data_port(lib::SMB_MULTI_PIN_INSERTION_DATA_PORT, port_manager),
			smb_multi_pin_locking_data_port(lib::SMB_MULTI_PIN_LOCKING_DATA_PORT, port_manager),
			epos_reply_data_request_port(lib::EPOS_REPLY_DATA_REQUEST_PORT, port_manager),
			smb_multi_leg_reply_data_request_port(lib::SMB_MULTI_LEG_REPLY_DATA_REQUEST_PORT, port_manager)
{

	create_kinematic_models_for_given_robot();
}

// Stworzenie modeli kinematyki dla robota IRp-6 na postumencie.
void robot::create_kinematic_models_for_given_robot(void)
{
	// Stworzenie wszystkich modeli kinematyki.
	add_kinematic_model(new kinematics::smb::model());
	// Ustawienie aktywnego modelu.
	set_kinematic_model(0);
}

void robot::create_command()
{

	//	int new_data_counter;
	bool is_new_data;
	bool is_new_request;

	sr_ecp_msg.message("create_command");

	is_new_data = false;

	if (epos_cubic_command_data_port.get(epos_cubic_command_structure) == mrrocpp::lib::NewData) {
		ecp_command.instruction.set_type = ARM_DEFINITION;
		// generator command interpretation
		// narazie proste przepisanie

		ecp_edp_cbuffer.variant = lib::SMB_CBUFFER_EPOS_CUBIC_COMMAND;

		ecp_edp_cbuffer.epos_cubic_command_structure = epos_cubic_command_structure;

		if (is_new_data) {
			throw ecp_robot::ECP_error(lib::NON_FATAL_ERROR, INVALID_COMMAND_TO_EDP);
		} else {
			is_new_data = true;
		}
	}

	if (epos_trapezoidal_command_data_port.get(epos_trapezoidal_command_structure) == mrrocpp::lib::NewData) {
		ecp_command.instruction.set_type = ARM_DEFINITION;
		// generator command interpretation
		// narazie proste przepisanie

		ecp_edp_cbuffer.variant = lib::SMB_CBUFFER_EPOS_TRAPEZOIDAL_COMMAND;

		ecp_edp_cbuffer.epos_trapezoidal_command_structure = epos_trapezoidal_command_structure;

		if (is_new_data) {
			throw ecp_robot::ECP_error(lib::NON_FATAL_ERROR, INVALID_COMMAND_TO_EDP);
		} else {
			is_new_data = true;
		}
	}

	/*
	 if (epos_gen_parameters_data_port.get(epos_gen_parameters_structure) == mrrocpp::lib::NewData) {
	 ecp_command.instruction.set_type = ARM_DEFINITION;
	 // generator command interpretation
	 // narazie proste przepisanie

	 ecp_edp_cbuffer.variant = lib::SMB_CBUFFER_EPOS_GEN_PARAMETERS;

	 ecp_edp_cbuffer.epos_gen_parameters_structure = epos_gen_parameters_structure;

	 if (is_new_data) {
	 throw ecp_robot::ECP_error(lib::NON_FATAL_ERROR, INVALID_COMMAND_TO_EDP);
	 } else {
	 is_new_data = true;
	 }
	 }
	 */
	if (smb_multi_pin_insertion_data_port.get(smb_multi_pin_insertion_structure) == mrrocpp::lib::NewData) {
		ecp_command.instruction.set_type = ARM_DEFINITION;
		// generator command interpretation
		// narazie proste przepisanie

		ecp_edp_cbuffer.variant = lib::SMB_CBUFFER_PIN_INSERTION;

		ecp_edp_cbuffer.multi_pin_insertion = smb_multi_pin_insertion_structure;

		if (is_new_data) {
			throw ecp_robot::ECP_error(lib::NON_FATAL_ERROR, INVALID_COMMAND_TO_EDP);
		} else {
			is_new_data = true;
		}
	}

	if (smb_multi_pin_locking_data_port.get(smb_multi_pin_locking_structure) == mrrocpp::lib::NewData) {
		ecp_command.instruction.set_type = ARM_DEFINITION;
		// generator command interpretation
		// narazie proste przepisanie

		ecp_edp_cbuffer.variant = lib::SMB_CBUFFER_PIN_LOCKING;

		ecp_edp_cbuffer.multi_pin_locking = smb_multi_pin_locking_structure;

		if (is_new_data) {
			throw ecp_robot::ECP_error(lib::NON_FATAL_ERROR, INVALID_COMMAND_TO_EDP);
		} else {
			is_new_data = true;
		}
	}

	is_new_request = (epos_reply_data_request_port.is_new_request()
			|| smb_multi_leg_reply_data_request_port.is_new_request());

	communicate_with_edp = true;

	if (is_new_data && is_new_request) {
		ecp_command.instruction.instruction_type = lib::SET_GET;
	} else if (is_new_data) {
		ecp_command.instruction.instruction_type = lib::SET;
	} else if (is_new_request) {
		ecp_command.instruction.instruction_type = lib::GET;
	} else {
		communicate_with_edp = false;
	}

	if (is_new_request) {
		ecp_command.instruction.get_type = ARM_DEFINITION;
	}

	// message serialization
	if (communicate_with_edp) {
		memcpy(ecp_command.instruction.arm.serialized_command, &ecp_edp_cbuffer, sizeof(ecp_edp_cbuffer));
	}
}

void robot::get_reply()
{

	// message deserialization
	memcpy(&edp_ecp_rbuffer, reply_package.arm.serialized_reply, sizeof(edp_ecp_rbuffer));

	// generator reply generation
	for (int i = 0; i < 6; i++) {
		epos_reply_structure.epos_controller[i].position = edp_ecp_rbuffer.epos_controller[i].position;
		epos_reply_structure.epos_controller[i].motion_in_progress
				= edp_ecp_rbuffer.epos_controller[i].motion_in_progress;
	}

	smb_multi_leg_reply_structure = edp_ecp_rbuffer.multi_leg_reply;

	if (epos_reply_data_request_port.is_new_request()) {
		epos_reply_data_request_port.set(epos_reply_structure);
	}

	if (smb_multi_leg_reply_data_request_port.is_new_request()) {
		smb_multi_leg_reply_data_request_port.set(smb_multi_leg_reply_structure);
	}

}

} // namespace smb
} // namespace ecp
} // namespace mrrocpp

