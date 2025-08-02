#!/usr/bin/env bash

set -e

TARGET_NAME="peek"
C_FILE="peek.c"
BUILD_FILE="./$TARGET_NAME"
DEFAULT_INSTALL_DIR="/usr/local/bin"

# === Color helpers ===
info()    { echo -e "\033[1;34m[INFO]\033[0m $1"; }
success() { echo -e "\033[1;32m[SUCCESS]\033[0m $1"; }
error()   { echo -e "\033[1;31m[ERROR]\033[0m $1"; exit 1; }

# === Step 1: Check if source file exists ===
if [ ! -f "$C_FILE" ]; then
    error "File '$C_FILE' not found. Please run this script from the project root."
fi

# === Step 2: Check for compiler ===
if command -v gcc &> /dev/null; then
    COMPILER="gcc"
elif command -v clang &> /dev/null; then
    COMPILER="clang"
else
    error "Neither gcc nor clang was found. Please install a C compiler first."
fi

info "Using compiler: $COMPILER"

# === Step 3: Compile ===
info "Compiling $C_FILE..."
$COMPILER "$C_FILE" -o "$BUILD_FILE"
chmod +x "$BUILD_FILE"

# === Step 4: Ask for install path ===
echo
read -p "Enter install path [default: $DEFAULT_INSTALL_DIR]: " USER_INSTALL_DIR
INSTALL_DIR="${USER_INSTALL_DIR:-$DEFAULT_INSTALL_DIR}"

# Check if dir exists
if [ ! -d "$INSTALL_DIR" ]; then
    error "Directory '$INSTALL_DIR' does not exist."
fi

# === Step 5: Move binary ===
DEST="$INSTALL_DIR/$TARGET_NAME"

# Check for name conflict
if command -v "$TARGET_NAME" &> /dev/null && [ "$(command -v $TARGET_NAME)" != "$DEST" ]; then
    info "Another command named '$TARGET_NAME' exists at: $(command -v $TARGET_NAME)"
    read -p "Overwrite it anyway? [y/N]: " confirm
    [[ "$confirm" != "y" && "$confirm" != "Y" ]] && error "Installation aborted."
fi

info "Installing '$TARGET_NAME' to $INSTALL_DIR (requires sudo)..."
sudo mv "$BUILD_FILE" "$DEST"
sudo chmod +x "$DEST"

# === Step 6: Done ===
success "'$TARGET_NAME' installed successfully at $DEST!"
echo "Run '$TARGET_NAME -h' to get started 🚀"
