#pragma once
#include "BaseCommandHandler.h"
#include "BoatControlPanelConstants.h"

// internal message handlers
class SystemCommandHandler : public BaseCommandHandler
{
private:
    SerialCommandManager* _commandMgrComputer;
    SerialCommandManager* _commandMgrLink;
public:
    SystemCommandHandler(SerialCommandManager* commandMgrComputer, SerialCommandManager* commandMgrLink);
    ~SystemCommandHandler();
    bool handleCommand(SerialCommandManager* sender, const String command, const StringKeyValue params[], int paramCount) override;

    const String* supportedCommands(size_t& count) const override;
private:
    void broadcast(const String& cmd, const StringKeyValue* param = nullptr);
    uint16_t freeRam();
};
