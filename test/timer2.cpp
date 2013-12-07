#include <jynet/event_dispatcher.h>
#include <jynet/timer_handler.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>

class test_timer2 : public timer_handler
{
	public:
		test_timer2(int ms, int id)
			: ms_(ms), id_(id)
		{ }

		virtual void handle_timeout(
				event_dispatcher *dispatcher,
				int64_t cur_ms)
		{
//			std::cout << this->id_ << ", now:" << cur_ms << std::endl;
			this->schedule_timer(dispatcher, this->ms_);
		}

		void start(event_dispatcher *dispatcher)
		{
			this->schedule_timer(dispatcher, this->ms_);
		}

	private:
		int ms_;
		int id_;
};

int main()
{
	event_dispatcher ed;
	if (ed.init() != 0) {
		std::cout << "init err." << std::endl;
		return -1;
	}

	::srand(::time(NULL));
	const int timer_size = 10000;
	for (int i = 0; i < timer_size; ++i)
	{
		int ms = rand() % 10000 + 1;
		test_timer2 *timer = new test_timer2(ms, i);
		timer->start(&ed);
	}

	ed.run();
	return 0;
}
