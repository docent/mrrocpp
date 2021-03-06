// -------------------------------------------------------------------------
//                            sg_local.h
// Definicje struktur danych i metod dla procesu EDP on_track
//
// Ostatnia modyfikacja: 2006
// -------------------------------------------------------------------------


#ifndef __REGULATOR_SARKOFAG_H
#define __REGULATOR_SARKOFAG_H
#include "base/edp/edp.h"
#include "base/edp/regulator.h"

namespace mrrocpp {
namespace edp {
namespace sarkofag {

// stale dla automatu w regulatorze sarkofagu
#define GRIPPER_BLOCKED_TIME_PERIOD 200
#define MAX_GRIPPER_SUM_OF_CURRENTS_NR_OF_ELEMENTS 1000

#define SARKOFAG_GRIPPER_SUM_OF_CURRENTS_NR_OF_ELEMENTS 100
#define SARKOFAG_GRIPPER_SUM_OF_CURRENTS_MAX_VALUE 1000
// #define SARKOFAG_GRIPPER_SUM_PWM_MIN 80


#define SARKOFAG_AXIS_7_SYNCHRO_STEP_COARSE -0.5
#define SARKOFAG_AXIS_7_SYNCHRO_STEP_FINE   -0.2

#define SARKOFAG_AXE8_MAX_PWM_INCREMENT	1000

#define SARKOFAG_AXIS_7_INC_PER_REVOLUTION  128.0  // Liczba impulsow enkodera na obrot walu - musi byc float

/*-----------------------------------------------------------------------*/
class NL_regulator_8_sarkofag : public common::NL_regulator
{
	/* Klasa regulatorow konkretnych */
	// Obiekt z algorytmem regulacji

	long gripper_blocked_start_time;
	int sum_of_currents, current_index;
	int currents[MAX_GRIPPER_SUM_OF_CURRENTS_NR_OF_ELEMENTS];

public:
			NL_regulator_8_sarkofag(uint8_t reg_no, uint8_t reg_par_no, double aa, double bb0, double bb1, double k_ff, common::motor_driven_effector &_master); // konstruktor

	virtual uint8_t compute_set_value(void);
	// obliczenie nastepnej wartosci zadanej dla napedu - metoda konkretna

}; // end: class NL_regulator_8
// ----------------------------------------------------------------------


} // namespace sarkofag
} // namespace edp
} // namespace mrrocpp


#endif
