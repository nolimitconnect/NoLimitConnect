#!/bin/bash
# setup_nolimitconnect_docs.sh
# Script to create nolimitconnect-docs folder structure and touch Markdown files

# Base folder
BASE_DIR="nolimitconnect-docs"

# Subfolders
DEVELOPER_DOCS="$BASE_DIR/developer-docs"
USER_DOCS="$BASE_DIR/user-docs"
HOWTO_DOCS="$BASE_DIR/howto-docs"
ASSETS_DIR="$BASE_DIR/assets"

# Create folder structure
mkdir -p "$DEVELOPER_DOCS"
mkdir -p "$USER_DOCS"
mkdir -p "$HOWTO_DOCS"
mkdir -p "$ASSETS_DIR"

# Developer docs files
touch "$DEVELOPER_DOCS/Setup_For_Docker_Image_Build_Environment.md"
touch "$DEVELOPER_DOCS/Setup_Ubuntu_Build_Environment.md"
touch "$DEVELOPER_DOCS/Setup_Windows_Build_Environment.md"
touch "$DEVELOPER_DOCS/Compile_NoLimitConnect_Docker.md"
touch "$DEVELOPER_DOCS/Compile_NoLimitConnect_Flatpak.md"
touch "$DEVELOPER_DOCS/Compile_NoLimitConnect_QCreator.md"
touch "$DEVELOPER_DOCS/Compile_NoLimitConnect_VisualStudio.md"

# User docs placeholder
touch "$USER_DOCS/README.md"

# How-to docs placeholder
touch "$HOWTO_DOCS/README.md"

# Copy or touch icon placeholder
touch "$ASSETS_DIR/nlc-icon.svg"

echo "Folder structure and Markdown files created successfully!"
