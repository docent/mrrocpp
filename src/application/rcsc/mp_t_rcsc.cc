#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <list>
#include <map>

#include "lib/typedefs.h"
#include "lib/impconst.h"
#include "lib/com_buf.h"

#include "lib/srlib.h"
#include "base/mp/mp.h"
#include "base/mp/mp_g_common.h"
#include "mp_t_rcsc.h"
#include "ecp_mp_tr_rc_windows.h"
#include "robot/festival/ecp_g_festival.h"
#include "robot/festival/ecp_mp_t_festival.h"
#include "robot/irp6ot_m/irp6ot_m_const.h"
#include "robot/irp6p_m/irp6p_m_const.h"
#include "subtask/ecp_mp_st_bias_edp_force.h"
#include "subtask/ecp_mp_st_tff_nose_run.h"
#include "generator/ecp/ecp_mp_g_smooth.h"
#include "generator/ecp/ecp_mp_g_force.h"
#include "application/servovision/ecp_mp_g_simple_visual_servo_manager.h"

#include <boost/foreach.hpp>

namespace mrrocpp {
namespace mp {
namespace task {

void rubik_cube_solver::initiate(common::CUBE_COLOR up_is, common::CUBE_COLOR down_is, common::CUBE_COLOR front_is, common::CUBE_COLOR rear_is, common::CUBE_COLOR left_is, common::CUBE_COLOR right_is)
{
	cube_state = new common::CubeState(up_is, down_is, front_is, rear_is, left_is, right_is);

	manipulation_sequence_computed = false;
}

rubik_cube_solver::rubik_cube_solver(lib::configurator &_config) :
	task(_config), cube_state(NULL)
{
	// Powolanie czujnikow

	//odczyt z configa
	vis_servoing = config.value <int> ("vis_servoing");

	if (vis_servoing) {
		BOOST_FOREACH(ecp_mp::sensor_item_t & s, sensor_m)
{		s.second->configure_sensor();
	}
}

if (vis_servoing)
{
	// dodanie transmitter'a
	transmitter_m[ecp_mp::transmitter::TRANSMITTER_RC_WINDOWS] =
	new ecp_mp::transmitter::rc_windows (ecp_mp::transmitter::TRANSMITTER_RC_WINDOWS, "[transmitter_rc_windows]", *this);

}
}

rubik_cube_solver::~rubik_cube_solver()
{
if(cube_state)
delete cube_state;
}

void rubik_cube_solver::identify_colors() //DO WIZJI (przekladanie i ogladanie scian)
{
//sekwencja poczatkowa w kolejnosci: UP, DOWN, FRONT, BACK, LEFT, RIGHT
//cube_initial_state=BGROWY

// manianka
cube_state->set_state(common::BLUE, common::GREEN, common::RED, common::ORANGE, common::WHITE, common::YELLOW);

const common::CUBE_TURN_ANGLE changing_order[] = {
	common::CL_0, common::CL_0, common::CL_180, common::CL_0, common::CL_180, common::CL_0
};

for(int k=0; k<6; k++)
{
	face_turn_op(common::CL_0);

	set_next_ecps_state (ecp_mp::task::ECP_GEN_FESTIVAL, 0, "oglo~dam kolory na s~ciance", 0, 1, lib::ROBOT_FESTIVAL.c_str());

	// uruchomienie generatora empty_gen i oczekiwanie na zakonczenie obydwu generatorow ECP
	run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
	(1, 1, lib::ROBOT_FESTIVAL.c_str(), lib::ROBOT_FESTIVAL.c_str());

	if (vis_servoing)
	{
		ecp_mp::sensor::sensor<lib::cube_face_t> * cube_recognition = dynamic_cast<ecp_mp::sensor::sensor<lib::cube_face_t> *> (sensor_m[lib::SENSOR_CAMERA_ON_TRACK]);

		wait_ms(5000);
		cube_recognition->initiate_reading();
		wait_ms(1000);
		cube_recognition->get_reading();

		for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
		cube_state->cube_tab[k][3*i+j]=cube_recognition->image.colors[3*i+j];
	}

	printf("\nFACE FACE %d:\n",k);
	flushall();

	for(int i=0; i<9; i++)
	{
		switch (cube_state->cube_tab[k][i])
		{
			case 1:
			cube_state->cube_tab[k][i]='r';
			printf("R");
			break;
			case 2:
			cube_state->cube_tab[k][i]='o';
			printf("O");
			break;
			case 3:
			cube_state->cube_tab[k][i]='y';
			printf("Y");
			break;
			case 4:
			cube_state->cube_tab[k][i]='g';
			printf("G");
			break;
			case 5:
			cube_state->cube_tab[k][i]='b';
			printf("B");
			break;
			case 6:
			cube_state->cube_tab[k][i]='w';
			printf("W");
			break;
			default:
			cube_state->cube_tab[k][i]='o';
			printf("?");
			break;
		}
		if(cube_state->cube_tab[k][i] != 'o') {
			putchar(std::toupper(cube_state->cube_tab[k][i]));
		} else {
			putchar('?');
		}
	}
	printf("\n");

	wait_ms(1000);
	face_change_op(changing_order[k]);

} //for
}

bool rubik_cube_solver::communicate_with_windows_solver()
{
char cube_tab_send[55];
char manipulation_sequence[200];

for(int i=0; i<3; i++)
for(int j=0; j<3; j++)
cube_tab_send[2*9+3*i+j]=cube_state->cube_tab[0][3*i+j]; //rot cl 0

for(int i=0; i<3; i++)
for(int j=0; j<3; j++)
cube_tab_send[1*9+3*j+2-i]=cube_state->cube_tab[1][3*i+j]; //rot cl 90

for(int i=0; i<3; i++)
for(int j=0; j<3; j++)
cube_tab_send[3*9+3*(2-j)+i]=cube_state->cube_tab[2][3*i+j]; //rot ccl 90

for(int i=0; i<3; i++)
for(int j=0; j<3; j++)
cube_tab_send[5*9+3*i+j]=cube_state->cube_tab[3][3*i+j]; //rot cl 0

for(int i=0; i<3; i++)
for(int j=0; j<3; j++)
cube_tab_send[4*9+3*j+2-i]=cube_state->cube_tab[4][3*i+j]; //rot cl 90

for(int i=0; i<3; i++)
for(int j=0; j<3; j++)
cube_tab_send[0*9+3*j+2-i]=cube_state->cube_tab[5][3*i+j]; //rot cl 90

printf("SEQ IN COLOR : %s\n",cube_tab_send);

char c_up=cube_tab_send[4];
char c_right=cube_tab_send[13];
char c_front=cube_tab_send[22];
char c_down=cube_tab_send[31];
char c_left=cube_tab_send[40];
char c_back=cube_tab_send[49];

printf("%c %c %c %c %c %c\n", c_up, c_right, c_front, c_down, c_left, c_back);

for(int i=0; i<54; i++)
{
	if (cube_tab_send[i] == c_up)
	cube_tab_send[i]='u';
	else if (cube_tab_send[i] == c_down)
	cube_tab_send[i]='d';
	else if (cube_tab_send[i] == c_front)
	cube_tab_send[i]='f';
	else if (cube_tab_send[i] == c_back)
	cube_tab_send[i]='b';
	else if (cube_tab_send[i] == c_right)
	cube_tab_send[i]='r';
	else if (cube_tab_send[i] == c_left)
	cube_tab_send[i]='l';
}

/*
 for(int i=0; i<54; i++)
 {
 switch (cube_tab_send[i])
 {
 case 'b': cube_tab_send[i]='u'; break;
 case 'g': cube_tab_send[i]='d'; break;
 case 'o': cube_tab_send[i]='f'; break;
 case 'r': cube_tab_send[i]='b'; break;
 case 'y': cube_tab_send[i]='r'; break;
 case 'w': cube_tab_send[i]='l'; break;
 }
 }
 */

cube_tab_send[54]='\0';

printf("SEQ FROM VIS : %s\n",cube_tab_send);

//reszta
// struktura pomiocnicza
common::SingleManipulation single_manipulation;

// czyszczenie listy
manipulation_list.clear();

ecp_mp::transmitter::transmitter_base * transmitter_ptr = transmitter_m[ecp_mp::transmitter::TRANSMITTER_RC_WINDOWS];
assert(transmitter_ptr);

ecp_mp::transmitter::rc_windows * rc_solver_ptr = dynamic_cast<ecp_mp::transmitter::rc_windows *> (transmitter_ptr);
assert(rc_solver_ptr);

ecp_mp::transmitter::rc_windows & rc_solver = *rc_solver_ptr;

for(int i=0; i<54; i++)
{
	rc_solver.to_va.rc_state[i]=cube_tab_send[i];
}
rc_solver.to_va.rc_state[54]='\0';

set_next_ecps_state (ecp_mp::task::ECP_GEN_FESTIVAL, 0, "mys~le~", 0, 1, lib::ROBOT_FESTIVAL.c_str());

// uruchomienie generatora empty_gen i oczekiwanie na zakonczenie obydwu generatorow ECP
run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(1, 1, lib::ROBOT_FESTIVAL.c_str(), lib::ROBOT_FESTIVAL.c_str());

rc_solver.t_write();

rc_solver.t_read(true);

printf ("OPS: %s", rc_solver.from_va.sequence);

strcpy (manipulation_sequence, rc_solver.from_va.sequence);

if ((manipulation_sequence[0]=='C') && (manipulation_sequence[1]=='u') && (manipulation_sequence[2]=='b') && (manipulation_sequence[3]=='e'))
{
	printf("Jam jest daltonista. ktory Ci nie uloz*y kostki\n");
	manipulation_sequence_computed = false;
	return false;
}

//sekwencja poczatkowa w kolejnosci: UP, DOWN, FRONT, BACK, LEFT, RIGHT
//cube_initial_state=BGROWY


int s=0;
int str_size=0;
for (unsigned int char_i=0; char_i < strlen(rc_solver.from_va.sequence)-1; char_i ++)
{
	if (s==0)
	{
		switch (rc_solver.from_va.sequence[char_i])
		{
			case 'U':
			manipulation_sequence[str_size] = 'B';
			break;
			case 'D':
			manipulation_sequence[str_size] = 'G';
			break;
			case 'F':
			manipulation_sequence[str_size] = 'O';
			break;
			case 'B':
			manipulation_sequence[str_size] = 'R';
			break;
			case 'L':
			manipulation_sequence[str_size] = 'W';
			break;
			case 'R':
			manipulation_sequence[str_size] = 'Y';
			break;
		}
		s=1;
		str_size++;
	}
	else if (s==1)
	{
		switch (rc_solver.from_va.sequence[char_i])
		{
			case ' ':
			manipulation_sequence[str_size] = '1';
			s=0;
			break;
			case '2':
			manipulation_sequence[str_size] = '2';
			s=2;
			break;
			case '\'':
			manipulation_sequence[str_size] = '3';
			s=2;
			break;
		}
		str_size++;
	}
	else if (s==2)
	{
		s=0;
	}

}

if (s==1)
{
	str_size--;
	manipulation_sequence[str_size] = '1';
	str_size++;
}
manipulation_sequence[str_size]='\0';

printf ("\n%d %d\n", str_size, strlen(manipulation_sequence));
printf ("SEQ from win %s\n", rc_solver.from_va.sequence);
printf ("\nSEQ2 %s\n", manipulation_sequence);

//pocztaek ukladania
// dodawanie manipulacji do listy
for (unsigned int char_i=0; char_i < strlen(manipulation_sequence)-1; char_i += 2)
{
	single_manipulation.set_state(common::read_cube_color(manipulation_sequence[char_i]),
			common::read_cube_turn_angle(manipulation_sequence[char_i+1]));
	manipulation_list.push_back(single_manipulation);
}

set_next_ecps_state (ecp_mp::task::ECP_GEN_FESTIVAL, 0, "juZ ukl/adam", 0, 1, lib::ROBOT_FESTIVAL.c_str());

// uruchomienie generatora empty_gen i oczekiwanie na zakonczenie obydwu generatorow ECP
run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(1, 1, lib::ROBOT_FESTIVAL.c_str(), lib::ROBOT_FESTIVAL.c_str());

manipulation_sequence_computed = true;

return false;
}

void rubik_cube_solver::execute_manipulation_sequence()
{
for(std::list<common::SingleManipulation>::iterator manipulation_list_iterator = manipulation_list.begin();
		manipulation_list_iterator != manipulation_list.end(); manipulation_list_iterator++)
{
	manipulate(manipulation_list_iterator->face_to_turn, manipulation_list_iterator->turn_angle);
}
}

void rubik_cube_solver::manipulate(common::CUBE_COLOR face_to_turn, common::CUBE_TURN_ANGLE turn_angle )
{

if (face_to_turn == cube_state->up)
{
	// printf("cube_state->up\n");
	face_change_op(common::CL_90);
	face_turn_op(turn_angle);
}
else if (face_to_turn == cube_state->down)
{
	// printf("cube_state->down\n");
	face_change_op(common::CCL_90);
	face_turn_op(turn_angle);

}
else if (face_to_turn == cube_state->front)
{
	// printf("cube_state->front\n");
	face_change_op(common::CL_0);
	face_turn_op(common::CL_0);
	face_change_op(common::CL_90);
	face_turn_op(turn_angle);
}
else if (face_to_turn == cube_state->rear)
{
	// printf("cube_state->rear\n");
	face_change_op(common::CL_0);
	face_turn_op(common::CL_0);
	face_change_op(common::CCL_90);
	face_turn_op(turn_angle);
}
else if (face_to_turn == cube_state->left)
{
	// printf("cube_state->left\n");
	face_change_op(common::CL_0);
	face_turn_op(turn_angle);
}
else if (face_to_turn == cube_state->right)
{
	// printf("cube_state->right\n");
	face_change_op(common::CL_180);
	face_turn_op(turn_angle);
}
}

// obrot sciany
void rubik_cube_solver::face_turn_op(common::CUBE_TURN_ANGLE turn_angle)
{

// ustawienie chwytakow we wlasciwej wzajemnej orientacji

//	printf("face_turn_op_CL: %d\n", turn_angle);

// wlaczenie generatora uczacego w obu robotach
switch (turn_angle)
{
	case common::CL_90:
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fturn_ap_cl_90_phase_1.trj", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6p_sm_fturn_ap_cl_90_phase_1.trj", 0,1, lib::ROBOT_IRP6P_M.c_str());
	break;
	case common::CL_0:
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fturn_ap_cl_0_phase_1.trj", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6p_sm_fturn_ap_cl_0_phase_1.trj", 0,1, lib::ROBOT_IRP6P_M.c_str());
	break;
	case common::CCL_90:
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fturn_ap_ccl_90_phase_1.trj", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6p_sm_fturn_ap_ccl_90_phase_1.trj", 0,1, lib::ROBOT_IRP6P_M.c_str());
	break;
	case common::CL_180:
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fturn_ap_cl_180_phase_1.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6p_sm_fturn_ap_cl_180_phase_1.trj", 0,1, lib::ROBOT_IRP6P_M.c_str());
	break;
	default:
	break;
}

// uruchomienie generatora empty_gen i oczekiwanie na zakonczenie obydwu generatorow ECP
run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(3, 2, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str(), lib::ROBOT_FESTIVAL.c_str(),
		lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str());

// zblizenie chwytaka tracka do nieruchomego chwytaka postumenta

switch (turn_angle)
{
	case common::CL_90:
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fturn_ap_cl_90_phase_2.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());
	break;
	case common::CL_0:
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fturn_ap_cl_0_phase_2.trj", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
	break;
	case common::CCL_90:
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fturn_ap_ccl_90_phase_2.trj", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
	break;
	case common::CL_180:
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fturn_ap_cl_180_phase_2.trj", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
	break;
	default:
	break;
}

