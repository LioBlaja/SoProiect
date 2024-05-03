#!/bin/bash

#chmod +x LioBlaja.sh -- need for permissions

#if [ "$1" = 'config' ]; then
#    current_dir=$(pwd)
#    export PATH="$PATH:$current_dir"
#    echo "Compilation successful"
if [ "$1" = 'init' ] && [ -n "$2" ]; then
    # Compile setup program
    gcc setup.c snapshot.c -o setup
    
    # Check if setup compilation was successful
    if [ $? -eq 0 ]; then
        # Wait until setup executable is created
        while [ ! -f setup ]; do
            sleep 1
        done

        # Navigate to directory and create/reset snapshot.txt file
        cd "$2"
        touch snapshot.txt
        echo -n "" > snapshot.txt
        cd ..

        # Execute setup program with provided directory
        ./setup "$1" "$2"

    else
        echo "Compilation failed"
        exit 1
    fi
elif [ "$1" = 'status' ]; then
    # Execute setup program with 'status' argument
    ./setup "$1" 
else
    echo "Failed"
fi
