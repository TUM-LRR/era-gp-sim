@echo off
for /r %%F in (*.yaml) do call python ..\scripts\y2j.py "%%F"