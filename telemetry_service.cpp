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

#include "c_library_v2/common/mavlink.h"
#include "mavserial.h"
#include "websocket.h"

mavserial ms; // mavlink serial interface
websocket ws; // json websocket interface

//Callbacks
void ws_emitMessage(std::string message);  // serial -> websocket
void mav_emitMessage(std::string message); // websocket -> serial


// Service entry
int main(int argc, char* argv[]) 
{
    std::string uri = "ws://localhost:3000";

    if (argc == 2) {
        uri = argv[1];
    }

    // initialize mavlink serial
    ms.set_message_handler(&ws_emitMessage);
    ms.run("/dev/serial0",  57600);

    // initialize websocket
    ws.set_message_handler(&mav_emitMessage);
    ws.connect(uri);  
    
    return 0;
}

//Callback to send message via Websocket
void ws_emitMessage(std::string message)
{
    if(message.length() > 0)
    {
        ws.send(message);
    }
}

//callback to send mavlink message over serial
void mav_emitMessage(std::string message)
{
    //this method is left blank.
    std::cout << "In Callback: " << message << std::endl;
}