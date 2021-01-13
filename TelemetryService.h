/* Mavlink to websocket telemetry streaming service.
 * Copyright (c) 2020 Roman Belokurov
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#pragma once

#include <glib.h>
#include <glib-unix.h>
#include "MavlinkSerial.h"
#include "WebSocketClient.h"


class TelemetryService
{
    public:
        TelemetryService(std::string webSocketUri, std::string serialPort, int baudrate)
        {
            m_webSocketClient = 0;
            m_mavlinkSerial = 0;
            m_mainLoop = 0;
            
            m_webSocketUri = webSocketUri;
            m_serialPort = serialPort;
            m_baudrate = baudrate;
        }

        ~TelemetryService()
        {   m_webSocketClient = 0;
            delete(m_webSocketClient);
            m_mavlinkSerial = 0;
            delete(m_mavlinkSerial);
        }

        bool Initialize(void(*on_mavlink_message_callback)(std::string), void(*on_websocket_message_callback)(std::string))
        {
            m_mavlinkSerial = new MavlinkSerial();
            m_mavlinkSerial->set_message_handler(on_websocket_message_callback);

            m_webSocketClient = new WebSocketClient();
            m_webSocketClient->set_message_handler(on_mavlink_message_callback);
            
            return !m_mavlinkSerial && !m_webSocketClient;
        }

        void RunService()
        {                       
            m_mainLoop = g_main_loop_new (NULL, FALSE);

            m_mavlinkSerial->run(m_serialPort,  m_baudrate);
            m_webSocketClient->Connect(m_webSocketUri);
            
            g_main_loop_run (m_mainLoop);
            g_main_loop_unref (m_mainLoop);
        }

        void StopService()
        {
            g_main_loop_unref (m_mainLoop);
            m_webSocketClient->Close();
        }
    
        WebSocketClient* GetWebSocketHandler() {return m_webSocketClient;}
        MavlinkSerial* GetMavlinkSerialHandler() {return m_mavlinkSerial;}

    private:
        WebSocketClient* m_webSocketClient;
        MavlinkSerial* m_mavlinkSerial;

        std::string m_webSocketUri;
        std::string m_serialPort;
        int m_baudrate;

        GMainLoop* m_mainLoop;
};