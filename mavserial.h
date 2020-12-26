/* Mavlink serial interface.
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

#ifndef MAVSERIAL_H
#define MAVSERIAL_H

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <thread>
#include <mutex>

#include "helpers.h"
#include "json.h"

class mavserial
{
    public:
        mavserial(){ }
        ~mavserial(){ mavlink_thread->join(); }
        
        void set_message_handler(void(*on_message_callback)(std::string))
        {
            this->on_message_callback = on_message_callback;
        }

        void run(const char* device, int baudrate)
        {
            serialHandle = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
            if (serialHandle == -1)
            {
                std::cout << "Error - Unable to open UART.  Ensure it is not in use by another application" << std::endl;
                return;                
            }
            
            if(get_baud(baudrate) < 0) 
            { 
                std::cout << "Wrong baudrate setting. Abort." << std::endl;
                return;
            }

            struct termios options;
            tcgetattr(serialHandle, &options);    

            options.c_cflag = get_baud(baudrate) | CS8 | CLOCAL | CREAD;
            options.c_iflag = IGNPAR;    
            options.c_oflag = 0;
            options.c_lflag = 0;
            
            tcsetattr(serialHandle, TCSANOW, &options);
            fcntl(serialHandle, F_SETFL, FNDELAY);

            if(serialHandle >= 0)
            {
                mavlink_thread = std::make_unique<std::thread>(serial_loop, serialHandle, on_message_callback);
            }
        }

        
        static void serial_loop(int serialHandle, void(*on_message_callback)(std::string))
        {
            while(true)
            {
                unsigned char rx_buffer[MAVLINK_MAX_PACKET_LEN+16];
                int rx_length = read(serialHandle, (void *)rx_buffer, MAVLINK_MAX_PACKET_LEN);

                if(rx_length > 0)
                {
                    mavlink_message_t msg;
                    mavlink_status_t status;
                    int chan = 0;
                    for (ssize_t i = 0; i < rx_length; i++)
                    {
                        if (mavlink_parse_char(chan, rx_buffer[i], &msg, &status))
                        {           
                            on_message_callback(serialize_json(&msg));
                        }
                    }     
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 10Hz
            }
        }
    
    private: 
        std::unique_ptr<std::thread> mavlink_thread;
        int serialHandle = -1;
        void (*on_message_callback)(std::string);
};

#endif