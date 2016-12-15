/*
	cpu.h

   Austin Ingraham, Gabriel Summers
	Date: 12/12/16
	Description:
		This header file contains the implementation details of the CPU object.
*/

#include <stdio.h>
#include <stdint.h>

#ifndef _CPU_H_
#define _CPU_H_

#define POINTER_ERROR -1
#define INDEX_OUT_OF_BOUNDS -1
#define FILE_ERROR -1

#define NO_OF_REGISTERS 32

/* Status word masks */
#define N_MASK 0x0004
#define Z_MASK 0x0002
#define P_MASK 0x0001

/* Sign-Extension codes */
#define RET_REG 31
#define OPCODE_MASK 0xFC000000
#define RD_MASK 0x0000F800
#define RS_MASK 0x03E00000
#define RT_MASK	0x001F0000
#define FUNC_MASK 0x0000003F
#define SHFT_MASK 0x000007C0
#define MEM_SIZE 0xFFFF
#define IMMED_MASK 0x0000FFFF
#define TARGET_MASK 0x03FFFFFF

/* OPCODES */
#define ADD	0
#define LA	15
#define LW	35
#define SW	43

#define HALT 37

typedef unsigned int Register;
typedef unsigned char Byte;

typedef struct alu_s {
	Register  a, b, r;
} ALU_s;

typedef ALU_s * ALU_p;

typedef struct cpu_s {
	Register reg_file[NO_OF_REGISTERS];
	ALU_p alu;
	Register ir, pc, sw; 	// IR, PC, and SW (status word)
	Register mar, mdr;
} CPU_s;

typedef CPU_s * CPU_p;

//PROTOTYPES
//ALU
ALU_p constructALU(void);
ALU_p getALU(CPU_p);
Register getALU_A(ALU_p);
Register getALU_B(ALU_p);
Register getALU_R(ALU_p);
Register alu_ADD(CPU_p);

//CPU
CPU_p constructCPU(void);
int initCPU (CPU_p);
int resetCPU(CPU_p);
int setIR (CPU_p, char*);
int setRegister(CPU_p, unsigned int, int);
Register getIR(CPU_p);
Register getRD (CPU_p cpu);
Register getRS (CPU_p cpu);
Register getRegister(CPU_p, int);
Register getOPCODE(CPU_p);

int takeBranch(CPU_p, Register);
int setSW(CPU_p, int);

//Trap
void trapHalt(CPU_p);

//Utility functions
void displayRegisterBinary(Register);
void displayByteBinary(Byte);
void purgeBuffer();

//Debug and simulator initialization
int debug(CPU_p);
void displayMemory(CPU_p, int);
void initRegisters(CPU_p);
void initMemory();
FILE* openFile(char[]);
FILE* getInputFile();
//int loadData(FILE*);

#endif
