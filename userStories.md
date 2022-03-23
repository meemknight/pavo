User Stories
---

  The most widely used debugger for c/cpp on windows is the one from microsoft visual studio. While it has the most features it doesn't work on linux and it is very bloated. If you don't have a computer with a SSD you are left without a debugger. Another viable option would be the one from CLion which is less bloated but still not as light as you would want it to be. You can debug c/cpp programs with vs code with the necessary extensions but it is very limited in features. GDB is a good linux debugger but it only has a CLI interface which is inconvinient. A good option would be https://remedybg.handmade.network but it is not a free option.

  A lightweight easy to use debugger would be very helpfull for beginner programmers and help them find bugs easier.

  This are the basic features needed in a debugger:
  
1. As a user I want to launch a compiled program.

2. As a user I want to set breakpoints in my program so that I can halt the exection of said program.

3. As a user I want to see the values of certain variables when the program has been halted so that I can check the current state of the program.

4. As a user I want to take a single step forward in the execution of my program after it has been halted so that I can follow the changes of the variables.

5. As a user I want to continue the execution of my program after it has been halted up to the next breakpoint that has been set or to the end of the program if no such breakpoint exist.

6. As a user I want the debugger to work on both Linux and Windows if possible.

7. A GUI implementation would make the debugger much practical to use than a CLI.

8. As a user I want the debugger to run smoothly.

9. A multy language feature would be desired if possible.
