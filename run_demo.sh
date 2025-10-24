#!/bin/bash

# AIMLAB VR Data Streamer - Demo Script
# This script demonstrates communication between C++ and Python implementations

echo "========================================="
echo "  AIMLAB VR Data Streamer Demo"
echo "========================================="
echo ""

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check prerequisites
echo "Checking prerequisites..."

if ! command_exists g++; then
    echo "Error: g++ compiler not found. Please install build-essential."
    exit 1
fi

if ! command_exists python3; then
    echo "Error: Python 3 not found. Please install Python 3.7 or later."
    exit 1
fi

echo "✓ All prerequisites met"
echo ""

# Compile C++ application
echo "Compiling C++ application..."
if g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network 2>/dev/null; then
    echo "✓ C++ application compiled successfully"
else
    echo "✗ Failed to compile C++ application"
    echo "  Trying with explicit ws2_32 library (Windows)..."
    if g++ -std=c++17 -pthread aimlab_network_cpp.cpp -o aimlab_network -lws2_32 2>/dev/null; then
        echo "✓ C++ application compiled successfully (Windows)"
    else
        echo "✗ Failed to compile C++ application"
        exit 1
    fi
fi
echo ""

# Function to run a node in background
run_node() {
    local node_type=$1
    local log_file=$2
    
    echo "Starting $node_type node..."
    
    if [ "$node_type" = "cpp" ]; then
        ./aimlab_network > "$log_file" 2>&1 &
    else
        python3 aimlab_network_python.py > "$log_file" 2>&1 &
    fi
    
    local pid=$!
    echo "  PID: $pid"
    echo "  Log: $log_file"
    return $pid
}

# Create log directory
mkdir -p logs

# Start both nodes
echo "Starting network nodes..."
echo ""

# Start C++ node
run_node "cpp" "logs/cpp_node.log"
CPP_PID=$!

# Wait a moment for C++ node to initialize
sleep 2

# Start Python node
run_node "python" "logs/python_node.log"
PYTHON_PID=$!

# Wait for nodes to connect
echo ""
echo "Waiting for nodes to discover and connect..."
sleep 3

echo ""
echo "========================================="
echo "  Demo Running"
echo "========================================="
echo ""
echo "Both nodes are now running and should be connected."
echo ""
echo "You can:"
echo "1. Check the logs in the 'logs' directory"
echo "2. Send test messages using the test_communication.py script"
echo "3. Monitor network traffic with tcpdump/Wireshark on port 45000-45001"
echo ""
echo "Press Ctrl+C to stop the demo"
echo ""

# Function to cleanup on exit
cleanup() {
    echo ""
    echo "Stopping nodes..."
    kill $CPP_PID 2>/dev/null
    kill $PYTHON_PID 2>/dev/null
    echo "Demo stopped"
    exit 0
}

# Set up signal handler
trap cleanup SIGINT SIGTERM

# Keep script running
while true; do
    # Check if processes are still running
    if ! kill -0 $CPP_PID 2>/dev/null; then
        echo "C++ node has stopped unexpectedly"
        cleanup
    fi
    
    if ! kill -0 $PYTHON_PID 2>/dev/null; then
        echo "Python node has stopped unexpectedly"
        cleanup
    fi
    
    sleep 5
done
