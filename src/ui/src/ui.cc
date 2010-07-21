/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

#include "ui/ui.h"
#include <cstdio>

busy_flagger::busy_flagger(busy_flag & _flag) :
	flag(_flag) {
	flag.increment();
}

busy_flagger::~busy_flagger() {
	flag.decrement();
}

busy_flag::busy_flag() :
	counter(0) {
}

void busy_flag::increment(void) {
	boost::mutex::scoped_lock lock(m_mutex);
	counter++;
}

void busy_flag::decrement(void) {
	boost::mutex::scoped_lock lock(m_mutex);
	counter--;
}

bool busy_flag::is_busy() const {
	//	boost::mutex::scoped_lock lock(m_mutex);
	return (counter);
}

function_execution_buffer::function_execution_buffer() :
	has_command(false) {

}

void function_execution_buffer::command(command_function_t _com_fun) {
	boost::unique_lock<boost::mutex> lock(mtx);

	// assign command for execution
	com_fun = _com_fun;
	has_command = true;

	cond.notify_one();

	return;
}

int function_execution_buffer::wait_and_execute() {
	command_function_t popped_command;

	{
		boost::unique_lock<boost::mutex> lock(mtx);

		while (!has_command) {
			cond.wait(lock);
		}

		has_command = false;
		popped_command = com_fun;
	}

	busy_flagger flagger(communication_flag);
	set_ui_state_notification(UI_N_BUSY);
	return popped_command();
}

void feb_thread::operator()() {
#if defined(__QNXNTO__)
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGALRM);

	if (SignalProcmask(0, 0, SIG_BLOCK, &set, NULL) == -1) {
		std::perror("SignalProcmask(edp_irp6p_tid)");
	}
#endif

	while (true) {
		feb.wait_and_execute();
	}
}

feb_thread::feb_thread(function_execution_buffer & _feb) :
	feb(_feb) {
	thread_id = new boost::thread(boost::bind(&feb_thread::operator(), this));
}

feb_thread::~feb_thread() {
	thread_id->interrupt();
	thread_id->join(); // join it
	delete thread_id;
}
