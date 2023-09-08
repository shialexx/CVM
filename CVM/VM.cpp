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
    VirtualMachine(const std::vector<uint8_t>& ByteCode, std::map<std::string, Function> Data = {})
    {
        Memory = ByteCode;
        ProgramCounter = 0;
        this->Data = Data;

        for (int i = 1; i < 5; i++)
            this->Registers[i] = 0;
    }

    void Execute() {
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
        IntStack.push(ReadInt());
    }

    void PushStr() {
        StringStack.push(ReadString());
    }

    void PushReg() {
        ProgramCounter++;
        IntStack.push(Registers[Memory[ProgramCounter]]);
    }

    void Pop()
    {
        if (IntStack.empty() || StringStack.empty())
            return;

        StringStack.pop();
        IntStack.pop();
    }

    void PopReg() {
        if (IntStack.empty())
            return;

        IntStack.pop();
        ProgramCounter++;
        Registers[Memory[ProgramCounter]] = IntStack.top();
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
        ProgramCounter++;
        Registers[Memory[ProgramCounter]] = ReadInt();
    }

    void MoveReg()
    {
        auto [R1, R2] = ReadRegs();
        Registers[R1] = Registers[R2];
    }

    void CallBack()
    {
        if (StringStack.empty())
            return;

        IntStack.push(Data[StringStack.top()]());
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
        auto [R1, R2] = ReadRegs();
        Registers[R1] += Registers[R2];
    }

    void Sub() {
        auto [R1, R2] = ReadRegs();
        Registers[R1] -= Registers[R2];
    }

    void Mul() {
        auto [R1, R2] = ReadRegs();
        Registers[R1] *= Registers[R2];
    }

    void Cmp() {
        auto [R1, R2] = ReadRegs();
        IntStack.push(Registers[R1] - Registers[R2]);
    }

    void Je() {
        if (IntStack.top() == 0)
            ProgramCounter = ReadInt();

        IntStack.pop();
    }

    void Jne() {
        if (IntStack.top() != 0)
            ProgramCounter = ReadInt();

        IntStack.pop();
    }

    void Jmp() {
        ProgramCounter = ReadInt();
    }

    int ReadInt() {
        ProgramCounter += 4;
        return Memory[ProgramCounter];
    }

    std::tuple<int, int> ReadRegs() {
        uint8_t R1 = Memory[ProgramCounter];
        ProgramCounter++;
        uint8_t R2 = Memory[ProgramCounter];
        ProgramCounter++;

        return std::make_tuple(R1, R2);
    }

    std::string ReadString() {
        ProgramCounter++;
        ProgramCounter += Memory[ProgramCounter];
        return std::string(Memory.begin() + ProgramCounter, Memory.begin() + ProgramCounter + Memory[ProgramCounter]);
    }
};