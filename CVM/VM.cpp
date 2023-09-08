#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <functional>
#include <map>


typedef std::function<int()> Function;

class VirtualMachine {
private:
    std::map<std::string, Function> data{};
    std::vector<uint8_t> memory;
    std::stack<int> intStack;
    std::stack<std::string> stringStack;
    std::map<int, int> registers;
    int programCounter;

public:
    VirtualMachine(const std::vector<uint8_t>& bytecode, std::map<std::string, Function> data)
    {
        memory = bytecode;
        programCounter = 0;
        this->data = data;
        this->registers[1] = 0; //ax
        this->registers[2] = 0; //bx
        this->registers[3] = 0; //cx
        this->registers[4] = 0; //dx
        this->registers[5] = 0; //system register (ex)
    }

    void execute() {
        while (programCounter < memory.size()) {
            uint8_t instruction = memory[programCounter];
            programCounter++;

            switch (instruction) {
            case 0x01:
                pushInt();
                break;
            case 0x02:
                pushStr();
                break;
            case 0x03:
                pushReg();
            case 0x04:
                print();
                break;
            case 0x05:
                input();
                break;
            case 0x06:
                add();
                break;
            case 0x07:
                sub();
                break;
            case 0x08:
                mul();
                break;
            case 0x09:
                cmp();
                break;
            case 0x0A:
                je();
                break;
            case 0x0B:
                jne();
                break;
            case 0x0C:
                jmp();
                break;
            case 0x0D:
                callBack();
                break;
            case 0x0E:
                moveInt();
                break;
            case 0x0F:
                moveReg();
                break;
            case 0x10:
                pop();
                break;
            case 0x11:
                popReg();
                break;
            default:
                std::cerr << "Invalid Instruction: " << instruction << std::endl;
                return;
            }
        }
    }

private:
    void pushInt() {
        int value = readInt();
        intStack.push(value);
    }

    void pushStr() {
        std::string value = readString();
        stringStack.push(value);
    }

    void pushReg() {
        uint8_t r1 = memory[programCounter];
        programCounter++;
        intStack.push(registers[r1]);
    }

    void pop()
    {
        if (!intStack.empty()) {
            intStack.pop();
        }
        else if (!stringStack.empty()) {
            stringStack.pop();
        }
    }

    void popReg() {
        if (!intStack.empty()) {
            int number = intStack.top();
            intStack.pop();
            uint8_t r1 = memory[programCounter];
            programCounter++;
            registers[r1] = number;
        }
    }

    void print() {
        if (!intStack.empty()) {
            std::cout << intStack.top() << std::endl;
            intStack.pop();
        }
        else if (!stringStack.empty()) {
            std::cout << stringStack.top() << std::endl;
            stringStack.pop();
        }
    }

    void moveInt()
    {
        uint8_t r1 = memory[programCounter];
        programCounter++;
        auto number = readInt();
        registers[r1] = number;
    }

    void moveReg()
    {
        auto result = readRegs();
        int r1 = std::get<0>(result);
        int r2 = std::get<1>(result);
        registers[r1] = registers[r2];
    }

    void callBack()
    {
        if (!stringStack.empty()) {
            std::string name = stringStack.top();

            int i = data[name]();
            intStack.push(i);
        }
    }

    void input() {
        if (!intStack.empty()) {
            int input;
            std::cin >> input;
            intStack.push(input);
        }
        else if (!stringStack.empty()) {
            std::string input;
            std::cin >> input;
            stringStack.push(input);
        }
    }

    void add() {
        auto result = readRegs();
        int r1 = std::get<0>(result);
        int r2 = std::get<1>(result);
        registers[r1] += registers[r2];
    }

    void sub() {
        auto result = readRegs();
        int r1 = std::get<0>(result);
        int r2 = std::get<1>(result);
        registers[r1] -= registers[r2];
    }

    void mul() {
        auto result = readRegs();
        int r1 = std::get<0>(result);
        int r2 = std::get<1>(result);
        registers[r1] *= registers[r2];
    }

    void cmp() {
        auto result = readRegs();
        int r1 = std::get<0>(result);
        int r2 = std::get<1>(result);
        intStack.push(r1 - r2);
    }

    void je() {
        int jumpAddress = readInt();
        if (intStack.top() == 0) {
            programCounter = jumpAddress;
        }
        intStack.pop();
    }

    void jne() {
        int jumpAddress = readInt();
        if (intStack.top() != 0) {
            programCounter = jumpAddress;
        }
        intStack.pop();
    }

    void jmp() {
        int jumpAddress = readInt();
        programCounter = jumpAddress;
    }

    int readInt() {
        int value = memory[programCounter];
        programCounter += 4;
        return value;
    }

    std::tuple<int, int> readRegs() {
        uint8_t r1 = memory[programCounter];
        programCounter++;
        uint8_t r2 = memory[programCounter];
        programCounter++;

        return std::make_tuple(r1, r2);
    }

    std::string readString() {
        int size = memory[programCounter];
        programCounter++;
        std::string value(memory.begin() + programCounter, memory.begin() + programCounter + size);
        programCounter += size;
        return value;
    }
};