/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

#include "ui/src/speaker/ui_r_speaker.h"
#include "ui/src/speaker/ui_ecp_r_speaker.h"
#include "robot/speaker/speaker_const.h"
#include "ui/ui_class.h"

/* Local headers */
#include "../ablibs.h"
#include "../abimport.h"
#include "../gcc_ntox86/proto.h"

//
//
// KLASA UiRobotIrp6ot_m
//
//


UiRobotSpeaker::UiRobotSpeaker(Ui& _ui) :
	UiRobot(_ui, EDP_SPEAKER_SECTION, ECP_SPEAKER_SECTION),
			is_wind_speaker_play_open(false), ui_ecp_robot(NULL) {

}

int UiRobotSpeaker::reload_configuration() {

	// jesli speaker ma byc aktywny
	if ((state.is_active = ui.config->value<int> ("is_speaker_active")) == 1) {

		//ui_state.is_any_edp_active = true;
		if (ui.is_mp_and_ecps_active) {
			state.ecp.network_trigger_attach_point
					= ui.config->return_attach_point_name(
							lib::configurator::CONFIG_SERVER,
							"trigger_attach_point", state.ecp.section_name);

			state.ecp.pid = -1;
			state.ecp.trigger_fd = -1;
		}

		switch (state.edp.state) {
		case -1:
		case 0:

			state.edp.pid = -1;
			state.edp.reader_fd = -1;
			state.edp.state = 0;

			if (ui.config->exists(ROBOT_TEST_MODE, state.edp.section_name))
				state.edp.test_mode = ui.config->value<int> (ROBOT_TEST_MODE,
						state.edp.section_name);
			else
				state.edp.test_mode = 0;

			state.edp.hardware_busy_attach_point
					= ui.config->value<std::string> (
							"hardware_busy_attach_point",
							state.edp.section_name);

			state.edp.network_resourceman_attach_point
					= ui.config->return_attach_point_name(
							lib::configurator::CONFIG_SERVER,
							"resourceman_attach_point", state.edp.section_name);

			state.edp.network_reader_attach_point
					= ui.config->return_attach_point_name(
							lib::configurator::CONFIG_SERVER,
							"reader_attach_point", state.edp.section_name);

			state.edp.node_name = ui.config->value<std::string> ("node_name",
					state.edp.section_name);

			state.edp.preset_sound_0 = ui.config->value<std::string> (
					"preset_sound_0", state.edp.section_name);
			state.edp.preset_sound_1 = ui.config->value<std::string> (
					"preset_sound_1", state.edp.section_name);
			state.edp.preset_sound_2 = ui.config->value<std::string> (
					"preset_sound_2", state.edp.section_name);

			break;
		case 1:
		case 2:
			// nie robi nic bo EDP pracuje
			break;
		default:
			break;
		}

	} else // jesli  conveyor ma byc nieaktywny
	{

		switch (state.edp.state) {
		case -1:
		case 0:
			state.edp.state = -1;
			break;
		case 1:
		case 2:
			// nie robi nic bo EDP pracuje
			break;
		default:
			break;
		}
	} // end speaker

	return 1;

}

int UiRobotSpeaker::manage_interface() {

	switch (state.edp.state) {
	case -1:
		ApModifyItemState(&robot_menu, AB_ITEM_DIM, ABN_mm_speaker, NULL);
		break;
	case 0:
		ApModifyItemState(&robot_menu, AB_ITEM_DIM, ABN_mm_speaker_edp_unload,
				ABN_mm_speaker_play, ABN_mm_speaker_preset_sounds, NULL);
		ApModifyItemState(&robot_menu, AB_ITEM_NORMAL, ABN_mm_speaker,
				ABN_mm_speaker_edp_load, NULL);

		break;
	case 1:
	case 2:
		ApModifyItemState(&robot_menu, AB_ITEM_NORMAL, ABN_mm_speaker, NULL);
		//		ApModifyItemState( &all_robots_menu, AB_ITEM_NORMAL, ABN_mm_all_robots_edp_unload, NULL);
		// jesli robot jest zsynchronizowany
		if (state.edp.is_synchronised) {
			// ApModifyItemState( &robot_menu, AB_ITEM_DIM, ABN_mm_speaker_synchronisation, NULL);

			switch (ui.mp.state) {
			case UI_MP_NOT_PERMITED_TO_RUN:
			case UI_MP_PERMITED_TO_RUN:
				ApModifyItemState(&robot_menu, AB_ITEM_NORMAL,
						ABN_mm_speaker_edp_unload, ABN_mm_speaker_play,
						ABN_mm_speaker_preset_sounds, NULL);
				ApModifyItemState(&robot_menu, AB_ITEM_DIM,
						ABN_mm_speaker_edp_load, NULL);
				break;
			case UI_MP_WAITING_FOR_START_PULSE:
				ApModifyItemState(&robot_menu, AB_ITEM_NORMAL,
						ABN_mm_speaker_play, ABN_mm_speaker_preset_sounds, NULL);
				ApModifyItemState(&robot_menu, AB_ITEM_DIM,
						ABN_mm_speaker_edp_unload, ABN_mm_speaker_edp_load,
						NULL);
				break;
			case UI_MP_TASK_RUNNING:
			case UI_MP_TASK_PAUSED:
				ApModifyItemState(&robot_menu, AB_ITEM_DIM, // modyfikacja menu - ruchy reczne zakazane
						ABN_mm_speaker_play, ABN_mm_speaker_preset_sounds, NULL);
				break;
			default:
				break;
			}
		} else // jesli robot jest niezsynchronizowany
		{
			/*
			 ApModifyItemState( &robot_menu, AB_ITEM_NORMAL, ABN_mm_speaker_edp_unload, NULL);
			 ApModifyItemState( &robot_menu, AB_ITEM_DIM, ABN_mm_speaker_edp_load, NULL);
			 */
		}
		break;
	default:
		break;
	}

	return 1;

}

int UiRobotSpeaker::close_all_windows() {

	int pt_res = PtEnter(0);

	close_wnd_speaker_play(NULL, NULL, NULL);

	if (pt_res >= 0) {
		PtLeave(0);
	}
	return 1;


}

int UiRobotSpeaker::delete_ui_ecp_robot() {
	delete ui_ecp_robot;
	return 1;
}
