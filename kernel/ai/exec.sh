#!/usr/bin/env bash
#
# Builds both the trainer and evaluator host binaries.
# Lives in my_os/kernel/ai/ - run it from right there:
#
#   ./train.sh
#
set -e  # stop immediately if any command fails

cd "$(dirname "$0")"  # make sure we're always running from kernel/ai/

echo "== Compiling main.cpp (Trainer) =="
g++ -std=c++20 -O0 -g -fsanitize=address -Wall -DHOST_BUILD main.cpp -o train

echo "== Compiling evaluate.cpp (Evaluator) =="
g++ -std=c++20 -O0 -g -fsanitize=address -Wall -DHOST_BUILD evaluate.cpp -o evaluate

echo "== Build complete! =="
echo "To train the model, run:     ./train"
echo "To evaluate the model, run:  ./evaluate"
