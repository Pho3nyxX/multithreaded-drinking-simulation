# Multithreaded Drinking Simulation

A multithreaded C++ simulation demonstrating how concurrent processes safely interact with shared resources using mutexes and condition variables, modeled through a scenario of students sharing a drink barrel monitored by a bartender.

---

## Project Overview

This simulation consists of several interacting components:

- **Student Threads** – Each student runs in its own thread and repeatedly attempts to access the shared barrel, consume a random number of drinks, and then enter a thinking state.  
- **Bartender Thread** – Remains idle until notified that the barrel is empty, then refills it a limited number of times.   
- **Shared Barrel** – A global resource protected by mutex locks to prevent race conditions during access.   
- **Control System** – Allows the simulation to be paused and resumed while displaying Process Control Block (PCB) data for all active threads.   

Together, these components demonstrate real-world thread behavior such as contention, waiting, signalling, and synchronized resource management.

---

## How the Simulation Works

#### Students

- Each student is a thread.
- Students attempt to take a random number of drinks (1–5).
- After drinking, they enter a "thinking" state for a random period.
- They maintain a PCB (Process Control Block) showing:
    - Thread ID
    - State
    - Drinks requested
    - Drinks consumed
    - Wait count

#### Bartender

- Sleeps until the barrel is empty.   
- Refills the barrel 3 times maximum.   
- Each refill adds 31–50 drinks.   
- Displays its wake count and current state.   

#### Barrel

- Shared resource initialized with 50 drinks.
- Access controlled via mutex locks.
- When empty, it signals the bartender using a condition variable.

#### Pause / Resume

- Pressing ENTER toggles the simulation between paused and running states.
- While paused, all threads wait.
- PCB data for all entities is printed.

---

## Build and Run:

1. Open a terminal window
2. ``cd`` to the project directory
3. Build command:
    ```
    g++ -pthread Project.cpp –o Project
    ```
4. Run command:
    ```
    ./Project
    ```
5. Pause command: Enter
6. Resume command: Enter
7. Exit command: Ctrl+c 
