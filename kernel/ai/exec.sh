#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"

echo "== Compiling optimized trainer =="
g++ -std=c++20 -O3 -march=native -flto -Wall \
    -DHOST_BUILD \
    main.cpp \
    -o train

echo "== Compiling debug evaluator =="
g++ -std=c++20 -O0 -g -fsanitize=address -Wall \
    -DHOST_BUILD \
    evaluate.cpp \
    -o evaluate

echo "== Compiling optimized model to header =="
g++ -std=c++20 -O3 -march=native -flto -Wall \
    -DHOST_BUILD \
    model_to_header.cpp \
    -o model_to_header

echo "== Build complete! =="
echo "Train:    ./train"
echo "Evaluate: ./evaluate"
echo "Save weights to model: ./model_to_header rock_ai.model rock_ai_weights.hpp [array_name]"


