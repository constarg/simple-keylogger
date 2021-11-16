# Introduction
This keylogger is a project that was made for educational purposes and does not send any key to the internet. What it does is take the keys and store them in a local file. I wanted to make such a keylogger for a long time to learn how a program could take keystrokes and use them either for good or for bad.

# Download & Build

First download the program from GitHub and go to the File-Sorter-Core folder.

```
% git clone https://github.com/rounnus/linux-keylogger.git
% cd linux-keylogger/
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
The cmake should be from version 3.19 and above, if you do not have this version please go to the following site and download the latest version:
https://cmake.org/download/
`

After this the program will be installed and ready to run.

# Features
- [x] Auto detect keyboard
- [x] Work with multiple keyboards.
- [x] Detect any new keyboard on air.
