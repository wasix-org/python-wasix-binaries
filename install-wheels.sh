#! /bin/bash

set -euxo pipefail

find ./wheels -name *.whl | \
  xargs -I {} unzip -o {} -d "${PYTHONHOME:=../cpython-install/cpython}/lib/python3.13/site-packages"