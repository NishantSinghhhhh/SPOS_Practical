#include <bits/stdc++.h>
using namespace std;

// ======== MICROPROCESSOR MODEL ========

// 8-bit registers & flags
struct Registers {
    uint8_t A{0};   // Accumulator
    uint8_t B{0};   // General purpose
    uint8_t PC{0};  // Program Counter
    bool Z{false};  // Zero flag
};

// Simple RAM (256 bytes)
struct Memory {
    array<uint8_t, 256> mem{};
    uint8_t read(uint8_t addr) const { return mem[addr]; }
    void write(uint8_t addr, uint8_t val) { mem[addr] = val; }
};

// ALU (Arithmetic / Logic Unit)
struct ALU {
    static uint8_t add(uint8_t a, uint8_t b, bool& Z) {
        uint16_t r = uint16_t(a) + uint16_t(b);
        Z = (uint8_t(r) == 0);
        return uint8_t(r);
    }
    static uint8_t sub(uint8_t a, uint8_t b, bool& Z) {
        uint16_t r = uint16_t(a) - uint16_t(b);
        Z = (uint8_t(r) == 0);
        return uint8_t(r);
    }
    static uint8_t _and(uint8_t a, uint8_t b, bool& Z) {
        uint8_t r = a & b; Z = (r == 0); return r;
    }
    static uint8_t _or(uint8_t a, uint8_t b, bool& Z) {
        uint8_t r = a | b; Z = (r == 0); return r;
    }
    static uint8_t _xor(uint8_t a, uint8_t b, bool& Z) {
        uint8_t r = a ^ b; Z = (r == 0); return r;
    }
};

// Opcodes (1-byte). Operands (0–2 bytes) follow.
enum OPC : uint8_t {
    NOP=0x00, HLT=0xFF,
    // Load / Store
    LDI_A=0x10, LDI_B=0x11,  // LDI_* imm8
    LDA  =0x12,              // A <- [addr]
    STA  =0x13,              // [addr] <- A
    // ALU (A = A op B or A op mem[addr])
    ADD_B=0x20, SUB_B=0x21, AND_B=0x22, OR_B=0x23, XOR_B=0x24,
    ADD_M=0x25, SUB_M=0x26,  // A = A op [addr]
    // Move
    MOV_AB=0x30, MOV_BA=0x31,
    // Control flow
    JMP=0x40, JZ=0x41, JNZ=0x42,
    // I/O
    OUTA=0x50                // print A
};

// Control Unit + whole CPU
struct CPU {
    Registers R;
    Memory M;
    bool halted{false};
    bool trace{true};

    uint8_t fetch() { return M.read(R.PC++); }