// uruchomienie generatora empty_gen i oczekiwanie na zakonczenie obydwu generatorow ECP
run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(2, 1, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_FESTIVAL.c_str(),
		lib::ROBOT_IRP6OT_M.c_str());

// zacisniecie postumenta na kostce

// wlaczenie generatora do konfiguracji czujnika w EDP w obydwu robotach
configure_edp_force_sensor(true, true);

// wlaczenie generatora zacisku na kostce w robocie irp6p
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_TFF_RUBIK_GRAB, (int) ecp_mp::task::RCSC_RG_FACE_TURN_PHASE_0, "", 0, 1, lib::ROBOT_IRP6P_M.c_str());
// uruchomienie generatora empty_gen
run_extended_empty_gen (false, 1, lib::ROBOT_IRP6P_M.c_str());
// wlaczenie generatora zacisku na kostce w robocie irp6p
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_TFF_RUBIK_GRAB, (int) ecp_mp::task::RCSC_RG_FROM_OPEARTOR_PHASE_1, "", 0,1, lib::ROBOT_IRP6P_M.c_str());
// uruchomienie generatora empty_gen
run_extended_empty_gen (false, 1, lib::ROBOT_IRP6P_M.c_str());
// wlaczenie generatora zacisku na kostce w robocie irp6p
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_TFF_RUBIK_GRAB, (int) ecp_mp::task::RCSC_RG_FROM_OPEARTOR_PHASE_2, "", 0,1, lib::ROBOT_IRP6P_M.c_str());
// uruchomienie generatora empty_gen
run_extended_empty_gen (false, 1, lib::ROBOT_IRP6P_M.c_str());

