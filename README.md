# simple linux keylogger

[![linux-keylogger Build](https://github.com/EmbeddedCat/linux-keylogger/actions/workflows/cmake.yml/badge.svg?branch=main)](https://github.com/EmbeddedCat/linux-keylogger/actions/workflows/cmake.yml)

[![GitHub license](https://img.shields.io/badge/Licence-MIT-brightgreen)](https://github.com/EmbeddedCat/simple-reverse-shell/blob/main/LICENSE)
[![Release](https://img.shields.io/badge/Release-1.0-brightgreen)](https://github.com/EmbeddedCat/simple-reverse-shell/releases/tag/v1.0)


# Disclaimer

As a student who likes the field of security, I always wondered how one could programmatically implement such a tool. The tool is a simple, well commented project for anyone who is purely interested in the educational part only!! like me. Therefore, I bear no responsibility, in case someone, somehow, uses this tool, for any malicious purpose. I hope you like my project and found it interesting, keep learning. Also this program does not send any of the keystrokes over a network, it only stores them in a local file.

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
