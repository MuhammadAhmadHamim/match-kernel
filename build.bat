@echo off
g++ backend/main.cpp -std=c++17 -o server -D_WIN32_WINNT=0x0A00 -lws2_32 -lwininet -static-libgcc -static-libstdc++
echo Successfully built!