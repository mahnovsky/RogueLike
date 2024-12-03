#include "timer_manager.hpp"


TimerInstance::TimerInstance(TimerManager* manager)
	: m_manager(manager)
{
}

TimerInstance::~TimerInstance()
{
	m_manager->destroy_timer(this);
}

void TimerInstance::start()
{
	m_manager->start_timer(this);
}

void TimerInstance::stop()
{
	
}

TimerManager& TimerManager::get()
{
    static TimerManager mng;

    return mng;
}

TimerManager::TimerManager()
    :m_timers()
    ,m_remove_timers()
{}

TimerInstancePtr TimerManager::create_timer(std::chrono::seconds delay, timer_function func, void* user_data, int repeat)
{
	const auto timer_it = alloc_timer();

    timer_it->delay = delay;
	timer_it->func = func;
	timer_it->user_data = user_data;
	timer_it->repeat_count = repeat;
	timer_it->owner = NEW_OBJ(TimerInstance, this);

    return TimerInstancePtr{ timer_it->owner };
}

void TimerManager::update( )
{
	const auto current = std::chrono::steady_clock::now();

    for(auto it = m_timers.begin(); it != m_timers.end(); ++it)
    {
        Timer& timer = *it;

        if (!it->owner) {
			m_remove_timers.emplace_back(it);
			continue;
        }

        const auto delta = current - it->timestamp;

        if(delta > timer.delay )
        {
            timer.func( timer.user_data );
            
            if( timer.repeat_count > 0 )
            {
                --timer.repeat_count;
            }

            if( timer.repeat_count == 0 )
            {
				m_remove_timers.emplace_back(it);

                continue;
            }

            timer.timestamp = std::chrono::steady_clock::now();
        }
    }

    remove_expired_timers();
}

void TimerManager::remove_expired_timers()
{
	if (m_remove_timers.empty())
	{
        return;
	}

    for(const auto it : m_remove_timers)
    {
		m_timers.erase(it);
    }

    m_remove_timers.clear();
}

std::vector<TimerManager::Timer>::iterator TimerManager::alloc_timer()
{
	if (!m_remove_timers.empty())
	{
		const auto timer_it = m_remove_timers.back();
		m_timers.pop_back();

        return timer_it;
	}


	m_timers.emplace_back(Timer{});

    return std::prev(m_timers.end());
}

void TimerManager::destroy_timer(TimerInstance* instance)
{
	const auto it = std::find_if(m_timers.begin(), m_timers.end(), [instance](const Timer& t) {
		return t.owner == instance;
	});

    if (it != m_timers.end()) {
		it->owner = nullptr;
    }
}

void TimerManager::start_timer(TimerInstance* instance)
{
}
