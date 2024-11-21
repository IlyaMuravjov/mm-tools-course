#!/bin/bash

cd "$(dirname "$0")"

make
echo "Starting experiments"

./overcommit 100
