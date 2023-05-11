# The RAT (Remote Access Trojan)

This repository contains the code for a botnet and Remote Access Shell in Python and C++. 

**Disclaimer: This code is intended for educational and research purposes only. Please do not use it for any malicious purposes. The author and contributors of this code are not responsible for any damage caused by its use.**

## Table of Contents
- [Overview](#overview)
- [Botnet Features](#botnet-features)
- [Client Features](#client-features)
- [Usage](#usage)
- [Contributing](#contributing)

## Overview
The botnet and client implementation consists of two parts: a Python script that serves as the botnet and a C++ program that can be compiled to an exe to be ran on target computers (with consent) to be a part of the botnet. The Python script listens for incoming connections from computers that run the C++ program, and the RAT connects to the Python script and waits for commands to execute.

## Botnet Features
The botnet implemented in the Python script has the following features:
- Listens for incoming connections from multiple clients
- Executes commands on all connected clients simultaneously
- Sends command output back to the botnet for viewing
- Implements basic command parsing and execution
- The botnet can focus on a specific client to send commands to, allowing for more targeted actions. This feature can be useful in situations where it is necessary to execute a specific command on a particular client without affecting others.

## RAT Features
The client implemented in the C++ program has the following features:
- Connects to the botnet and waits for commands
- Sends command output back to the botnet for viewing
- Supports basic commands such as `cd`, `ls`, and `pwd`
- Implements persistence by automatically running on startup, ensuring it remains connected to the botnet even after a reboot. Further, he client also implements persistence by creating a new thread that will attempt to reconnect to the botnet if the connection is lost. This ensures that the client will continue to receive commands even if the connection is temporarily disrupted.
- - Supports the following basic commands:
  - `cd` - change directory
  - `whoami` - display the current user
  - `hostname` - display the hostname of the machine
  - `nano` - open a file in the Nano text editor
  - `exec` - execute a command
  - `rm` - remove a file or directory
  - `pwd` - display the current working directory
  - `ls` - list files and directories in the current directory

## Usage
To use this implementation, follow these steps:
1. Clone this repository: `git clone https://github.com/yourusername/botnet.git`
2. Start the Python script: `python botnet.py`
3. Compile and run the C++ program on the client machines: `g++ -o WindowsUpdater client.cpp -lgdi32 -lws2_32 -static-libgcc -static-libstdc++ -mwindow`
4. Once the client is connected to the botnet, execute commands by typing them into the Python script terminal

**Note: Be sure to change the IP address in the `client.cpp` file to match the IP address of the machine running the Python script.**

## Contributing
Contributions to this project are welcome. To contribute, follow these steps:
1. Fork this repository
2. Create a new branch: `git checkout -b my-new-branch`
3. Make changes and commit: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-branch`
5. Submit a pull request