// obrot kostki
switch (turn_angle)
{
	case common::CL_90:
	set_next_ecps_state (ecp_mp::task::ECP_GEN_FESTIVAL, 0, "obracam kostke~", 0, 1, lib::ROBOT_FESTIVAL.c_str());
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_TFF_RUBIK_FACE_ROTATE, (int) ecp_mp::task::RCSC_CL_90, "", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
	// uruchomienie generatora empty_gen
	run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
	(2, 2, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_FESTIVAL.c_str(), lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_FESTIVAL.c_str());
	break;
	case common::CL_0:
	break;
	case common::CCL_90:
	set_next_ecps_state (ecp_mp::task::ECP_GEN_FESTIVAL, 0, "obracam kostke~", 0, 1, lib::ROBOT_FESTIVAL.c_str());
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_TFF_RUBIK_FACE_ROTATE, (int) ecp_mp::task::RCSC_CCL_90, "", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
	// uruchomienie generatora empty_gen
	run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
	(2, 2, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_FESTIVAL.c_str(), lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_FESTIVAL.c_str());
	break;
	case common::CL_180:
	set_next_ecps_state (ecp_mp::task::ECP_GEN_FESTIVAL, 0, "obracam kostke~", 0,1, lib::ROBOT_FESTIVAL.c_str());
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_TFF_RUBIK_FACE_ROTATE, (int) ecp_mp::task::RCSC_CL_180, "", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
	// uruchomienie generatora empty_gen
	run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
	(2, 2, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_FESTIVAL.c_str(), lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_FESTIVAL.c_str());
	break;
	default:
	break;
}

