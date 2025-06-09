#! /bin/bash

set -euxo pipefail

find ./wheels -name *.whl | \
  xargs -I {} unzip -o {} -d ../cpython-install/cpython/lib/python3.13/site-packages