#!/bin/bash

cd "$(dirname "$0")"

make
echo "Starting experiments"

for add_newline in false true; do
    for remove_fflush in false true; do
        for redirect_output in false true; do
            args=""
            if [ "$add_newline" = true ]; then
                args="$args -n"
            fi
            if [ "$remove_fflush" = true ]; then
                args="$args -f"
            fi

            echo "==="
            echo "Added '\\n': $add_newline"
            echo "Removed 'fflush': $remove_fflush"
            echo "Redirected to file: $redirect_output"
            echo "Output:"

            if [ "$redirect_output" = true ]; then
                ./io_flush $args > output.txt 2>&1
                cat output.txt
                rm output.txt 
            else
                ./io_flush $args
            fi
            echo "==="

        done
    done
done