// rozwarcie chwytaka tracka

// wlaczenie generatora zacisku na kostce w robocie irp6ot
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::RELATIVE, "src/application/rcsc/trj/gripper_opening2.trj", 0, 1, lib::ROBOT_IRP6OT_M.c_str());

// uruchomienie generatora empty_gen
run_extended_empty_gen (false, 1, lib::ROBOT_IRP6OT_M.c_str());

// odejscie tracka od postumenta
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fturn_de.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());
// uruchomienie generatora empty_gen
run_extended_empty_gen (false, 1, lib::ROBOT_IRP6OT_M.c_str());
}

// zmiana sciany (przelozenie kostki)
void rubik_cube_solver::face_change_op(common::CUBE_TURN_ANGLE turn_angle)
{

// zblizenie chwytakow

//	printf("face_turn_op_CL: %d\n", turn_angle);

set_next_ecps_state (ecp_mp::task::ECP_GEN_FESTIVAL, 0, "przekl/adam kostke~", 0, 1, lib::ROBOT_FESTIVAL.c_str());

// uruchomienie generatora empty_gen i oczekiwanie na zakonczenie obydwu generatorow ECP

// wlaczenie generatora uczacego w obu robotach
switch (turn_angle)
{
	case common::CL_90:

	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fchange_ap_cl_90_phase_1.trj", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6p_sm_fchange_ap_cl_90_phase_1.trj", 0,1, lib::ROBOT_IRP6P_M.c_str());
	break;
	case common::CL_0:

	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fchange_ap_cl_0_phase_1.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6p_sm_fchange_ap_cl_0_phase_1.trj", 0,1, lib::ROBOT_IRP6P_M.c_str());
	break;
	case common::CCL_90:

	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fchange_ap_ccl_90_phase_1.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6p_sm_fchange_ap_ccl_90_phase_1.trj", 0, 1, lib::ROBOT_IRP6P_M.c_str());
	break;
	case common::CL_180:

	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fchange_ap_cl_180_phase_1.trj", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6p_sm_fchange_ap_cl_180_phase_1.trj", 0, 1, lib::ROBOT_IRP6P_M.c_str());
	break;
	default:
	break;
}

