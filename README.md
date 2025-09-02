# FTP Module

## Overview
This C++ module implements a File Transfer Protocol (FTP) handler for a network application, operating at the Application Layer (OSI Layer 7). It processes FTP control and data connections, handling commands and responses, and provides integration with a Qt-based graphical interface for settings on Windows platforms. The module is designed to work within a larger network analysis or monitoring system.

## Features
- **Protocol Handling**: Processes FTP control commands (e.g., `USER`, `PASS`, `PORT`, `RETR`, `STOR`, `CWD`) and responses (e.g., `220`, `227`, `250`, `257`).
- **Connection Management**: Manages both control and data connections using `FTP_ControlConnection` and `FTP_DataConnection` classes.
- **Qt Integration**: Supports a graphical settings interface using Qt, with platform-specific handling for Windows.
- **Session Tracking**: Maintains active FTP sessions in `control_connections` and `data_connections` containers.
- **Localization**: Uses `QObject::tr` for translatable strings (e.g., module name, description).
- **Resource Management**: Initializes and frees resources associated with FTP sessions.
