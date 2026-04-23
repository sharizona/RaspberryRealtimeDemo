# Dockerfile for Real-Time Systems Demo
# Provides Linux environment with full POSIX real-time scheduling support

FROM ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build tools and dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    make \
    gdb \
    valgrind \
    strace \
    htop \
    vim \
    nano \
    && rm -rf /var/lib/apt/lists/*

# Create working directory
WORKDIR /app

# Copy project files
COPY examples/ /app/examples/
COPY README.md /app/

# Build all examples
RUN cd /app/examples/01_basic_pthread && make && \
    cd /app/examples/02_realtime_pthread && make && \
    cd /app/examples/03_scheduling_comparison && make && \
    cd /app/examples/04_priority_inversion && make

# Create a helpful startup script
RUN echo '#!/bin/bash\n\
echo "==========================================="\n\
echo "Real-Time Systems Demo - Docker Container"\n\
echo "==========================================="\n\
echo ""\n\
echo "Available examples:"\n\
echo "  1. Basic Pthread (SCHED_OTHER):        cd examples/01_basic_pthread && ./basic_pthread"\n\
echo "  2. Real-Time Pthread (SCHED_FIFO):     cd examples/02_realtime_pthread && ./realtime_pthread"\n\
echo "  3. Scheduling Comparison:              cd examples/03_scheduling_comparison && ./sched_comparison"\n\
echo "  4. Priority Inversion Demo:            cd examples/04_priority_inversion && ./priority_inversion"\n\
echo ""\n\
echo "Note: Examples 2-4 demonstrate SCHED_FIFO and need --privileged mode"\n\
echo ""\n\
echo "Quick start:"\n\
echo "  make -C examples/01_basic_pthread run"\n\
echo "  make -C examples/02_realtime_pthread run"\n\
echo ""\n\
exec bash' > /app/start.sh && chmod +x /app/start.sh

# Set default command
CMD ["/app/start.sh"]
