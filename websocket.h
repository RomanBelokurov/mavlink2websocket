/* Websocket client class.
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

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

class websocket
{
    public:
        typedef websocketpp::client<websocketpp::config::asio_client> client;

        websocket()
        {
            m_client.clear_access_channels(websocketpp::log::alevel::all);
            m_client.set_access_channels(websocketpp::log::alevel::connect);
            m_client.set_access_channels(websocketpp::log::alevel::disconnect);
            m_client.set_access_channels(websocketpp::log::alevel::app);

            isConnected = false;
        }

        void (*on_message_callback)(std::string);
        void set_message_handler(void(*on_message_callback)(std::string))
        {
            this->on_message_callback = on_message_callback;
        }

        void connect(const std::string &uri)
        {
            m_client.init_asio();

            using websocketpp::lib::bind;
            using websocketpp::lib::placeholders::_1;
            using websocketpp::lib::placeholders::_2;
            
            m_client.set_close_handler(bind(&websocket::on_close, this, _1));
            m_client.set_fail_handler(bind(&websocket::on_fail, this, _1));
            m_client.set_message_handler(bind(&websocket::on_message, this, _1, _2));


            websocketpp::lib::error_code ec;
            client::connection_ptr con = m_client.get_connection(uri, ec);
            
            if (ec) 
            {
                m_client.get_alog().write(websocketpp::log::alevel::app, "Get Connection Error: " + ec.message());
                return;
            }

            m_hdl = con->get_handle();
            m_client.connect(con);
            isConnected = true;

            m_client.run();
        }

        void on_close(websocketpp::connection_hdl) 
        {
            m_client.get_alog().write(websocketpp::log::alevel::app, "Connection closed, stopping telemetry!");
            isConnected = false;
        }

        void on_fail(websocketpp::connection_hdl) 
        {
            m_client.get_alog().write(websocketpp::log::alevel::app, "Connection failed, stopping telemetry!");
            isConnected = false;
        }

        void on_message(websocketpp::connection_hdl, client::message_ptr msg) 
        {
            if (msg->get_opcode() == websocketpp::frame::opcode::text)
		    {
			    m_client.get_alog().write(websocketpp::log::alevel::app, "Text Message Received: " + msg->get_payload());
                on_message_callback(msg->get_payload());
		    }
		    else
		    {
			    m_client.get_alog().write(websocketpp::log::alevel::app, "Binary Message Received: " + websocketpp::utility::to_hex(msg->get_payload()));
		    }
        }
        
        void send(std::string message)
        {
            if(isConnected)
            {
                websocketpp::lib::error_code ec;
                m_client.send(m_hdl, message, websocketpp::frame::opcode::text, ec);
            }
        }
    
    private:
        client m_client;
        websocketpp::connection_hdl m_hdl;
        bool isConnected;
};

#endif