#!/bin/bash

find $PWD -name "*.yaml" | xargs ../scripts/y2j.py
