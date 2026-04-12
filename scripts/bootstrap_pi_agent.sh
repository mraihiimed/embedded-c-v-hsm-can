#!/bin/bash
set -e

echo "=== Jenkins Raspberry Pi Agent Bootstrap ==="

# Update system
sudo apt update
sudo apt upgrade -y

echo "=== Installing core build tools ==="
sudo apt install -y \
    build-essential \
    gcc \
    g++ \
    make \
    git \
    python3 \
    python3-pip \
    cppcheck \
    lcov \
#    genhtml \
    libncurses5-dev \
    libncursesw5-dev

echo "=== Verifying tools ==="
for tool in gcc make cppcheck python3 lcov genhtml git; do
    if command -v $tool >/dev/null; then
        echo "✔ $tool found: $(command -v $tool)"
    else
        echo "❌ $tool NOT FOUND"
        exit 1
    fi
done

echo "=== Creating CI directories ==="
mkdir -p ~/jenkins-agent
mkdir -p ~/jenkins-agent/workspace

echo "=== Bootstrap complete ==="
echo "Your Raspberry Pi is now ready to act as a Jenkins CI agent."
