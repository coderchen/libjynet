#include "timer_min_heap.h"

#include <string.h>

#include "timer_handler.h"

timer_min_heap::timer_min_heap()
	: capacity_(0), size_(0), timers_(NULL)
{
}
timer_min_heap::~timer_min_heap()
{
	if (this->timers_)
		delete []this->timers_;
}

int timer_min_heap::grow_up()
{
	int new_capacity = this->capacity_ ? this->capacity_ * 2 : 512;
	timer_node *new_timers = new timer_node[new_capacity];
	if (!new_timers)
		return -1;

	if (this->timers_)
	{
		::memcpy(new_timers, this->timers_, sizeof(timer_node) * this->capacity_);	
		delete []this->timers_;
	}
	this->timers_ = new_timers;
	this->capacity_ = new_capacity;
	return 0;
}
int timer_min_heap::nearest_timeout(int64_t now_ms)
{
	int ms = -1;
	if (this->size_ > 0)
	{
		ms = this->timers_[0].dead_line_ - now_ms;
		if (ms <= 0) // sorry, may be dealyed
			ms = 5;
	}
	return ms;
}
int timer_min_heap::schedule_timer(int64_t dead_line, timer_handler *handler)
{
	while (this->size_ >= this->capacity_)
		if (this->grow_up() != 0)
			return -1;

	this->timers_[this->size_].dead_line_ = dead_line;
	this->timers_[this->size_].handler_ = handler;
	this->adjust_up(this->size_++);

	return 0;
}
timer_handler* timer_min_heap::check_timeout(int64_t now_ms)
{
	timer_handler *handler = NULL;
	if (this->size_ && this->timers_[0].dead_line_ <= now_ms)
	{
		handler = this->timers_[0].handler_;
		this->timers_[0] = this->timers_[--this->size_];
		this->adjust_down(0);
	}
	return handler;
}
void timer_min_heap::adjust_down(int slot)
{
	if (slot < 0 || slot >= this->size_)
		return;

	timer_node tmp_node = this->timers_[slot];
	while (2 * slot + 1 < this->size_)
	{
		int child = 2 * slot + 1;
		if (child + 1 < this->size_ 
				&& this->timers_[child + 1].dead_line_ < this->timers_[child].dead_line_)
			++child;
		if (this->timers_[child].dead_line_ < tmp_node.dead_line_)
		{
			this->timers_[slot] = this->timers_[child];
			slot = child;
		}
		else
			break;
	}
	this->timers_[slot] = tmp_node;
}
void timer_min_heap::adjust_up(int slot)
{
	if (slot <= 0 || slot >= this->size_)
		return;

	timer_node tmp_node = this->timers_[slot];
	while (slot > 0 && (slot - 1) / 2 >= 0)
	{
		int parent = (slot - 1) / 2;
		if (this->timers_[parent].dead_line_ > tmp_node.dead_line_)
		{
			this->timers_[slot] = this->timers_[parent];
			slot = parent;
		}
		else
			break;
	}
	this->timers_[slot] = tmp_node;
}
