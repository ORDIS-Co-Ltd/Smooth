//
// Created by permal on 6/25/17.
//

#include <algorithm>
#include <smooth/core/Task.h>
#include <smooth/core/timer/ElapsedTime.h>
#include <smooth/core/logging/log.h>

using namespace smooth::core::logging;

namespace smooth
{
    namespace core
    {
        Task::Task(const std::string& task_name, uint32_t stack_size, uint32_t priority,
                   std::chrono::milliseconds tick_interval)
                : name(task_name),
                  worker(),
                  stack_size(stack_size),
                  priority(priority),
                  tick_interval(tick_interval),
                  notification(*this)
        {
        }

        Task::Task(uint32_t priority, std::chrono::milliseconds tick_interval)
                :
                name("MainTask"),
                stack_size(0),
                priority(priority),
                tick_interval(tick_interval),
                notification(*this)
        {
            is_attached = true;
        }

        Task::~Task()
        {
            notification.clear();
        }

        void Task::start()
        {
            // Prevent multiple starts
            if (!started)
            {
                if (is_attached)
                {
                    // Attaching to another task, just run execute.
                    exec();
                }
                else
                {
                    worker = std::thread([this]()
                                         {
                                             this->exec();
                                         });

                    // To avoid race conditions between tasks during start up,
                    // always wait for the new task to start.
                    while(!started)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                }
            }
        }

        void Task::exec()
        {
#ifdef ESP_PLATFORM
            freertos_task = xTaskGetCurrentTaskHandle();
            vTaskPrioritySet(nullptr, priority);
#endif

            Log::verbose("Task", Format("Initializing task '{1}'", Str(name)));

            init();
            started = true;

            Log::verbose("Task", Format("Task '{1}' initialized", Str(name)));

            timer::ElapsedTime delayed{};

            delayed.start();

            for (;;)
            {
                // Try to keep the tick alive even when there are lots of incoming messages
                // by simply not checking the queues when more than one tick interval has passed.
                if (tick_interval.count() > 0 && delayed.get_running_time() > tick_interval)
                {
                    tick();
                    delayed.reset();
                }
                else
                {
                    auto* queue = notification.wait_for_notification(this, tick_interval);

                    if (queue == nullptr)
                    {
                        // Timeout - no messages.
                        tick();
                        delayed.reset();
                    }
                    else
                    {
                        // A queue has signaled an item is available.
                        // Note: do not get tempted to retrieve all messages from
                        // the queue - it would cause message ordering to get mixed up.
                        queue->forward_to_event_queue();
                    }
                }
            }
        }

        void Task::register_queue_with_task(smooth::core::ipc::ITaskEventQueue* task_queue)
        {
            task_queue->register_notification(&notification);
        }
    }
}