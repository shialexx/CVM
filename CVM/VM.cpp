#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <functional>
#include <map>


typedef std::function<int()> Function;

class VirtualMachine {
private:
    std::map<std::string, Function> Data{};
    std::vector<uint8_t> Memory;
    std::stack<int> IntStack;
    std::stack<std::string> StringStack;
    std::map<int, int> Registers;
    int ProgramCounter;
public:
    VirtualMachine(const std::vector<uint8_t>& ByteCode, std::map<std::string, Function> Data)
    {
        Memory = ByteCode;
        ProgramCounter = 0;
        this->Data = Data;

        for (int i = 1; i < 5; i++)
            this->Registers[i] = 0;
    }

    void execute() {
        while (ProgramCounter < Memory.size()) {
            uint8_t Instructions = Memory[ProgramCounter];
            ProgramCounter++;

            switch (Instructions) {
            case 0x01:
                PushInt();
                break;
            case 0x02:
                PushStr();
                break;
            case 0x03:
                PushReg();
            case 0x04:
                Print();
                break;
            case 0x05:
                Input();
                break;
            case 0x06:
                Add();
                break;
            case 0x07:
                Sub();
                break;
            case 0x08:
                Mul();
                break;
            case 0x09:
                Cmp();
                break;
            case 0x0A:
                Je();
                break;
            case 0x0B:
                Jne();
                break;
            case 0x0C:
                Jmp();
                break;
            case 0x0D:
                CallBack();
                break;
            case 0x0E:
                MoveInt();
                break;
            case 0x0F:
                MoveReg();
                break;
            case 0x10:
                Pop();
                break;
            case 0x11:
                PopReg();
                break;
            default:
                std::cerr << "Invalid Instruction: " << Instructions << std::endl;
                return;
            }
        }
    }

private:
    void PushInt() {
        int Value = ReadInt();
        IntStack.push(Value);
    }

    void PushStr() {
        std::string Value = ReadString();
        StringStack.push(Value);
    }

    void PushReg() {
        uint8_t R1 = Memory[ProgramCounter];
        ProgramCounter++;
        IntStack.push(Registers[R1]);
    }

    void Pop()
    {
        if (!IntStack.empty()) {
            IntStack.pop();
        }
        else if (!StringStack.empty()) {
            StringStack.pop();
        }
    }

    void PopReg() {
        if (!IntStack.empty()) {
            int Number = IntStack.top();
            IntStack.pop();
            uint8_t R1 = Memory[ProgramCounter];
            ProgramCounter++;
            Registers[R1] = Number;
        }
    }

    void Print() {
        if (!IntStack.empty()) {
            std::cout << IntStack.top() << std::endl;
            IntStack.pop();
        }
        else if (!StringStack.empty()) {
            std::cout << StringStack.top() << std::endl;
            StringStack.pop();
        }
    }

    void MoveInt()
    {
        uint8_t R1 = Memory[ProgramCounter];
        ProgramCounter++;
        auto Number = ReadInt();
        Registers[R1] = Number;
    }

    void MoveReg()
    {
        auto Result = ReadRegs();
        int R1 = std::get<0>(Result);
        int R2 = std::get<1>(Result);
        Registers[R1] = Registers[R2];
    }

    void CallBack()
    {
        if (!StringStack.empty()) {
            std::string Name = StringStack.top();

            int It = Data[Name]();
            IntStack.push(It);
        }
    }

    void Input() {
        if (!IntStack.empty()) {
            int Input;
            std::cin >> Input;
            IntStack.push(Input);
        }
        else if (!StringStack.empty()) {
            std::string Input;
            std::cin >> Input;
            StringStack.push(Input);
        }
    }

    void Add() {
        auto Result = ReadRegs();
        int R1 = std::get<0>(Result);
        int R2 = std::get<1>(Result);
        Registers[R1] += Registers[R2];
    }

    void Sub() {
        auto Result = ReadRegs();
        int R1 = std::get<0>(Result);
        int R2 = std::get<1>(Result);
        Registers[R1] -= Registers[R2];
    }

    void Mul() {
        auto Result = ReadRegs();
        int R1 = std::get<0>(Result);
        int R2 = std::get<1>(Result);
        Registers[R1] *= Registers[R2];
    }

    void Cmp() {
        auto Result = ReadRegs();
        int R1 = std::get<0>(Result);
        int R2 = std::get<1>(Result);
        IntStack.push(R1 - R2);
    }

    void Je() {
        int JumpAddress = ReadInt();
        if (IntStack.top() == 0) {
            ProgramCounter = JumpAddress;
        }
        IntStack.pop();
    }

    void Jne() {
        int JumpAddress = ReadInt();
        if (IntStack.top() != 0) {
            ProgramCounter = JumpAddress;
        }
        IntStack.pop();
    }

    void Jmp() {
        int JumpAddress = ReadInt();
        ProgramCounter = JumpAddress;
    }

    int ReadInt() {
        int Value = Memory[ProgramCounter];
        ProgramCounter += 4;
        return Value;
    }

    std::tuple<int, int> ReadRegs() {
        uint8_t R1 = Memory[ProgramCounter];
        ProgramCounter++;
        uint8_t R2 = Memory[ProgramCounter];
        ProgramCounter++;

        return std::make_tuple(R1, R2);
    }

    std::string ReadString() {
        int size = Memory[ProgramCounter];
        ProgramCounter++;
        std::string Value(Memory.begin() + ProgramCounter, Memory.begin() + ProgramCounter + size);
        ProgramCounter += size;
        return Value;
    }
};