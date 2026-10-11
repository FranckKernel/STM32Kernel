#!/usr/bin/env bash

pkill -f Renode
sleep 0.5
pkill -INT -x picocom
sleep 0.5
clear

sleep 0.1
stty sane
reset
