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

#pragma once

#include <string>
#include <iostream>

#include <glib.h>
#include <glib-unix.h>
#include <libsoup/soup.h>


class WebSocketClient
{
    public:
        WebSocketClient() : websocket_connection(nullptr) {}
        ~WebSocketClient() {}

        
        void set_message_handler(void(*on_message_callback)(std::string))
        {
            this->on_message_callback = on_message_callback;
        }

        bool IsConnected() const
        {
            return (websocket_connection != nullptr);
        }

        bool Connect(const std::string &server_addr)
        {
            this->websocket_connection = nullptr;
            this->server_addr = server_addr;

            // Create the soup session with WS or WSS
            SoupSession *session = soup_session_new();
            if (!session) 
            {
                std::cout << "Failed to create SoupSession\n";
                return false;
            }

            // Create the soup message
            SoupMessage *msg = soup_message_new(SOUP_METHOD_GET, this->server_addr.c_str());
            if (!msg) 
            {
                std::cout << "Failed to create SoupMessage\n";
                g_object_unref(session);
                return false;
            }

            // Connect to our websocket server
            std::cout << "Connecting to websocket uri: " << server_addr << std::endl;
            soup_session_websocket_connect_async(
                session,
                msg,
                NULL, NULL, NULL,
                (GAsyncReadyCallback) &OnConnection,
                this
            );
    
            g_clear_object(&msg);
            g_clear_object(&session);
            return true;
        }
        
        bool SendMessage(const std::string &message)
        {
            if (message.empty()) {
                std::cout << "SendMessage: Empty message\n";
                return true;
            }

            if (!websocket_connection) {                
                return false;
            }

            soup_websocket_connection_send_text(websocket_connection, message.c_str());
            return true;
        }

        bool Close()
        {
            if (websocket_connection)
                soup_websocket_connection_close(websocket_connection, SOUP_WEBSOCKET_CLOSE_NORMAL, NULL);
            return true;
        }

    private:
        
        static void OnConnection(SoupSession *session, GAsyncResult *res, gpointer data)
        {
            ((WebSocketClient *) data)->OnConnectionImplementation(session, res);
        }

        void OnConnectionImplementation(SoupSession *session, GAsyncResult *res)
        {
            GError *error = NULL;
            SoupWebsocketConnection *conn = soup_session_websocket_connect_finish(session, res, &error);
    
            if (error) 
            {
                std::cout << "Failed to connect: " << error->message << std::endl;
                g_error_free(error);
                g_clear_object(&conn);
                return;
            }            

            g_signal_connect(conn, "message",  G_CALLBACK(OnMessage),  this);
            g_signal_connect(conn, "closing",  G_CALLBACK(OnClosing),  this);
            g_signal_connect(conn, "closed",   G_CALLBACK(OnClose),    this);
            
            websocket_connection = conn;            
        }

        static void OnMessage(SoupWebsocketConnection *conn, int type, GBytes *message, gpointer data){
            ((WebSocketClient *) data)->OnMessageImplementation(conn, type, message);
        }

        void OnMessageImplementation(SoupWebsocketConnection *conn, int type, GBytes *gbytes)
        {
            if (type == SOUP_WEBSOCKET_DATA_TEXT) 
            {
                unsigned int message_size;
                std::string message = static_cast<const char *>(g_bytes_get_data(gbytes, (gsize *)&message_size));
                on_message_callback(message);
            }
            else if (type == SOUP_WEBSOCKET_DATA_BINARY) 
            {
                unsigned int data_size;
                const unsigned char *data = static_cast<const unsigned char *>(g_bytes_get_data(gbytes, (gsize *)&data_size));
                std::cout << "Received binary data: " << data_size << " bytes\n";
            }
            else 
            {
                std::cout << "Invalid data type\n";
            }
        }


        static void OnClosing(SoupWebsocketConnection *conn, gpointer data){
            ((WebSocketClient *) data)->OnClosingImplementation(conn);
        }
        
        void OnClosingImplementation(SoupWebsocketConnection *conn)
        {
            std::cout << "Websocket is closing\n";
        }



        static void OnClose(SoupWebsocketConnection *conn, gpointer data){
            ((WebSocketClient *) data)->OnCloseImplementation(conn);
        }
        
        void OnCloseImplementation(SoupWebsocketConnection *conn)
        {
            soup_websocket_connection_close(conn, SOUP_WEBSOCKET_CLOSE_NORMAL, NULL);
            g_clear_object(&conn);
            websocket_connection = nullptr;
            std::cout << "Websocket is closed\n";
        }

    private:
        void (*on_message_callback)(std::string);
        std::string server_addr;
        SoupWebsocketConnection* websocket_connection;
};
