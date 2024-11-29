#!/bin/bash

cd "$(dirname "$0")"

make
./run_experiments_without_make.sh
