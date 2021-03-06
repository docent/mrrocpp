// ------------------------------------------------------------------------
// Proces:		EDP
// Plik:			edp_irp6p_effector.h
// System:	QNX/MRROC++  v. 6.3
// Opis:		Robot IRp-6 na postumencie
//				- deklaracja klasy edp_irp6p_effector
//
// Autor:		tkornuta
// Data:		17.01.2007
// ------------------------------------------------------------------------


#ifndef __EDP_IRP6P_TFG_H
#define __EDP_IRP6P_TFG_H

// Klasa edp_irp6s_robot.
#include "robot/irp6p_tfg/sg_irp6p_tfg.h"
#include "base/edp/edp_e_motor_driven.h"
#include "robot/irp6p_tfg/irp6p_tfg_const.h"

namespace mrrocpp {
namespace edp {
namespace irp6p_tfg {

// Klasa reprezentujaca tasmociag.
class effector : public common::motor_driven_effector
{
protected:
	// Metoda tworzy modele kinematyczne dla robota IRp-6 na postumencie.
	virtual void create_kinematic_models_for_given_robot(void);

public:
	// Konstruktor.
	effector(lib::configurator &_config);

	void set_robot_model(const lib::c_buffer &instruction); // zmiana narzedzia

	void create_threads();

	// Przemieszczenie ramienia.
	void move_arm(const lib::c_buffer &instruction);
	// Odczytanie pozycji ramienia.
	void get_arm_position(bool read_hardware, lib::c_buffer &instruction);
	// Aktualizacja polozenia.


	common::servo_buffer *return_created_servo_buffer();
	void master_order(common::MT_ORDER nm_task, int nm_tryb);

};

} // namespace common
} // namespace edp
} // namespace mrrocpp

#endif
