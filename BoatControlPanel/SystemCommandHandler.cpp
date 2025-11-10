// 
// 
// 

#include "SystemCommandHandler.h"

SystemCommandHandler::SystemCommandHandler(SerialCommandManager* commandMgrComputer, SerialCommandManager* commandMgrLink)
    : _commandMgrComputer(commandMgrComputer), _commandMgrLink(commandMgrLink)
{

}

SystemCommandHandler::~SystemCommandHandler()
{

}

const String* SystemCommandHandler::supportedCommands(size_t& count) const
{
    static const String cmds[] = { HeartbeatCommand, SystemInitialized, FreeMemory };
    count = sizeof(cmds) / sizeof(cmds[0]);
    return cmds;
}

bool SystemCommandHandler::handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount)
{
    String cmd = command;
    cmd.trim();

    if (cmd == HeartbeatCommand)
    {
        sendAckOk(sender, cmd);
    }
    else if (cmd == SystemInitialized)
    {
        sendAckOk(sender, cmd);
    }
    else if (cmd == FreeMemory)
    {
        // Example: return a dummy free memory value
        StringKeyValue param = { "mem", String(freeRam()) };
        sendAckOk(sender, cmd, &param);
    }
    else
    {
        sendAckErr(sender, cmd, "Unknown system command");
    }

    return true;
}

uint16_t SystemCommandHandler::freeRam() {
    extern int __heap_start, * __brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void SystemCommandHandler::broadcast(const String& cmd, const StringKeyValue* param)
{
    if (_commandMgrLink != nullptr)
    {
        sendAckOk(_commandMgrLink, cmd, param);
    }

    if (_commandMgrComputer != nullptr)
    {
        sendAckOk(_commandMgrComputer, cmd, param);
    }
}
