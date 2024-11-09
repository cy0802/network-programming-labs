# INP112 Lab03

Date: 2023-10-19

[TOC]

# Play with Mazes

## Description

This lab aims to practice implementing clients to interact with TCP servers. We prepared four different flavors of text-based maze servers. Please follow the instructions and have fun!

1. You may play with the maze servers *manually* before implementing the clients. The servers can be accessed via the following commands:

   ```
   nc inp.zoolab.org 10301
   nc inp.zoolab.org 10302
   nc inp.zoolab.org 10303
   nc inp.zoolab.org 10304
   ```

1. Each server displays a full or partial maze upon connecting to the server. You (or your client implementation) have to walk from the current position (indicated as an asterisk `*`) to the exit `E`. The sharp (`#`) character indicates a wall in the maze, and the dot (`.`) character indicates a road in the maze.

1. To change the current position, you can send commands `W`, `S`, `A`, and `D` to move up, down, left, and right, respectively. 

1. The time limits for solving the mazes are as follows:
   - maze #1: 60s
   - maze #2: 60s
   - maze #3: 60s
   - maze #4: 600s

1. If you have correctly solved a maze, i.e., walk from the initial position to the exit `E`, the server displays a `Bingo!` message followed by a secret message. Please paste the secret message to the verification service running at https://inp.zoolab.org/maze/service, and then the TAs can verify if your solution is correct. 

## Demonstration

1. [20%] Your implemented TCP client can solve maze #1. Repeat running your solver program three times. Each time, your solver must solve the challenge successfully. 

1. [25%] Your implemented TCP client can solve maze #2. Repeat running your solver program three times. Each time, your solver must solve the challenge successfully.

1. [30%] Your implemented TCP client can solve maze #3. Repeat running your solver program three times. Each time, your solver must solve the challenge successfully.

1. [25%] Your implemented TCP client can solve maze #4. Repeat running your solver program three times. Your solver should find a correct solution at least one time.
