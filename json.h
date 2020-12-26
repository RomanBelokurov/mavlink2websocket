/* Mavlink packet to JSON object parser.
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

#ifndef JSON_H_
#define JSON_H_

#include "c_library_v2/common/mavlink.h"

#include <iostream>
#include <string>

const mavlink_message_info_t *mavlink_get_message_info_by_id(uint32_t msgid)
{
    static const mavlink_message_info_t mavlink_message_info[] = MAVLINK_MESSAGE_INFO;
    uint32_t low=0, high=sizeof(mavlink_message_info)/sizeof(mavlink_message_info[0]);
            
    while (low < high) 
    {
        uint32_t mid = (low+1+high)/2;
        if (msgid < mavlink_message_info[mid].msgid) 
        {
            high = mid-1;
            continue;
        }

        if (msgid > mavlink_message_info[mid].msgid) 
        {
            low = mid;
            continue;
        }

        low = mid;
        break;
    }

    if (mavlink_message_info[low].msgid == msgid) 
    {
        return &mavlink_message_info[low];
    }
    return NULL;
}

void append_json(std::string &targetStream, std::string key, int value)
{
    targetStream.append("\"");
    targetStream.append(key);
    targetStream.append("\":");
    targetStream.append(std::to_string(value));
}

void append_json(std::string &targetStream, std::string key, float value)
{
    targetStream.append("\"");
    targetStream.append(key);
    targetStream.append("\":");
    targetStream.append(std::to_string(value));
}

void append_json(std::string &targetStream, std::string key, double value)
{
    targetStream.append("\"");
    targetStream.append(key);
    targetStream.append("\":");
    targetStream.append(std::to_string(value));
}

void append_json(std::string &targetStream, std::string key, char value)
{
    targetStream.append("\"");
    targetStream.append(key);
    targetStream.append("\":");
    targetStream.append(std::to_string(value));
}

std::string serialize_json(const mavlink_message_t* msg)
{
    std::string json; 
    json.append("{");
    append_json(json, "MSG_ID", (int)(msg->msgid));
    json.append(",");

    const mavlink_message_info_t* mInfo = mavlink_get_message_info_by_id(msg->msgid);
    for(int f = 0; f < mInfo->num_fields; f++)
    {  
        mavlink_field_info_t field = mInfo->fields[f];
        
        switch(field.type)
        {
            case MAVLINK_TYPE_CHAR:
                append_json(json, (std::string)field.name, _MAV_RETURN_char(msg, field.wire_offset));
                break;
            
            case MAVLINK_TYPE_UINT8_T:
                append_json(json, (std::string)field.name, (int)_MAV_RETURN_uint8_t(msg, field.wire_offset));
                break;                
            
            case MAVLINK_TYPE_INT8_T:            
                append_json(json, (std::string)field.name, (int)_MAV_RETURN_int8_t(msg, field.wire_offset));
                break;

            case MAVLINK_TYPE_UINT16_T:
                append_json(json, (std::string)field.name, (int)_MAV_RETURN_uint16_t(msg, field.wire_offset));
                break;
            
            case MAVLINK_TYPE_INT16_T:
                append_json(json, (std::string)field.name, (int)_MAV_RETURN_int16_t(msg, field.wire_offset));
                break;

            case MAVLINK_TYPE_UINT32_T:
                append_json(json, (std::string)field.name, (int)_MAV_RETURN_uint32_t(msg, field.wire_offset));
                break;

            case MAVLINK_TYPE_INT32_T:
                append_json(json, (std::string)field.name, (int)_MAV_RETURN_int32_t(msg, field.wire_offset));
                break;
            
            case MAVLINK_TYPE_UINT64_T:
                append_json(json, (std::string)field.name, (int)_MAV_RETURN_uint64_t(msg, field.wire_offset));
                break;
            
            case MAVLINK_TYPE_INT64_T:            
                append_json(json, (std::string)field.name, (int)_MAV_RETURN_int64_t(msg, field.wire_offset));
                break;
            
            case MAVLINK_TYPE_FLOAT:
                append_json(json, (std::string)field.name, (float)_MAV_RETURN_float(msg, field.wire_offset));
                break;
            
            case MAVLINK_TYPE_DOUBLE:
                append_json(json, (std::string)field.name, (double)_MAV_RETURN_double(msg, field.wire_offset));
                break;
        }
        
        if(f < mInfo->num_fields - 1)json.append(",");
    }

    json.append("}");    
    return json;
}

#endif