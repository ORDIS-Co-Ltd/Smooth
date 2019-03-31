#pragma once

#include <smooth/core/network/ProtocolClient.h>
#include <chrono>
#include "StreamingProtocol.h"
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/network/BufferContainer.h>
#include <smooth/core/network/event/DataAvailableEvent.h>

namespace server_socket_test
{

    class StreamingClient
            : public smooth::core::network::ProtocolClient<StreamingProtocol>
    {
        public:
            explicit StreamingClient(smooth::core::Task& task)
                    : ProtocolClient<StreamingProtocol>(task, *this, *this, *this)

            {
            }

            ~StreamingClient() override
            {

            }

            void event(const smooth::core::network::event::DataAvailableEvent<StreamingProtocol>& event) override
            {

            }

            void event(const smooth::core::network::event::TransmitBufferEmptyEvent& event) override
            {

            }

            void event(const smooth::core::network::event::ConnectionStatusEvent& event) override
            {

            }


            std::chrono::milliseconds get_send_timeout() override
            {
                return std::chrono::seconds{1};
            };
    };


}