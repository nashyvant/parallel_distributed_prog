# HW4 Answer the following questions about threading and memory access

[Register](https://en.wikipedia.org/wiki/Processor_register)
[Cache](https://en.wikipedia.org/wiki/Cache_(computing))

1. Basic facts
    a. What does a computer's clock speed regulate? 
    CPU speed which impacts the speed of execution of an instruction.
    A CPU with a clock speed of 3.2 GHz executes 3.2 billion cycles per second. 
    (Older CPUs had speeds measured in megahertz, or millions of cycles per second.) 
    Sometimes, multiple instructions are completed in a single clock cycle; 
    in other cases, one instruction might be handled over multiple clock cycles. 
    Since different CPU designs handle instructions differently, it’s best to compare clock speeds 
    within the same CPU brand and generation.
    For example, a CPU with a higher clock speed from five years ago might be outperformed by a 
    new CPU with a lower clock speed, as the newer architecture deals with instructions more efficiently.

    b. We double the clock speed
      1. memory read is __________
      2. memory write is _________
      3. reading from cache is _______
      4. multiplication speed is _______
      Here’s how things typically change when you double the clock speed:

1. Same   
Doubling the CPU clock doesn’t necessarily speed up memory access, as it depends on external memory speed (e.g., RAM).

2. Same 
Similar to memory read, memory write operations depend on the memory’s latency and bandwidth, not directly 
on the CPU clock speed.

3. 2x faster
Cache memory operates closely with the CPU, so if the CPU clock speed doubles, cache access 
times generally scale accordingly. Thus, reading from the cache would be approximately 2x faster.

4. 2x faster
Since the CPU clock directly impacts the execution speed of arithmetic operations, doubling the clock 
speed typically results in arithmetic operations, like multiplication, being performed 2x faster.

    c. Speed of each instruction in clock cycles.
      1. add %rax, %rbx _________ 1 clock cycle (or the latency)
      2. mul %rax, %rbx _________ 3-5 
      3. div %rcx       _________ 15
      4. shl $1, %rax   _________ 1
    d. Assume the following memory read instructions are in cache,
       but the location being read is new. timing is 46-45-45 (CAS, RAS, precharge )
      1. mov (%rsi), %rax       _________ (46+45 cycles if you ve never seen it) or just 46 if already in 
      the row, and 1 cycle getting a new block in the row sequentially. This is the # of cycles
      memory takes and we are not counting any additional cycles from the CPU etc.
      ```assembler
      mov (%rsi), %rax 46+45
      mov 8(%rsi), %rbx 1 ( a little more than 1)
      mov 16(%rsi), %rcx 1
      mov 24(%rsi), %rdx 1
      ```
      2. vmovdqa (%rsi), %ymm0 __________ 46+45+1*3 (on a 64 bit machine)
      3. what is the amortized cost of reading 16 sequential memory locations? 46+45+1*15
      4. what is the amortized cost of reading 32 sequential memory locations? 
      For DDR5, each bank of memory can do 16 bursts. If you have 2 banks it'll work the same way. 
      46+45+1*31
      5. follow up to #4, what would be the cost of next block of 32?
      46(to address a column)+1*31

    e. Why can't we just double the clock speed to make our computer faster? _______________
     From Lawrence Stewart, Quora

    As the clock speeds up, the time between clocks becomes shorter. At some point, the logic 
    (ands, ors, invert, memory cells, wires) are simply not fast enough to finish their work before 
    the next clock. If you try to run the clock faster than that, the CPU will just not work (if you are lucky) 
    or give wrong answers (if you are unlucky).

   To some extent, you can speed up the logic by cooling it down or by turning up the voltage. 
   Too much cooling risks breaking connections which become brittle, and too much voltage risks overheating.

   Running the CPU too close to the limits of what the logic can do risks subtle errors, general flakiness, 
   and random crashes.

   Speeding up the clock while supplying extra cooling and more voltage is called overclocking, 
   and it does work, up to a point.

   ChatGPT:
    Heat Generation: Higher clock speeds lead to increased power consumption and heat generation. 
    CPUs produce more heat as they operate faster, which can lead to overheating. If the heat isn't 
    dissipated effectively, it can damage the CPU or lead to system instability.
   
   Power Consumption: The power consumption of a CPU increases non-linearly with clock speed. 
   This means that doubling the clock speed doesn't just double the power usage; it can significantly 
   increase it, leading to issues with energy efficiency and battery life in portable devices.

   Diminishing Returns: As clock speeds increase, the performance gains become smaller due to various factors, 
   including memory latency and other bottlenecks in the system architecture. Other components, such as 
   RAM and storage, may not be able to keep up with a CPU running at very high speeds.

    f. What is the fastest memory in a computer? ________________ Registers
    g. What is the 2nd fastest memory? ________________ Cache
    h. What makes main memory (DRAM) so much slower than the fastest memory? ________________ 
    
    DRAM/main memory is away from the CPU on the mother board. It uses capacitors to store data, 
    that leaks charge over time, and hence requires frequent refresh cycles to maintain 
    data integrity.The constant refresh cycles adds more processing time to main memory and hence is slower.
    This memory is also called as dynamic RAM(DRAM)

    Registers/cache are located on the CPU and use a flip-flop technique to store data. 
    This method does not constant refresh cycles and is very expensive. It is called as static RAM.

    i. How many integer registers are there on an x86-64 machine? ________________ 16
    %rax, %rbx, %rcx, %rdx, %rsi, %rdi, %rbp, %rsp (legacy registers from the x86 architecture)
    %r8, %r9, %r10, %r11, %r12, %r13, %r14, %r15 (additional registers introduced in the x86-64 extension)

    j. How many vector registers are there on an AVX2 machine? ________________ 16
    AVX2 (Advanced Vector Extensions 2) is an extension of the AVX (Advanced Vector Extensions) 
    instruction set introduced by Intel. It enhances the capabilities of SIMD (Single Instruction, Multiple Data) operations, allowing for more efficient data processing in parallel.

    16 vector registers in total, referred to as YMM0-YMM15. Each YMM register is 256 bits wide 
    and is used to handle packed floating-point or integer operations.

    k. How many integer registers are there on an ARM64 machine? ________________ 31 + 1 special register
     X0-X30 are used for general-purpose operations
     XZR register serves as a constant zero in certain instructions

    l. Why might the ARM designers have chosen differently than Intel? ________________
    ARM is designed for lower energy consumption and the end market is focused on mobile phones 
    and other embedded system. Hence the architecture decisions could have been much 
    different from Intel's. Intel is more focused on laptop market.

    m. A special called RIP on intel rip. What does it stand for: _____________________
    Instruction pointer register
    n. Look up what it does _________?
    The instruction pointer register (%rip) points to the next instruction to execute; it cannot be 
    directly accessed by the programmer, but is heavily used as the base for position-independent 
    code addressing.

    o. What is the register RSP on intel? _____________________
    Stack pointer register. The stack grows down in memory; %rsp points to the lowest occupied 
    stack location (not to the next one to use).
   
    p. What is L1 cache on x86 architecture? It is the smallest and fastest cache
    number of cores ____1______, size ____________ 256 kB
    q. What is L2 cache on x86 architecture? cache that is bigger than L1 but slower than L1. number of cores ____2______, size ____2 MB________
    r. What is L3 cache on x86 architecture? cache bigger than L2 but slower than L2. number of cores _____4_____, size _____4 MB_______
    The number of cores is based for a 4 core processor
    s. Approximately how long does it take light to travel 30cm? _______________
    speed = dist * time
    3 *10^8 m/s = 30 cm/ 100 * x
    x = 3*10^9
       -------- = 1 ns
         3

2. Class Survey
   a. Enter your name into a row of the spreadsheet and complete the data for your computer.
   b. If you have more than one computer you can pick the one you use. You may enter more than one.
https://docs.google.com/spreadsheets/d/10DiQJcTMTqcE1JjSKFx0AWUcOAqu75cQUcKsF0jtxBg/edit?usp=sharing

2. Basics of Multiprocessing
    a. What is a process?
    A process is an instance of a program in execution. Each process has its own memory space for
    code, data, stack, registers, heap and program counter, and are thus isolated from each other.
    The operating system can switch from one process to the other and it is an expensive operation that is
    resource intensive. Processes communicate via IPC.

    b. What is a thread?
    Thread is a smallest unit of execution within a process. A process can contain multiple threads.
    Threads share the same heap, code and globals.Each thread has its registers, stack 
    and program counter.Threads are light-weight and is faster to conext switch between them. Threads can
    communicate with each other via shared memory.

    c. Every thread requires at a minimum sp, pc/rip. Explain
   Threads can be executed concurrently or in parallel. To be able to do so, the threads need to maintain the
   order of execution which can be obtained by having its own stack (monitored by stack pointer) and 
   program counter to know which is the next instruction to execute for the given thread. This also helps
   maintain the state of each thread. They all share the same code, data and heap space.
 
    d. A computer with 4 cores is running a job.
       1 thread, t=10s, 2 threads t=5s, 4 threads t=3s
       Neglecting hyperthreading, Why might it not be a good idea to run with 8 threads? 
       Since the computer has only 4 cores, it would be ideal to run with 4 threads so each thread gets a core.
       If we were able to run this with 8 threads, a lot of time will be spent in context switching rather 
       than do actual CPU work. This reduces overall efficiency and increases execution time. Adding more threads 
       beyond the number of available cores often results in minimal or no performance 
       improvement, and can sometimes even degrade performance due to overhead and contention.

3. Explain the benchmark results for memory_timings.s.
   a. For each function run explain (in one line) what it is attempting to measure.
   b. Why is write_one so much slower than read_one?
   c. Why is read_memory_avx faster than read_memory_scalar if both are reading the same amount of memory 
   sequentially?
   read_memory_avx reads 256 bits at a time while read_memory_scalar only reads 64 bits at a time.
   d. Run on your computer (or a lab computer if yours is an ARM or you have some other problem). Report the results.
     1. Extra credit: if you write your own assembly code and test a different architecture, + 50%
     2. Find the CPU and memory configuration for the machine you tested. 
     This can be About... in windows, or in linux you can use lscpu and cat /proc/cpuinfo

5. Explain what a cache is cache the second fastest memory available and on CPU. It stored the most recently used data.
   a. Explain what a cache miss is
   it is a cache miss when you want to read from memory and that is not currently available in the cache.

6. Why is it so important to understand memory performance for parallel computing?
a. memory speed 
all 4 cpus writing to different pieces of memory
sequentially - CPU not waiting too long; if jumping around, writing is much slower 
You also need to wait on the memory bus if you're writing to RAM or main memory 
CPU spends waiting for the other CPU to finish writing and if we're maxing out on memory BW, we're slowing down
the threads.

if reading from memory, some data might already be buffered in cache, so it'll be much faster. Now for cache,
there are several ways this affects timing -
L1 cache no impact because each core runs its own thread
L2 cache some impact because it is shared by 2 threads
L3 

memory bus would still be bottleneck for read because each thread will need to use busto read from RAM.

So the target here is to understand various performances for various memory locations - registers, cache, RAM
to make design decisions on how to make your program run faster.
Sequential memory access vs non-sequential

vaddpd %ymm0,%ymm1,%ymm2 #ymm2=ymm0+ymm1

7. Why does pipelining on a modern CPU make benchmarking so difficult?
how much can you get doen at the same time
IF, ID, IE - single threading

8. Why doesn't AMD define their own extensions to the instruction set, like more registers?
   AMD has to work on a common instruction set that is used by industry such as x86 or x86-64. Deviating from
   this will lead to compatibility issues. Many OS, compilers and applications are optimized for standard 
   instruction set.
9. Why doesn't a CPU manufacturer design a computer with 1 million registers?
      - Because building registers that are based off of the flip-flop design and on the CPU will be 
      very expensive. 
      - Each register requires a significant amount of silicon real estate and may not be realistic to 
      build such a big chip/motherboard which is not practical for current usage. 
      - Managing one million registers would add immense complexity to the CPU design and instruction set design.
      - Adding more registers makes the instructions bigger (register addresses and number of bits used to 
      address 1million registers), which increases instruction bandwidth needed and increases 
      code size and decreases the i-cache hit rate (for the same size cache). 


Google Drive - https://docs.google.com/document/d/1QA7ofmy6v8HcVPRJm0dCfwtjUXo8ZytChRqXxOdqwgk/edit?tab=t.0

ECE451/566 Test #1 Practice Problems

10. State what effects clock speed has on:
registers - directly propotional
L1 cache - directly propotional
L2 cache - directly propotional
memory - no effect
SSD - no effect

11. What are registers used for?
What is the effect of using the optimizer? XC
variables are associated with registers, the program does not constantly have to load/save from RAM
typically 3-4 times faster, code is much more parallizable

12. If your code only READs from memory, it MIGHT be parallelizable WHY? 
Because READS are safe and will not alter the data so multiple threads can read without any issues.
Threads will cache the values, and not hit RAM but for writing, NO, they cannot be parallelizable.

13. What is cache used for?
Remembering values that are frequently used, 
principle of locality - things you just read, you will probably read again
goal is to have a high “hit rate”

14. What is cache size?
On my machine:
L1 = 256kB (each core has its own)
L2 = 2MB  (each 2 cores share)
L3 = 4MB  (shared by everyone)

15. What happens if you ask for more memory than fits in cache?
This leads to cache miss and will need to be retreieved from RAM

->16. When can your program not use cache? Describe the patterns of use.
reading/writing
reading sequentially/random order
quantity of memory

17. Why does the number of registers affect the amount of parallelism we can achieve? Contrast having 
8 floating point registers vs. 32 registers for the equation:
x = (a * b + c) * sqrt(d * e + f) + sqrt(a+b)*g/h + sin(i)*cos(c)/sqrt(f)
    ------  ---        -----  ---       ---  ---      --     --      ---
         -----         
from a to i, there are a total of 9 variables here which is greater than 8. So, not all variables
can be stored in the 8 floating point registers and will need to be accessed from RAM which is slower compared to registers. The intermediate results for this equation need to be stored in memory to be able to compute the final results. More frequent spills to memory, resulting in slower computations and there is less parallel
execution as registers can't all hold the required intermediate values.

With 32 point floating registers, we can store intermediate results directly in the registers, which will lead
to faster access and will make the program more parallelizable. 

18. Why is it so important to compile with optimization (g++ -O2 test.cpp) when testing parallel code 
for performance?

Optimization flags enable the compiler to generate more efficient code. This means better utilization of CPU resources, reduced instruction count, and faster execution, which is essential when measuring performance.
It might eliminate redundant instructions, optimize loops, and improve memory access patterns. This ensures that the code utilizes the CPU's capabilities more effectively, leading to faster execution.

19. Describe the relative performance of the following and which of the below can be performed in parallel?
addition - yes; very fast
acquiring a semaphore lock - no; slow
multiplication - yes; fast
calling a function - yes; moderate because it involves storing the current PC, registers etc. and control flow
changes. 


20. Order the code from fastest to slowest and explain your reason (one sentence. Do not write a book!).
Assume that all code is compiled, that we make sure the optimizer does not mark is as dead.
For each code, mark it 1 to n. If it can use cache, mark L1, L2, or L3. 
For this problem assume L1 = 128k, L2 = 4MB, L3 = 16MB
see memorytimings.cpp in the repo sessions/reviewtest1

21. What are the features of DDR RAM?
DDR, short for double data rate, was introduced in 2000 as the next generation following SDRAM. DDR transfers data to the processor on both the downbeat and upbeat of the clock signal, so twice per cycle. Using both beats to transfer data makes DDR memory significantly faster than SDR memory, which uses only one edge of the clock signal to transfer data.

22. Is RAM random access? Yes, data can be read from or written to any address in memory directly and at high speed.

23. What is burst mode?
Burst mode helps in read/writing the entire memory (4 0r 8 based on burst size of the processor) based on the start address rather than read/write data specific to the start address. This helps store/write data onto cache and can eaily used in the next read/write cycle, making the consecutive operastions faster. 

24. How many elements in a burst for DDR4, DDR5?
DDR4 burst length = 8, DDR5 burst length = 16

25. Vector (SIMD) programming Single Instruction Multiple Data - AVX

26. Cache hit rate
suppose L1 = 90% hit rate 2clocks, L2 = 5%5clock, L3 = 3%10clock
suppose access memory takes 90 clocks

average access time =    = 1.8+0.25+0.3+1.8 = 4.15 clock cycles

27. Direct Memory Access (DMA)

Definition: DMA is a feature that allows certain hardware subsystems to access main system memory independently of the CPU. 
This means that peripherals can transfer data to and from memory without involving the CPU for each byte transferred.
Usage: DMA is commonly used in scenarios like disk drives, sound cards, and network cards, where large blocks of data need to 
be transferred efficiently.
Performance: By offloading data transfer tasks from the CPU, DMA can significantly improve system performance, especially in 
data-intensive operations, as it frees up CPU resources for other tasks.