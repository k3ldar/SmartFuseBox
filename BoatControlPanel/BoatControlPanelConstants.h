#pragma once

constexpr char SystemHeartbeatCommand[] = "F0";
constexpr char SystemInitialized[] = "F1";
constexpr char SystemFreeMemory[] = "F2";

constexpr char RelayRetrieveStates[] = "R2";
constexpr char RelaySetState[] = "R3";
constexpr char RelayStatusGet[] = "R4";

constexpr char AckSuccess[] = "ok";
constexpr char ValueParamName[] = "v";

constexpr char Equals = '=';
constexpr char Pipe = '|';