// uruchomienie generatora empty_gen i oczekiwanie na zakonczenie obydwu generatorow ECP
run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(3, 2, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str(), lib::ROBOT_FESTIVAL.c_str(),
		lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str());

switch (turn_angle)
{
	case common::CL_90:
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fchange_ap_cl_90_phase_2.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());
	break;
	case common::CL_0:
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fchange_ap_cl_0_phase_2.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());
	break;
	case common::CCL_90:
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fchange_ap_ccl_90_phase_2.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());
	break;
	case common::CL_180:
	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fchange_ap_cl_180_phase_2.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());
	break;
	default:
	break;
}
// TODO: wstawic generator VS + zblizanie silowe

// uruchomienie generatora empty_gen i oczekiwanie na zakonczenie obydwu generatorow ECP
run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(2, 1, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_FESTIVAL.c_str(),
		lib::ROBOT_IRP6OT_M.c_str());

// zacisniecie tracka na kostce

// wlaczenie generatora do konfiguracji czujnika w EDP w obydwu robotach
configure_edp_force_sensor(true, true);

// wlaczenie generatora zacisku na kostce w robocie irp6ot
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_TFF_RUBIK_GRAB, (int) ecp_mp::task::RCSC_RG_FCHANGE_PHASE_1, "", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
// uruchomienie generatora empty_gen
run_extended_empty_gen (false, 1, lib::ROBOT_IRP6OT_M.c_str());
// wlaczenie generatora zacisku na kostce w robocie irp6ot
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_TFF_RUBIK_GRAB, (int) ecp_mp::task::RCSC_RG_FCHANGE_PHASE_2, "", 0,1, lib::ROBOT_IRP6OT_M.c_str());
// uruchomienie generatora empty_gen
run_extended_empty_gen (false, 1, lib::ROBOT_IRP6OT_M.c_str());

