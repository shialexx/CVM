
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
    std::vector<uint8_t> bytecode = { 0x0D, 0x01 };

    std::map<std::string, Function> func = {};

    func["Test"] = Test;

    VirtualMachine vm(bytecode, func);
    vm.execute();

    return 0;
}
