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
        this->registers[1] = 0;
        this->registers[2] = 0;
        this->registers[3] = 0;
        this->registers[4] = 0;
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
                print();
                break;
            case 0x04:
                input();
                break;
            case 0x05:
                add();
                break;
            case 0x06:
                sub();
                break;
            case 0x07:
                mul();
                break;
            case 0x08:
                cmp();
                break;
            case 0x09:
                je();
                break;
            case 0x0A:
                jne();
                break;
            case 0x0B:
                jmp();
                break;
            case 0x0C:
                CallBack();
                break;
            case 0x0E:
                pop();
                break;
            case 0x0D:
                MovInt();
                break;
            case 0x0F:
                move_registers();
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

    void pop()
    {
        if (!intStack.empty()) {
            intStack.pop();
        }
        else if (!stringStack.empty()) {
            stringStack.pop();
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

    void MovInt()
    {
        uint8_t r1 = memory[programCounter];

        programCounter++;

        auto number = readInt();

        registers[r1] = number;
    }

    void move_registers()
    {
        uint8_t r1 = memory[programCounter];
        programCounter++;

        uint8_t r2 = memory[programCounter];
        programCounter++;

        registers[r1] = registers[r2];
    }

    void CallBack()
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
        int a = intStack.top();
        intStack.pop();
        int b = intStack.top();
        intStack.pop();
        intStack.push(a + b);
    }

    void sub() {
        int a = intStack.top();
        intStack.pop();
        int b = intStack.top();
        intStack.pop();
        intStack.push(b - a);
    }

    void mul() {
        int a = intStack.top();
        intStack.pop();
        int b = intStack.top();
        intStack.pop();
        intStack.push(a * b);
    }

    void cmp() {
        int a = intStack.top();
        intStack.pop();
        int b = intStack.top();
        intStack.pop();
        intStack.push(b - a);
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

    std::string readString() {
        int size = memory[programCounter];
        programCounter++;
        std::string value(memory.begin() + programCounter, memory.begin() + programCounter + size);
        programCounter += size;
        return value;
    }
};