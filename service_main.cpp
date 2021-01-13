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


#include <iostream>
#include <string>
#include <algorithm>

#include "TelemetryService.h"

TelemetryService* telemetryService;

//Callback to send message via Websocket
void ws_emitMessage(std::string message)
{
    if(message.length() > 0)
    {
        telemetryService->GetWebSocketHandler()->SendMessage(message);        
    }
}

//callback to send mavlink message over serial
static void ms_emitMessage(std::string message)
{                        
    std::cout << "In Callback: " << message << std::endl;
    
    // here we add some code to transmit if needed
    // telemetryService->GetMavlinkSerialHandler()->SendMessage();
}


char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{    
    return std::find(begin, end, option) != end;
}

int main(int argc, char* argv[]) 
{
    // default settings:
    std::string uri = "ws://localhost:3000";
    std::string serialDevice = "/dev/serial0";
    int baud = 57600;

    if(cmdOptionExists(argv, argv+argc, "-h"))
    {
        std::cout << "\n\nMavlink to websocket streaming service.\n";
        std::cout << "Usage: ./uavClient -uri ws://127.0.0.1:3000 -serial /dev/serial0 -baud 57600  \n\n";
        return 0;
    }

    if(cmdOptionExists(argv, argv+argc, "-uri"))
        uri = getCmdOption(argv, argv + argc, "-uri");

    if(cmdOptionExists(argv, argv+argc, "-serial"))
        serialDevice = getCmdOption(argv, argv + argc, "-serial");

    if(cmdOptionExists(argv, argv+argc, "-baud")){
        std::string baudString = getCmdOption(argv, argv + argc, "-baud");
        baud = std::stoi(baudString);
    }


    std::cout << "\nStarting service...\n";
    telemetryService = new TelemetryService(uri, serialDevice, baud);
    telemetryService->Initialize(&ms_emitMessage, &ws_emitMessage);
    telemetryService->RunService();

    return 0;
}