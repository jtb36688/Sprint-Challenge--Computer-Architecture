#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_LEN 1024
#define SP cpu->registers[7]

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, char *filename)
{
  // TODO: Replace this with something less hard-coded
FILE *fp;
char line[DATA_LEN];
int address = 0; // current index in memory
fp = fopen(filename, "r");

if (fp == NULL) {
  fprintf(stderr, "Provided filename does not exist in this directory");
}


while (fgets(line, DATA_LEN, fp) != NULL) {
  char *endptr;
  unsigned char v = strtoul(line, &endptr, 2); // cant use a pointer for 2nd arg because it would be a copy
  //&endptr points at the actual value in memory. This converts it to base 2

  if (endptr == line) { // Line must have a blank, so it ignores the line
    continue;
  }
  cpu->memory[address] = v;
  address++;

}

fclose(fp);
}

unsigned char cpu_ram_read(struct cpu *cpu, int address)
{
  return cpu->memory[address];
}

void cpu_ram_write(struct cpu *cpu, int address, char data)
{
  cpu->memory[address] = data;
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op) {
    case ALU_MUL:
      // TODO
      cpu->registers[regA] = cpu->registers[regA] * cpu->registers[regB];
      break;

    // TODO: implement more ALU ops
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction

  while (running) {
    unsigned char ir = cpu_ram_read(cpu, cpu->pc);
    unsigned char operand1 = cpu_ram_read(cpu, cpu->pc+1);
    unsigned char operand2 = cpu_ram_read(cpu, cpu->pc+2);
    int num_increments = (ir >> 6) + 1;
    int v;
    int reg;
    unsigned char retaddr;
    switch (ir) {
      case CALL:
        retaddr = cpu->pc + 2;
        SP--;
        // reg = operand1;
        // cpu->memory[cpu->registers[SP]] = retaddr;
        // cpu->pc = cpu->registers[reg];
        cpu->memory[cpu->registers[SP]] = retaddr;
        cpu->pc = cpu->registers[operand1];
        break;
      case RET:
        // cpu->pc = cpu_ram_read(cpu, SP);
        retaddr = cpu->memory[cpu->registers[SP]];
        cpu->registers[reg] = retaddr;
        SP++;
        cpu->pc = retaddr;
        break;
      case JMP:
        cpu->pc = cpu->registers[operand1];
        break;
      case PUSH:
        SP--;
        reg = operand1;
        v = cpu->registers[reg];
        cpu->memory[cpu->registers[SP]] = v;
        break;
      case POP:
        reg = operand1;
        v = cpu->memory[cpu->registers[SP]];
        cpu->registers[reg] = v;
        SP++;
        break;
      case HLT:
        running = 0;
        break;
      case LDI:
        reg = operand1;
        v = operand2;
        cpu->registers[reg] = v;
        break;
      case PRN:
        reg = operand1;
        printf("%d", cpu->registers[reg]);
        break;
      case MUL:
        alu(cpu, ALU_MUL, operand1, operand2);
        break;
      default:
        printf("Unknown instruction %02x at %02x\n", ir, cpu->pc);
        exit(1);
      
    }
    if (ir != CALL && ir != RET && ir != JMP) {
      cpu->pc += num_increments;
    }
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    // 2. Figure out how many operands this next instruction requires
    // 3. Get the appropriate value(s) of the operands following this instruction
    // 4. switch() over it to decide on a course of action.
    // 5. Do whatever the instruction should do according to the spec.
    // 6. Move the PC to the next instruction.
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  cpu->pc = 0;
  memset(cpu->registers, 0, 8);
  // set memory array to be all NULL or 0 values.
  memset(cpu->memory, 0, 256);
  // set stack pointer to be 7
  SP = 0xF4;
}
