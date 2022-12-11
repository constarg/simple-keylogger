# simple linux keylogger

[![linux-keylogger Build](https://github.com/EmbeddedCat/simple-keylogger/actions/workflows/cmake.yml/badge.svg?branch=main)](https://github.com/EmbeddedCat/simple-keylogger/actions/workflows/cmake.yml)


[![GitHub license](https://img.shields.io/badge/Licence-MIT-brightgreen)](https://github.com/EmbeddedCat/simple-keylogger/blob/main/LICENSE)
[![Release](https://img.shields.io/badge/Release-1.0-brightgreen)](https://github.com/EmbeddedCat/simple-keylogger/releases/tag/v1.0)
[![Purpose](https://img.shields.io/badge/Purpose-Educational-brightgreen)](https://github.com/EmbeddedCat/simple-keylogger/releases/tag/v1.0)
[![OS](https://img.shields.io/badge/OS-Linux-brightgreen)](https://github.com/EmbeddedCat/simple-keylogger)

# What is a keylogger?

Keystroke logging, often referred to as keylogging or keyboard capturing, is the action of recording (logging) the keys struck on a keyboard, typically covertly, so that a person using the keyboard is unaware that their actions are being monitored. Data can then be retrieved by the person operating the logging program. A keystroke recorder or keylogger can be either software or hardware.

While the programs themselves are legal, with many designed to allow employers to oversee the use of their computers, keyloggers are most often used for stealing passwords and other confidential information.

Keylogging can also be used to study keystroke dynamics or human-computer interaction. Numerous keylogging methods exist, ranging from hardware and software-based approaches to acoustic cryptanalysis.<br><br>
Source: `https://en.wikipedia.org/wiki/Keystroke_logging`

# Disclaimer

As a student who likes the field of security, I always wondered how one could programmatically implement such a tool. The tool is a simple, well commented project for anyone who is purely interested in the educational part only!! like me. Therefore, I bear no responsibility, in case someone, somehow, uses this tool, for any malicious purpose. I hope you like my project and found it interesting. Also this program does not send any of the keystrokes over a network, it only stores them in a local file.

# GitHub Policy on active-malware-or-exploits

https://docs.github.com/en/site-policy/acceptable-use-policies/github-active-malware-or-exploits

# Running example

<img src="https://user-images.githubusercontent.com/38585824/150214487-0cd08cfc-3b26-4975-8669-233acf9ee59b.gif" width="70%" height="50%">


# Installation requirements
The following packages must be installed.<br>
```
sudo apt-get install build-essential cmake git gcc
```

# Download & Build

First download the program from GitHub and go to the simple-keylogger folder.

```
% git clone https://github.com/EmbeddedCat/simple-keylogger.git
% cd simple-keylogger/
```

After installation the program must be built. In order to build the program, the following instructions must be
followed.<br>

```
% mkdir build
% cd build/
% cmake ../
% make
```

`
The cmake should be from version 3.20 and above, if you do not have this version please go to the following site and download the latest version:
https://cmake.org/download/
`

After this the program will be installed and ready to run.

# Features
- [x] Auto detect keyboard.
- [x] Work with multiple keyboards.
- [x] Detect any new keyboard on air.

# Compitible OS
- [x] Linux

# Log loction
Captured keystrokes stored at `~/.local/share/keylogger/logs/`