// docisniecie chwytaka tracka do kostki

set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_TFF_GRIPPER_APPROACH, (int) 0, "", 0,1, lib::ROBOT_IRP6OT_M.c_str());
// uruchomienie generatora empty_gen
run_extended_empty_gen (false, 1, lib::ROBOT_IRP6OT_M.c_str());

// zacisniecie tracka na kostce

// wlaczenie generatora zacisku na kostce w robocie irp6ot
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_TFF_RUBIK_GRAB, (int) ecp_mp::task::RCSC_RG_FCHANGE_PHASE_3, "", 0,1, lib::ROBOT_IRP6OT_M.c_str());
// uruchomienie generatora empty_gen
run_extended_empty_gen (false, 1, lib::ROBOT_IRP6OT_M.c_str());

// wstepne rozwarcie chwytaka postumenta
//set_next_ecps_state ((int) ecp_mp::task::ECP_ST_GRIPPER_OPENING, (int) ecp_mp::task::RCSC_GO_VAR_1, "",  0,1, lib::ROBOT_IRP6P_M.c_str());
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::RELATIVE, "src/application/rcsc/trj/gripper_opening.trj", 0,1, lib::ROBOT_IRP6P_M.c_str());
// uruchomienie generatora empty_gen
run_extended_empty_gen (false, 1, lib::ROBOT_IRP6P_M.c_str());

// ostateczne zacisniecie tracka na kostce
// wlaczenie generatora zacisku na kostce w robocie irp6ot
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_TFF_RUBIK_GRAB, (int) ecp_mp::task::RCSC_RG_FCHANGE_PHASE_4, "", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
// uruchomienie generatora empty_gen
run_extended_empty_gen (false, 1, lib::ROBOT_IRP6OT_M.c_str());

// dalsze rozwarcie chwytaka postumenta
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::RELATIVE, "src/application/rcsc/trj/gripper_opening2.trj", 0,1, lib::ROBOT_IRP6P_M.c_str());
// uruchomienie generatora empty_gen
run_extended_empty_gen (false, 1, lib::ROBOT_IRP6P_M.c_str());

// odejscie tracka od postumenta
switch (turn_angle)
{
	case common::CL_90:

	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fchange_de_cl_90.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());
	break;
	case common::CL_0:

	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fchange_de_cl_0.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());
	break;
	case common::CCL_90:

	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fchange_de_ccl_90.trj", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
	break;
	case common::CL_180:

	set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_fchange_de_cl_180.trj", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
	break;
	default:
	break;
}

// uruchomienie generatora empty_gen i oczekiwanie na zakonczenie obydwu generatorow ECP
run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(2, 1, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_FESTIVAL.c_str(), lib::ROBOT_IRP6OT_M.c_str());

// zmiana stanu kostki

common::CubeState tmp_cube_state;

switch (turn_angle)
{
	case common::CL_90:
	tmp_cube_state.set_state(cube_state->left, cube_state->right, cube_state->up, cube_state->down,
			cube_state->front, cube_state->rear);
	break;
	case common::CL_0:
	tmp_cube_state.set_state(cube_state->front, cube_state->rear, cube_state->left, cube_state->right,
			cube_state->up, cube_state->down);
	break;
	case common::CCL_90:
	tmp_cube_state.set_state(cube_state->right, cube_state->left, cube_state->down, cube_state->up,
			cube_state->front, cube_state->rear);
	break;
	case common::CL_180:
	tmp_cube_state.set_state(cube_state->front, cube_state->rear, cube_state->right, cube_state->left,
			cube_state->down, cube_state->up);
	break;
	default:
	break;
}

