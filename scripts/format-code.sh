#!/bin/bash

set -euo pipefail

cd "$(dirname "$(readlink -f "$0")")"/..


echo "Using clang format to format files..."
git ls-files | grep -P "\.[c|h|t](pp)?$" | xargs clang-format --style=file -i
