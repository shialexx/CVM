
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
    std::vector<uint8_t> bytecode = { 0x02, 0x04, 0x54, 0x65, 0x73, 0x74, 0x0C, 0x03 };

    std::map<std::string, Function> func = {};

    func["Test"] = Test;

    VirtualMachine vm(bytecode, func);
    vm.execute();

    return 0;
}
