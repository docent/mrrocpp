/*
 * generator/ecp_g_sleep.h
 *
 *Author: Tomasz Bem
 */

#ifndef ECP_G_SLEEP_H_
#define ECP_G_SLEEP_H_

#include <time.h>
#include "base/ecp/ecp_generator.h"

namespace mrrocpp {
namespace ecp {
namespace common {
namespace generator {

class sleep : public common::generator::generator
{
	private:
		double waittime;		//seconds to wait
		timespec sleeptime;		//structure for nanosleep funciton
		timespec acttime;
		timespec prevtime;
		timespec starttime;

	public:
		sleep(common::task::task& _ecp_task, double=1);		//constructor
		bool first_step();		//first step generation
		bool next_step();			//next step generation
		void init_time(double=1);	//initialize time
};

} // namespace generator
} // namespace common
} // namespace ecp
} // namespace mrrocpp

#endif /* ECP_G_SLEEP_H_ */
