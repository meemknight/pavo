# pavo

A command line and GUI debugger for linux (soon port to windows), following this tutorial: https://blog.tartanllama.xyz/writing-a-linux-debugger-setup/

---

Todo and Features

- [x] Linux Support
- [ ] Windows Support

- [ ] COmmand line interface
- [ ] GUI interface

- [ ] Open processes
- [ ] Launch hault and continue execution
- [ ] Set break points
    - [ ] On memory
    - [ ] On code lines
    - [ ] On functions
   
- [ ] Read write
    - [ ] to Memory
    - [ ] to Registers

- [ ] Setp in out next etc

- [ ] Print current location
- [ ] Print back trace
- [ ] Print values of variables

---

Technologies used: 

* cpp
* cmake

Libraries:

* Project configuration: [cmakeSetup](https://github.com/meemknight/cmakeSetup).
* Window management: [glfw](https://github.com/glfw/glfw).
* Opengl loader: [glad](https://github.com/Dav1dde/glad).
* GUI: [imgui](https://github.com/ocornut/imgui).
* Unit Tests: [Catch2](https://github.com/catchorg/Catch2).
* Formatting: [fmt](https://github.com/fmtlib/fmt).
* Detect cpu features: [cpu_features](https://github.com/google/cpu_features).
* Parse Linux specific debug symbols: [libelfin](https://github.com/aclements/libelfin).




