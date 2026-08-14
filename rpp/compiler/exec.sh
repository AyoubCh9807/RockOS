g++ \
    -std=c++20 \
    -ffreestanding \
    -fno-exceptions \
    -fno-rtti \
    -fno-stack-protector \
    -fno-pie \
    -c main.cpp -o main.o

g++ \
    -std=c++20 \
    -ffreestanding \
    -fno-exceptions \
    -fno-rtti \
    -fno-stack-protector \
    -fno-pie \
    -c ../../kernel/memory/new_delete.cpp -o new_delete.o

g++ \
    -nostdlib \
    -no-pie \
    main.o \
    new_delete.o \
    -o rpp
