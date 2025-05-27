# 🕹️ CHIP-8 Emulator

A simple CHIP-8 emulator written in C++ using SDL2. This emulator loads and runs CHIP-8 ROMs, such as the classic **IBM Logo**, with support for basic opcode interpretation, input handling, and graphics rendering.

---

## 📸 Features

- ✅ 64x32 monochrome display
- ✅ SDL2-based rendering
- ✅ Keyboard input support (mapped to CHIP-8 keys)
- ✅ Basic instruction set implemented
- ✅ IBM Logo ROM runs and displays correctly
- 🛠️ Easy to extend with sound or debugger support

## 📂 Project Structure

```bash
chip8-emulator/
├── src/ # All source files
│ ├── main.cpp
│ ├── cpu.cpp/h # Core CPU emulation
│ ├── display.cpp/h # Graphics output
│ ├── input.cpp/h # Input handling
├── README.md # This file
└── .gitignore
```

## 🚀 Getting Started

### 🔧 Prerequisites

- C++17 or newer
- [SDL2](https://www.libsdl.org/download-2.0.php)

## ⌨️ Controls
CHIP-8 uses a 16-key hexadecimal keypad:

mathematica
Copy
Edit
Original:      Mapped to Keyboard:

1 2 3 C        1 2 3 4  
4 5 6 D        Q W E R  
7 8 9 E        A S D F  
A 0 B F        Z X C V
You can adjust key mappings in input.cpp if needed.

## 📌 TODO
- [ ] Add sound (FX18, FX07)
- [ ] Add full instruction set
- [ ] Add debugging options (step mode, disassembler)
- [ ] Add unit tests

## 📜 License
This project is open-source and licensed under the MIT License.

## 🙌 Acknowledgments
- Cowgod's CHIP-8 Technical Reference
- SDL2 for graphics/input
- Community contributions to CHIP-8 documentation and ROMs