/* --------------------------------------------------------------------- */
/*                          SERVO_GROUP Process                          */
// ostatnia modyfikacja - styczen 2005
/* --------------------------------------------------------------------- */

#include <unistd.h>

#include "lib/typedefs.h"
#include "lib/impconst.h"
#include "lib/com_buf.h"

// Klasa edp_irp6p_effector.
#include "robot/irp6p_m/edp_irp6p_m_effector.h"
// Klasa hardware_interface.
#include "robot/irp6p_m/hi_irp6p_m.h"
// Klasa servo_buffer.
#include "robot/irp6p_m/sg_irp6p_m.h"
#include "robot/irp6p_m/regulator_irp6p_m.h"

namespace mrrocpp {
namespace edp {
namespace irp6p_m {

// extern uint64_t kk;				  // numer pomiaru od momentu startu pomiarow


/*-----------------------------------------------------------------------*/
servo_buffer::servo_buffer(effector &_master) :
	common::servo_buffer(_master), master(_master) {
	for (int j = 0; j < master.number_of_servos; j++) {
		synchro_axis_order[j] = ((j + IRP6P_SYN_INIT_AXE)
				% (master.number_of_servos));
		switch (j) {
		case IRP6P_GRIPPER_TURN_AXE:
			axe_inc_per_revolution[j]
					= IRP6_POSTUMENT_AXIS_6_INC_PER_REVOLUTION;
			synchro_step_coarse[j] = IRP6_POSTUMENT_AXIS_6_SYNCHRO_STEP_COARSE;
			synchro_step_fine[j] = IRP6_POSTUMENT_AXIS_6_SYNCHRO_STEP_FINE;
			break;
		default:
			axe_inc_per_revolution[j]
					= IRP6_POSTUMENT_AXIS_0_TO_5_INC_PER_REVOLUTION;
			synchro_step_coarse[j]
					= IRP6_POSTUMENT_AXIS_0_TO_5_SYNCHRO_STEP_COARSE;
			synchro_step_fine[j] = IRP6_POSTUMENT_AXIS_0_TO_5_SYNCHRO_STEP_FINE;
			break;
		}
	}

	thread_id = new boost::thread(boost::bind(&servo_buffer::operator(), this));
}
/*-----------------------------------------------------------------------*/

void servo_buffer::load_hardware_interface(void) {

	// tablica pradow maksymalnych dla poszczegolnych osi
	int max_current[IRP6P_M_NUM_OF_SERVOS] = {
			IRP6_POSTUMENT_AXIS_1_MAX_CURRENT,
			IRP6_POSTUMENT_AXIS_2_MAX_CURRENT,
			IRP6_POSTUMENT_AXIS_3_MAX_CURRENT,
			IRP6_POSTUMENT_AXIS_4_MAX_CURRENT,
			IRP6_POSTUMENT_AXIS_5_MAX_CURRENT,
			IRP6_POSTUMENT_AXIS_6_MAX_CURRENT };

	hi = new hardware_interface(master, IRQ_REAL, INT_FREC_DIVIDER,
			HI_RYDZ_INTR_TIMEOUT_HIGH, FIRST_SERVO_PTR,
			INTERRUPT_GENERATOR_SERVO_PTR, ISA_CARD_OFFSET, max_current);
	hi->init();

	// utworzenie tablicy regulatorow
	// Serwomechanizm 1

	// regulator_ptr[1] = new NL_regulator_2 (0, 0, 0.71, 13./4, 12.57/4, 0.35);
	// kolumna dla irp6 postument
	regulator_ptr[0] = new NL_regulator_2_irp6p(0, 0, 0.429, 6.834, 6.606,
			0.35, master); // kolumna dla irp6 postument

	regulator_ptr[1] = new NL_regulator_3_irp6p(0, 0, 0.64, 9.96 / 4, 9.54 / 4,
			0.35, master);

	// regulator_ptr[3] = new NL_regulator_4 (0, 0, 0.62, 9.85/4, 9.39/4, 0.35);
	regulator_ptr[2] = new NL_regulator_4_irp6p(0, 0, 0.333, 5.693, 5.427,
			0.35, master);

	regulator_ptr[3] = new NL_regulator_5_irp6p(0, 0, 0.56, 7.98 / 2, 7.55 / 2,
			0.35, master);

	// regulator_ptr[5] = new NL_regulator_6 (0, 0, 0.3079*2, 0.6, 0.6, 0.35);
	regulator_ptr[4] = new NL_regulator_6_irp6p(0, 0, 0.39, 8.62 / 2., 7.89
			/ 2., 0.35, master);
	// regulator_ptr[0] = new NL_regulator_1 (0, 0, 0.64, 16.61/5., 15.89/5., 0.35);

	regulator_ptr[5] = new NL_regulator_7_irp6p(0, 0, 0.39, 8.62 / 2., 7.89
			/ 2., 0.35, master);

	common::servo_buffer::load_hardware_interface();

}

} // namespace irp6p
namespace common {

servo_buffer* return_created_servo_buffer(motor_driven_effector &_master) {
	return new irp6p_m::servo_buffer((irp6p_m::effector &) (_master));
}

} // namespace common
} // namespace edp
} // namespace mrrocpp

