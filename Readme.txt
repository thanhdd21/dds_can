# DDS topic:
All DDS topics are inside src/idl folder (copied from Fcam source code)
Example to add subscribe a topic:
1. Generate source file from .idl file:
    `~/Fast-DDS/src/fastddsgen/scripts/fastddsgen -typeros2 -replace -I <include path> <filename>.idl`
    Some idl file using @verbatim to comment, that make the generate failed, just comment out them using C style //

2. Create a callback function inside dds_callbacks.[cpp,h], this function will be called everytime a topic arrived, take reference
    from current file content.
    Note: The can_buffer maybe unused for now, can remove it later

3. Add g_bridge->addSubscriber in main(), now we have flow dds_subscriber -> callback_function worked. The additional
execution for callback funtion result should be add in BridgeManager::run() funtion - here is where we send CAN to the world outside
<let create a singleton instance for sharing data between the callback_function and BridgeManager::run(), use mutex or any things to 
avoid race condition between two thread>

# CAN signal:
To define CAN messages and signals, let use MACRO inside `can_msg.h` file, this already defines method to pack/unoack signal inside a message

Then create function to pack/unpack signal based on what received from dds


