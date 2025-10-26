# Communication Commands
The following sections indicate the types of commands that can be sent to the Boat Control Panel, with examples and purpose.

## Configuration Commands
These are commands used to configure the system settings and can only be sent from a computer, they are not used for internal communication.

| Command | Example | Purpose |
|---|---|---|
| `C0` — Save settings | `C0` | Persist current in-memory config to EEPROM. Responds `SAVED` on success; error `EEPROM commit failed` on failure. No params. |
| `C1` — Get settings | `C1` | Request full config. Device replies with multiple commands: `C2 <boatName>`, `C3 <idx>:<name>` for each relay, `C4 <slot>:<relay>` for each home-slot mapping, then `OK`. No params. |
| `C2` — Reset settings | `C2` | Request full reset of all config settings. No params. |
| `C3` — Rename boat | `C3:Sea Wolf` or `C3:name=SeaWolf` | Set the boat name. Accepts a single token or a `<key>:<value>` pair (value is used if present). Empty name → error. Name is truncated to configured max length. |
| `C4` — Rename relay | `C4:2=Bilge` | Rename a relay. Param format: `<idx>:<name>`. `idx` must be 0..7 (`RELAY_COUNT`). Missing name → error. Name is truncated to relay name length. |
| `C5` — Map home button | `C5:1=3` (map) — `C5:1=255` (unmap) | Map a home-page slot to a relay. Param format: `<slot>:<relay>`. `button` must be 0..3 (`HOME_BUTTONS`). `relay` must be 0..7 or `255` to clear/unmap. |
| `C6` — Map home button color | `C6:0=4` (map button 1 to Red when activated) — `C6:1=255` (unmap colors) | Map a home-page button to a color when activated (on). Param format: `<slot>:<relay>`. `slot` must be 0..3 (`ConfigManager::HOME_SLOTS`). `relay` must be 0..7 or `255` to clear/unmap. |


Common error responses you may see: `Missing param`, `Missing params`, `Missing name`, `Empty name`, `Index out of range`, `Slot out of range`, `Relay out of range (or 255 to clear)`, `EEPROM commit failed`, `Unknown config command`.


## Acknowldement Commands
These commands are used in response to receiving a command.
| Command | Example | Purpose |
|---|---|---|
| `ACK` — Acknowledement | `ACK:C4=Index out of range` | Indicates that the C4 command was processed and the index specified was out of range. |
| `ACK` — Acknowledement | `ACK:C4=ok` | Indicates that the C4 command was processed successfully. |

## Relay Control Commands
These commands are used to control the relays on the Boat Control Panel. Commands can be sent from a computer or generated internally by the Boat Control Panel.
| Command | Example | Purpose |
|---|---|---|
| `R0` — Turn All Off | `R0` | Indicates that the C4 command was processed and the index specified was out of range. |
| `R1` — Turn All On | `R1` | Indicates that the C4 command was processed successfully. |
| `R2` — Retrieve States | `R2` | Retrieve the state of all relays. |
| `R3` — Set Relay State | `R3:3=1` (turn on relay 3) — `R3:5=0` (turn off relay 5) | Set the state of a specific relay. Param format: `<idx>=<state>`. `idx` must be 0..7 (`RELAY_COUNT`). `state` must be `0` (off) or `1` (on). |
| `R4` — Relay State Get | `R4:3` (retrieves status of relay 3) — `R4:5` (returns status of relay 5). Param format: `<idx>`. `idx` must be 0..7 (`RELAY_COUNT`). |
