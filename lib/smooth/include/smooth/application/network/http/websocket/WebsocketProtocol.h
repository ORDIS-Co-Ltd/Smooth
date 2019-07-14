// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <smooth/core/network/IPacketAssembly.h>
#include <smooth/application/network/http/regular/HTTPPacket.h>
#include <smooth/application/network/http/regular/IServerResponse.h>

namespace smooth::application::network::http::websocket
{
    class WebsocketProtocol
            : public smooth::core::network::IPacketAssembly<WebsocketProtocol, regular::HTTPPacket>
    {
            using packet_type = regular::HTTPPacket;
        public:
            WebsocketProtocol(int content_chunk_size, regular::IServerResponse& response)
                    : content_chunk_size(content_chunk_size),
                      response(response)
            {
            }

            int get_wanted_amount(regular::HTTPPacket& packet) override;

            void data_received(regular::HTTPPacket& packet, int length) override;

            uint8_t* get_write_pos(regular::HTTPPacket& packet) override;

            bool is_complete(regular::HTTPPacket& packet) const override;

            bool is_error() override;

            void packet_consumed() override;

            void reset() override;

        private:
            enum class State
            {
                    Header,
                    ExtendedPayloadLength_2,
                    ExtendedPayloadLength_8,
                    MaskingKey,
                    Payload
            };

            enum class OpCode
            {
                Continuation,
                Text,
                Binary,
                Reserved3,
                Reserved4,
                Reserved5,
                Reserved6,
                Reserved7,
                // Control frames below (MSB is set)
                Close,
                Ping,
                Pong,
                ReservedA,
                ReservedB,
                ReservedC,
                ReservedD,
                ReservedE,
                ReservedF,
            };


            State state{State::Header};

            int content_chunk_size;
            bool fin{false};
            bool RSV1{false};
            bool RSV2{false};
            bool RSV3{false};
            OpCode op_code{OpCode::Continuation};
            bool masked{false};
            regular::IServerResponse& response;

            bool error{false};
            int total_byte_count{0};
            int payload_length{0};
            int received_payload{0};
            int decode_index{0};

            std::array<uint8_t, 4> mask_key{};
            std::array<uint8_t, 11> frame_data{};
    };
}