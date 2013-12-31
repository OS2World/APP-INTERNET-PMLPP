@echo off
REM
REM Make it!
REM
wmake

REM
REM Optimize and pack it!
REM
lxopt /prep pmlpp.exe
pmlpp
lxopt /arrange pmlpp.exe

repack /exepack:2 pmlpp.exe

REM
REM if not using LXOPT... remember to comment out
REM 'op int' in the makefile &| pmlpp.wlk!
REM
