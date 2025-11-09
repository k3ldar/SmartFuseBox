# Communication Commands
The following sections indicate the types of commands that can be sent to the Boat Control Panel, with examples and purpose.

## System Commands
These are commands used to configure the system settings and can only be sent from a computer, they are not used for internal communication.

| Command | Example | Purpose |
|---|---|---|
| `F0` — Heart beat | `F0` | Send at rated intervals, if no ACK received indicates there is no connection available between control panel and fuse box. No params. |
| `F1` — System Initialized | `F1` | Sent by the system when initialization is complete to signal readiness. No params. Used to notify connected devices or software that the control panel is ready for operation. |

## Configuration Commands
These are commands used to configure the system settings and can only be sent from a computer, they are not used for internal communication.

| Command | Example | Purpose |
|---|---|---|
| `C0` — Save settings | `C0` | Persist current in-memory config to EEPROM. Responds `SAVED` on success; error `EEPROM commit failed` on failure. No params. |
| `C1` — Get settings | `C1` | Request full config. Device replies with multiple commands: `C3 <boatName>`, `C4 <idx>:<shortName\|longName>` for each relay, `C5 <slot>:<relay>` for each home-slot mapping, then `OK`. No params. |
| `C2` — Reset settings | `C2` | Request full reset of all config settings. No params. |
| `C3` — Rename boat | `C3:Sea Wolf` or `C3:name=SeaWolf` | Set the boat name. Accepts a single token or a `<key>:<value>` pair (value is used if present). Empty name → error. Name is truncated to configured max length. |
| `C4` — Rename relay | `C4:2=Bilge` or `C4:2=Bilge\|Bilge Pump` | Rename a relay. Param format: `<idx>=<shortName>` or `<idx>=<shortName\|longName>`. `idx` must be 0..7 (`RELAY_COUNT`). If no pipe character or long name provided, the short name is used for both. Short name is truncated to 5 chars (used on home page), long name is truncated to 20 chars (used on buttons page). Missing name → error. |
| `C5` — Map home button | `C5:1=3` (map) — `C5:1=255` (unmap) | Map a home-page slot to a relay. Param format: `<slot>:<relay>`. `button` must be 0..3 (`HOME_BUTTONS`). `relay` must be 0..7 or `255` to clear/unmap. |
| `C6` — Map home button color | `C6:0=4` (map button 1 to Red when activated) — `C6:1=255` (unmap colors) | Map a home-page button to a color when activated (on). Param format: `<slot>:<relay>`. `slot` must be 0..3 (`ConfigManager::HOME_SLOTS`). `relay` must be 0..7 or `255` to clear/unmap. |


Common error responses you may see: `Missing param`, `Missing params`, `Missing name`, `Empty name`, `Index out of range`, `Slot out of range`, `Relay out of range (or 255 to clear)`, `EEPROM commit failed`, `Unknown config command`.


## Acknowledgement Commands
These commands are used in response to receiving a command.

| Command | Example | Purpose |
|---|---|---|
| `ACK` — Acknowledgement | `ACK:C4=Index out of range` | Indicates that the C4 command was processed and the index specified was out of range. |
| `ACK` — Acknowledgement | `ACK:C4=ok` | Indicates that the C4 command was processed successfully. |

## Relay Control Commands
These commands are used to control the relays on the Boat Control Panel. Commands can be sent from a computer or generated internally by the Boat Control Panel.

| Command | Example | Purpose |
|---|---|---|
| `R0` — Turn All Off | `R0` | Indicates that the C4 command was processed and the index specified was out of range. |
| `R1` — Turn All On | `R1` | Indicates that the C4 command was processed successfully. |
| `R2` — Retrieve States | `R2` | Retrieve the state of all relays. |
| `R3` — Relay State Set | `R3:3=1` (turn on relay 3) — `R3:5=0` (turn off relay 5) | Set the state of a specific relay. Param format: `<idx>=<state>`. `idx` must be 0..7 (`RELAY_COUNT`). `state` must be `0` (off) or `1` (on). |
| `R4` — Relay State Get | `R4:3` (retrieves status of relay 3) — `R4:5` (returns status of relay 5). Param format: `<idx>`. `idx` must be 0..7 (`RELAY_COUNT`). |

## Sensor Commands
These commands are used to send sensor data from the Boat Control Panel to a computer.

| Command | Example | Purpose |
|---|---|---|
| `S0` — Temperature | `S0:v=72.5` | Send temperature sensor data. Param format: `<sensor>=<value>`. |
| `S1` — Humidity | `S1:v=55.2` | Send humidity sensor data. Param format: `<sensor>=<value>`. |
| `S2` — Bearing | `S2:v=128` | Send bearing sensor data. Param format: `<sensor>=<value>`. |
| `S3` — Direction | `S3:v=NNW` | Send direction sensor data. Param format: `<sensor>=<value>`. |
| `S4` — Speed | `S4:v=3.4` | Send speed sensor data. Param format: `<sensor>=<value>`. |
| `S5` — Compass Temp | `S5:v=23.4` | Send compass temperature sensor data. Param format: `<sensor>=<value>`. |
| `S6` — Water Level | `S6:v=3.4` | Send water level sensor data. Param format: `<sensor>=<value>`. |
| `S7` — Water Pump Active  | `S7:v=1` | Send water pump active status. Param format: `<sensor>=<value>`, 0 = off, 1 = on. |
| `S8` — Horn Active  | `S8:v=1` | Send horn active status. Param format: `<sensor>=<value>`, 0 = off, 1 = on. |

## Warning Commands
These commands are used to send warning data from the control panel to link/computer. WarningType corresponds to the enum in WarningManager.h.

| Command | Example | Purpose |
|---|---|---|
| `W0` — Warnings Active | `W0:v=3` | Number of active warnings 3 in this example. Param format: `<value>=<count>`. |
| `W1` — List Warnings | `W1` | Retrieve warning list. Param format: No parameters. |
| `W2` — Warning Status | `W2:0x05=1` | Sends warning status for each warning. Param format: `<WarningType>=<bool>`. |
| `W3` — Clear Warnings | `W3` | Clears all warning data. Param format: No Parameters. |
| `W4` — Warning Set Status | `W4:0x06=1` | Adds or removes a warning to the list of warnings. Param format: `<WarningType>=<bool>`. |
