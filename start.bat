@echo off

cd C:\Users\Martin\Desktop\real-time-ws\redis\bin\release\redis-2.8.17
start redis-server.exe

cd C:\Users\Martin\Desktop\real-time-ws\server\bin
set DEBUG=http,app & node www

exit