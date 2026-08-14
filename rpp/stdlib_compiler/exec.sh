set -e
echo "Compiling without pie..."
g++ -no-pie main.o -o main
echo "Assembling..."
nasm -f elf64 main.asm -o main.o
echo "Executing..."
./main
