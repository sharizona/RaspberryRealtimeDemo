#!/bin/bash
# Quick start script for Docker environment

set -e

echo "===========================================  "
echo "Real-Time Systems Demo - Docker Launcher"
echo "==========================================="
echo ""

# Check if Docker is installed
if ! command -v docker &> /dev/null; then
    echo "❌ Docker is not installed!"
    echo "Please install Docker Desktop: https://www.docker.com/products/docker-desktop"
    exit 1
fi

# Check if Docker is running
if ! docker info &> /dev/null; then
    echo "❌ Docker is not running!"
    echo "Please start Docker Desktop and try again."
    exit 1
fi

echo "✅ Docker is ready"
echo ""

# Function to show menu
show_menu() {
    echo "What would you like to do?"
    echo ""
    echo "  1) Build and start container"
    echo "  2) Enter running container (interactive shell)"
    echo "  3) Run basic pthread example (SCHED_OTHER)"
    echo "  4) Run real-time pthread example (SCHED_FIFO)"
    echo "  5) Run scheduling comparison"
    echo "  6) Run priority inversion demo"
    echo "  7) Stop container"
    echo "  8) Clean up (remove container and image)"
    echo "  9) Exit"
    echo ""
    read -p "Enter choice [1-9]: " choice
}

# Build and start
build_start() {
    echo ""
    echo "Building Docker image and starting container..."
    docker-compose up -d --build
    echo ""
    echo "✅ Container is running!"
    echo "   Use option 2 to enter the container"
}

# Enter container
enter_container() {
    echo ""
    echo "Entering container..."
    docker-compose exec realtime-demo /bin/bash
}

# Run example
run_example() {
    local example_path=$1
    local example_name=$2
    echo ""
    echo "Running: $example_name"
    echo "----------------------------------------"
    # Ensure it's compiled inside the container before running
    docker-compose exec realtime-demo bash -c "cd $example_path && make clean && make && ./$example_name"
    echo ""
    read -p "Press Enter to continue..."
}

# Stop container
stop_container() {
    echo ""
    echo "Stopping container..."
    docker-compose down
    echo "✅ Container stopped"
}

# Clean up
cleanup() {
    echo ""
    read -p "This will remove the container and image. Continue? (y/N): " confirm
    if [[ $confirm == [yY] ]]; then
        echo "Cleaning up..."
        docker-compose down
        docker rmi raspberry-realtime-demo 2>/dev/null || true
        echo "✅ Cleanup complete"
    fi
}

# Main loop
while true; do
    show_menu
    case $choice in
        1)
            build_start
            ;;
        2)
            enter_container
            ;;
        3)
            run_example "examples/01_basic_pthread" "basic_pthread"
            ;;
        4)
            run_example "examples/02_realtime_pthread" "realtime_pthread"
            ;;
        5)
            run_example "examples/03_scheduling_comparison" "sched_comparison"
            ;;
        6)
            run_example "examples/04_priority_inversion" "priority_inversion"
            ;;
        7)
            stop_container
            ;;
        8)
            cleanup
            ;;
        9)
            echo ""
            echo "Goodbye!"
            exit 0
            ;;
        *)
            echo "Invalid choice. Please try again."
            ;;
    esac
    echo ""
done
