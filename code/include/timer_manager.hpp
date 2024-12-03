#pragma once

#include "defines.hpp"
#include <chrono>



class TimerInstance;

using timer_function = void (*)(void*);
using TimerInstancePtr = std::unique_ptr<TimerInstance>;

class TimerInstance
{
public:
	TimerInstance(TimerManager* manager);
    ~TimerInstance();

	void start();
	void stop();

private:
	TimerManager* m_manager;
};

class TimerManager
{
public:
	struct Timer
	{
		std::chrono::seconds delay;
		std::chrono::time_point<std::chrono::steady_clock> timestamp;
		timer_function func;
		void* user_data;
		int repeat_count;

		TimerInstance* owner;
	};

    static TimerManager& get();

	TimerInstancePtr create_timer(std::chrono::seconds delay, timer_function func, void* user_data, int repeat = 1);

    void update();

	friend class TimerInstance;

private:
    TimerManager();

    void remove_expired_timers();

    std::vector<Timer>::iterator alloc_timer();

	void destroy_timer(TimerInstance* instance);

	void start_timer(TimerInstance* instance);

private:
    std::vector<Timer> m_timers;
	std::vector < std::vector<Timer>::iterator> m_remove_timers;
};

template<>
struct ::std::default_delete<TimerInstance>
{
	default_delete() = default;
	template<class U>
	constexpr default_delete(default_delete<U>) noexcept
	{
	}
	void operator()(TimerInstance* p) const noexcept { DELETE_OBJ(p); }
};
