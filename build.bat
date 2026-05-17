@echo off
g++ backend/main.cpp -std=c++17 -o server -D_WIN32_WINNT=0x0A00 -lws2_32 -lwininet -static-libgcc -static-libstdc++
echo Successfully built!
echo Starting MatchKernel server...
start server.exe
ping -n 3 127.0.0.1 >nul
start "" "frontend/index.html"