*cube_state = tmp_cube_state;

//	cube_state->print_cube_colors();

}

void rubik_cube_solver::configure_edp_force_sensor(bool configure_track, bool configure_postument)
{
if (configure_track)
{
	set_next_ecps_state (ecp_mp::task::ECP_ST_BIAS_EDP_FORCE, 0, "", 0,1, lib::ROBOT_IRP6OT_M.c_str());
}

if (configure_postument)
{
	set_next_ecps_state (ecp_mp::task::ECP_ST_BIAS_EDP_FORCE, 0, "", 0,1, lib::ROBOT_IRP6P_M.c_str());
}

if ((configure_track)&&(!configure_postument))
{
	run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
	(1, 1, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6OT_M.c_str());
}
else if ((!configure_track)&&(configure_postument))
{
	run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
	(1, 1, lib::ROBOT_IRP6P_M.c_str(), lib::ROBOT_IRP6P_M.c_str());
}
else if ((configure_track)&&(configure_postument))
{
	run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
	(2, 2, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str(), lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str() );
}
}

// dojscie
void rubik_cube_solver::approach_op(int mode)
{

//pierwsza konfiguracja czujnikow
//wlaczenie generatora do konfiguracji czujnika w EDP w obydwu robotach
configure_edp_force_sensor(true, true);

set_next_ecps_state (ecp_mp::task::ECP_GEN_FESTIVAL, ecp::festival::generator::generator::POLISH_VOICE, "jestem podatny", 0,1, lib::ROBOT_FESTIVAL.c_str());

// uruchomienie generatora empty_gen i oczekiwanie na zakonczenie obydwu generatorow ECP
run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(1, 1, lib::ROBOT_FESTIVAL.c_str(), lib::ROBOT_FESTIVAL.c_str());

if ((config.exists("irp6p_compliant")) && ((bool) config.value<int>("irp6p_compliant")))
{
	// wlaczenie genrator tff_nose_run_generator w postumencie
	set_next_ecps_state (ecp_mp::task::ECP_ST_TFF_NOSE_RUN, (int) 0, "", 0,1, lib::ROBOT_IRP6P_M.c_str());

	// uruchomienie generatora empty_gen
	run_extended_empty_gen (true, 1, lib::ROBOT_IRP6P_M.c_str());

	// przerwanie pracy generatora w ECP
	send_end_motion_to_ecps (1, lib::ROBOT_IRP6P_M.c_str());
}
else
{
	// wlaczenie genrator tff_nose_run_generator w tracku
	set_next_ecps_state (ecp_mp::task::ECP_ST_TFF_NOSE_RUN, (int) 0, "", 0,1, lib::ROBOT_IRP6OT_M.c_str());

	// uruchomienie generatora empty_gen
	run_extended_empty_gen (true, 1, lib::ROBOT_IRP6OT_M.c_str());

	// przerwanie pracy generatora w ECP
	send_end_motion_to_ecps (1, lib::ROBOT_IRP6OT_M.c_str());
}

set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_ap_1.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6p_sm_ap_1.trj", 0,1, lib::ROBOT_IRP6P_M.c_str());

run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(3, 2, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str(), lib::ROBOT_FESTIVAL.c_str(),
		lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str());

set_next_ecps_state (ecp_mp::task::ECP_GEN_FESTIVAL, 0, "jestem robotem usl/ugowym", 0, 1, lib::ROBOT_FESTIVAL.c_str());

//zadanie chwytania kostki
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_ap_2.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());

run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(3, 2, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str(), lib::ROBOT_FESTIVAL.c_str(),
		lib::ROBOT_FESTIVAL.c_str(), lib::ROBOT_IRP6OT_M.c_str());

//generator sledzacy kostke
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_IB_EIH, (int) 1, "", 0,1, lib::ROBOT_IRP6OT_M.c_str());

run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(3, 1, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str(), lib::ROBOT_FESTIVAL.c_str(),
		lib::ROBOT_IRP6OT_M.c_str());

// docisniecie chwytaka tracka do kostki
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_TFF_GRIPPER_APPROACH, (int) 0, "", 0,1, lib::ROBOT_IRP6OT_M.c_str());

run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(3, 1, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str(), lib::ROBOT_FESTIVAL.c_str(),
		lib::ROBOT_IRP6OT_M.c_str());

//podnoszenie o 2 milimetry nad kostkę
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::RELATIVE, "src/application/rcsc/trj/irp6ot_sm_raising_up.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());

