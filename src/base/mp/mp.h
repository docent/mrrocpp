// -------------------------------------------------------------------------
//                            mp.h
// Definicje struktur danych i metod dla procesow MP
//
// Ostatnia modyfikacja: 16.04.98
// -------------------------------------------------------------------------

#if !defined(__MP_H)
#define __MP_H

#include <map>
#include <stdio.h>

#if defined(__QNXNTO__)
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#endif

#include "lib/impconst.h"
#include "lib/com_buf.h"
#include "lib/configurator.h"

namespace mrrocpp {
namespace mp {
namespace robot {
class robot;
}

namespace common {

typedef std::map <lib::robot_name_t, robot::robot*> robots_t;
typedef robots_t::value_type robot_pair_t;

// ---------------------------------------------------------------
class MP_main_error
{ // Klasa obslugi bledow poziomie MP
public:
	const lib::error_class_t error_class;
	const uint64_t error_no;

	MP_main_error(lib::error_class_t err0, uint64_t err1, const char *file, int line) :
		error_class(err0), error_no(err1)
	{
		fprintf(stderr, "ecp_MP_main_error @ %s:%d\n", file, line);
	}
#define MP_main_error(e0,e1)	MP_main_error((e0),(e1), __FILE__, __LINE__)
};
// ---------------------------------------------------------------

} // namespace common

} // namespace mp
} // namespace mrrocpp

// to fix forward declaration issues
#include "base/mp/mp_generator.h"
#include "base/mp/mp_task.h"
#include "base/mp/mp_robot.h"
#include "lib/com_buf.h"

#endif
