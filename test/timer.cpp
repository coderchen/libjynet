#include <jynet/event_dispatcher.h>
#include <jynet/timer_handler.h>
#include <iostream>

class test_timer : public timer_handler
{
	public:
		test_timer(int ms, const char *name)
			: ms_(ms), name_(name)
		{ }

		virtual void handle_timeout(
				event_dispatcher *dispatcher,
				int64_t cur_ms)
		{
			std::cout << this->name_ << ", now:" << cur_ms << "(ms)" << std::endl;
			this->schedule_timer(dispatcher, this->ms_);
		}

		void start(event_dispatcher *dispatcher)
		{
			this->schedule_timer(dispatcher, this->ms_);
		}

	private:
		int ms_;
		const char *name_;
};

int main()
{
	event_dispatcher ed;
	if (ed.init() != 0) {
		std::cout << "init err." << std::endl;
		return -1;
	}

	test_timer timer1(10, "I'm 10ms timer");
	test_timer timer2(200, "I'm 200ms timer");
	test_timer timer3(2000, "I'm 2s timer");
	test_timer timer4(10000, "I'm 10s timer");

	timer1.start(&ed);
	timer2.start(&ed);
	timer3.start(&ed);
	timer4.start(&ed);

	ed.run();
	return 0;
}