    void step() {
        uint8_t op = fetch();
        auto imm8 = [&]{ return fetch(); };
        auto addr = [&]{ return fetch(); };

        if (trace) cout << hex << setw(2) << setfill('0');

        switch (op) {
            case NOP: if (trace) cout << "NOP\n"; break;
            case HLT: if (trace) cout << "HLT\n"; halted = true; break;

            case LDI_A: {
                uint8_t v = imm8(); R.A = v; R.Z = (R.A == 0);
                if (trace) cout << "LDI A,#" << int(v) << "  -> A=" << int(R.A) << "\n";
            } break;
            case LDI_B: {
                uint8_t v = imm8(); R.B = v; R.Z = (R.B == 0);
                if (trace) cout << "LDI B,#" << int(v) << "  -> B=" << int(R.B) << "\n";
            } break;
            case LDA: {
                uint8_t a = addr(); R.A = M.read(a); R.Z = (R.A == 0);
                if (trace) cout << "LDA [" << int(a) << "] -> A=" << int(R.A) << "\n";
            } break;
            case STA: {
                uint8_t a = addr(); M.write(a, R.A);
                if (trace) cout << "STA [" << int(a) << "] <- A(" << int(R.A) << ")\n";
            } break;

            case ADD_B: R.A = ALU::add(R.A, R.B, R.Z);
                if (trace) cout << "ADD A,B   -> A=" << int(R.A) << " Z=" << R.Z << "\n"; break;
            case SUB_B: R.A = ALU::sub(R.A, R.B, R.Z);
                if (trace) cout << "SUB A,B   -> A=" << int(R.A) << " Z=" << R.Z << "\n"; break;
            case AND_B: R.A = ALU::_and(R.A, R.B, R.Z);
                if (trace) cout << "AND A,B   -> A=" << int(R.A) << " Z=" << R.Z << "\n"; break;
            case OR_B:  R.A = ALU::_or (R.A, R.B, R.Z);
                if (trace) cout << "OR  A,B   -> A=" << int(R.A) << " Z=" << R.Z << "\n"; break;
            case XOR_B: R.A = ALU::_xor(R.A, R.B, R.Z);
                if (trace) cout << "XOR A,B   -> A=" << int(R.A) << " Z=" << R.Z << "\n"; break;

            case ADD_M: {
                uint8_t a = addr(); R.A = ALU::add(R.A, M.read(a), R.Z);
                if (trace) cout << "ADD A,[" << int(a) << "] -> A=" << int(R.A) << " Z=" << R.Z << "\n";
            } break;
            case SUB_M: {
                uint8_t a = addr(); R.A = ALU::sub(R.A, M.read(a), R.Z);
                if (trace) cout << "SUB A,[" << int(a) << "] -> A=" << int(R.A) << " Z=" << R.Z << "\n";
            } break;

            case MOV_AB: R.B = R.A; R.Z = (R.B == 0);
                if (trace) cout << "MOV B,A   -> B=" << int(R.B) << "\n"; break;
            case MOV_BA: R.A = R.B; R.Z = (R.A == 0);
                if (trace) cout << "MOV A,B   -> A=" << int(R.A) << "\n"; break;

            case JMP: {
                uint8_t a = addr(); if (trace) cout << "JMP " << int(a) << "\n"; R.PC = a;
            } break;
            case JZ:  { uint8_t a = addr(); if (trace) cout << "JZ  " << int(a) << " (Z="<<R.Z<<")\n";  if (R.Z)  R.PC = a; } break;
            case JNZ: { uint8_t a = addr(); if (trace) cout << "JNZ " << int(a) << " (Z="<<R.Z<<")\n"; if (!R.Z) R.PC = a; } break;

            case OUTA:
                cout << dec << "OUT A => " << int(R.A) << "\n"; break;

            default:
                cerr << "Illegal opcode 0x" << hex << int(op) << " at PC=" << int(R.PC-1) << "\n";
                halted = true;
        }
    }

    void run(size_t maxCycles = 1000) {
        size_t c = 0;
        while (!halted && c++ < maxCycles) step();
        if (c >= maxCycles) cerr << "Cycle limit reached\n";
    }
};

// ======== DEMO PROGRAM ========
// Program: Add two numbers from memory, store result, print it, loop-halt
// Memory map:
//   [0xF0] = 10
//   [0xF1] = 32
// Result -> [0xF2]

vector<uint8_t> demoProgram() {
    vector<uint8_t> p;
    auto emit=[&](uint8_t b){ p.push_back(b); };

    // Load A <- [0xF0], B <- imm 0 (then ADD with mem to show two modes)
    emit(LDA);   emit(0xF0);      // A = mem[F0] (10)
    emit(LDI_B); emit(0x00);      // B = 0
    emit(ADD_M); emit(0xF1);      // A = A + mem[F1] (32) => 42
    emit(STA);   emit(0xF2);      // mem[F2] = A
    emit(OUTA);                   // print A (42)
    // Show conditional branch:
    emit(SUB_M); emit(0xF2);      // A = A - mem[F2] => 0, Z=1
    emit(JZ);    emit(0x15);      // if Z jump to HLT (address set below)
    emit(JMP);   emit(0x00);      // else loop (won't happen)
    emit(HLT);                    // address 0x15 (21)
    return p;
}

int main() {
    CPU cpu;
    cpu.trace = true;

    // Preload data in RAM
    cpu.M.write(0xF0, 10);
    cpu.M.write(0xF1, 32);

    // Load program at address 0x00
    auto prog = demoProgram();
    for (size_t i = 0; i < prog.size(); ++i) cpu.M.write(uint8_t(i), prog[i]);

    cout << "=== Mini 8-bit CPU Simulation ===\n";
    cpu.run();

    // Inspect memory/result
    cout << "mem[F2] = " << int(cpu.M.read(0xF2)) << " (expected 42)\n";
    return 0;
}
