
#include <iostream>
#include <map>
#include <functional>
#include "VM.cpp"

int Test()
{
    return 2;
}

int main()
{
    std::vector<uint8_t> ByteCode = { 0x0E, 0x01, 0x03,0x00,0x00,0x00,0x03, 0x01, 0x04 };

    std::map<std::string, Function> Func = {};

    Func["Test"] = Test;

    VirtualMachine vm(ByteCode, Func);
    vm.execute();

    return 0;
}
