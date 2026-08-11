set -e
echo "Compiling..."
g++ -std=c++20 main.cpp -o rpp-test
echo "Running..."
./rpp-test