run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(3, 1, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str(), lib::ROBOT_FESTIVAL.c_str(),
		lib::ROBOT_IRP6OT_M.c_str());

//zaciskanie na kostce
set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::RELATIVE, "src/application/rcsc/trj/irp6ot_sm_gripper_closing.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());

run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(3, 1, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str(), lib::ROBOT_FESTIVAL.c_str(),
		lib::ROBOT_IRP6OT_M.c_str());

//zadanie chwytania kostki (koniec)

set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_ap_3.trj", 0, 1, lib::ROBOT_IRP6OT_M.c_str());
// uruchomienie generatora empty_gen i oczekiwanie na zakonczenie obydwu generatorow ECP

run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(3, 2, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str(), lib::ROBOT_FESTIVAL.c_str(),
		lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str());
}

// odejscie
void rubik_cube_solver::departure_op()
{
set_next_ecps_state (ecp_mp::task::ECP_GEN_FESTIVAL, 0, "skon~czyl/em", 0, 1, lib::ROBOT_FESTIVAL.c_str());

set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6ot_sm_de_1.trj", 0,1, lib::ROBOT_IRP6OT_M.c_str());

set_next_ecps_state (ecp_mp::common::generator::ECP_GEN_SMOOTH, (int) ecp_mp::task::ABSOLUTE, "src/application/rcsc/trj/irp6p_sm_de_1.trj", 0,1, lib::ROBOT_IRP6P_M.c_str());

run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(3, 1, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str(), lib::ROBOT_FESTIVAL.c_str(), lib::ROBOT_FESTIVAL.c_str());

set_next_ecps_state (ecp_mp::task::ECP_GEN_FESTIVAL, 0, "jade~ pracowac~ do anglii", 0, 1, lib::ROBOT_FESTIVAL.c_str());

// uruchomienie generatora empty_gen i oczekiwanie na zakonczenie obydwu generatorow ECP
run_extended_empty_generator_for_set_of_robots_and_wait_for_task_termination_message_of_another_set_of_robots
(3, 2, lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str(), lib::ROBOT_FESTIVAL.c_str(),
		lib::ROBOT_IRP6OT_M.c_str(), lib::ROBOT_IRP6P_M.c_str());
}

void rubik_cube_solver::main_task_algorithm(void)
{
std::string cube_initial_state_string = config.value<std::string>("cube_initial_state");

const char * cube_initial_state = cube_initial_state_string.c_str();

//	enum common::CUBE_COLOR {UKNOWN, common::RED, common::YELLOW, common::GREEN, common::BLUE, common::ORANGE, common::WHITE};
//	 cube_state::set_state(common::CUBE_COLOR up_is, common::CUBE_COLOR down_is, common::CUBE_COLOR front_is,
//		common::CUBE_COLOR rear_is, common::CUBE_COLOR left_is, common::CUBE_COLOR right_is)

initiate (common::read_cube_color(cube_initial_state[0]),
		common::read_cube_color(cube_initial_state[1]), common::read_cube_color(cube_initial_state[2]), common::read_cube_color(cube_initial_state[3]),
		common::read_cube_color(cube_initial_state[4]), common::read_cube_color(cube_initial_state[5]));

// Zlecenie wykonania kolejnego makrokroku
// printf("po start all \n");
for(;;)
{
	sr_ecp_msg->message("Nowa seria");

	if (vis_servoing)
	{
		//printf("if vis servoing\n");
		flushall();
		BOOST_FOREACH(ecp_mp::sensor_item_t & sensor_item, sensor_m) {
			sensor_item.second->configure_sensor();
		}
	}

	// przechwycenie kostki
	approach_op(vis_servoing);

	// IDENTIFY COLORS
	identify_colors();

	if (vis_servoing)
	{
		//printf("if vis servoing 2\n");
		flushall();
		if (communicate_with_windows_solver())
		{
			break;
		}
	}

	if ((vis_servoing)&&(manipulation_sequence_computed))
	{
		//printf("trzeci if\n");
		flushall();

		// wykonanie sekwencji manipulacji
		face_turn_op(common::CL_0);

		execute_manipulation_sequence();

		// zakonczenie zadania
		face_change_op(common::CL_0);
	}

	departure_op();

	break;

} // koniec: for(;;) - zewnetrzna petla
}

task* return_created_mp_task (lib::configurator &_config)
{
return new rubik_cube_solver(_config);
}

} // namespace task
} // namespace mp
} // namespace mrrocpp

