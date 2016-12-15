/*
	cpu.c

   Austin Ingraham, Gabriel Summers
	Date: 12/12/16
	Description:
		This file contains the implementation code for the CPU class.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"

/* Constructs an ALU
   OUT: pointer to an ALU
*/
ALU_p constructALU (void) {
	ALU_p alu = (ALU_p) malloc(sizeof(ALU_s));
	return alu;
}

/* Gets the given CPU's ALU
   IN: pointer to a CPU
   OUT: this CPU's ALU
*/
ALU_p getALU(CPU_p cpu) {
   return cpu->alu;
}

/* Gets the given ALU's 'a' Register
   IN: pointer to an ALU
   OUT: Register 'a'
*/
Register getALU_A(ALU_p alu) {
   if (alu == NULL) return POINTER_ERROR;
   return alu->a;
}

/* Gets the given ALU's 'b' Register
   IN: pointer to an ALU
   OUT: Register 'b'
*/
Register getALU_B(ALU_p alu) {
   if (alu == NULL) return POINTER_ERROR;
   return alu->b;
}

/* Gets the given ALU's 'r' (result) Register
   IN: pointer to an ALU
   OUT: Register 'r'
*/
Register getALU_R(ALU_p alu) {
   if (alu == NULL) return POINTER_ERROR;
   return alu->r;
}

/* Performs an ADD operation on this ALU
   IN: pointer to an ALU
   OUT: result Register's value
*/
Register alu_ADD(CPU_p cpu) {
   if (cpu == NULL) return POINTER_ERROR;
   if (cpu->alu == NULL) return POINTER_ERROR;
   cpu->alu->r = cpu->alu->a + cpu->alu->b;
   return cpu->alu->r;
}

/* Constructs and returns a pointer to a CPU. 
   OUT: pointer to a CPU. May be null if malloc fails.
*/
CPU_p constructCPU (void) {
	CPU_p cpu = (CPU_p) malloc (sizeof (CPU_s));
	return cpu;
}

/* Initializes the fields inside a CPU.
   IN: pointer to a CPU
   OUT: fields initialized, POINTER_ERROR returned if given CPU is null
 */
int initCPU (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	cpu->alu = constructALU();
	cpu->pc = 0;
	cpu->ir = 0;
	resetCPU(cpu);
}

/* Gets the IR of the given CPU
   IN: pointer to a CPU
   OUT: Register ir, POINTER_ERROR returned if given CPU is null
 */
Register getIR (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	return cpu->ir;
}

/* Gets the OPCODE of a given CPU from the current ir
   IN: pointer to a CPU
   OUT: Byte OPCODE, POINTER_ERROR returned if given CPU is null
 */
Register getOPCODE (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & OPCODE_MASK;
	temp = temp >> 26;
	return (Register) temp;
}

/* Gets RD of a given CPU from the current ir
   IN: pointer to a CPU
   OUT: Register RD, POINTER_ERROR returned if given CPU is null
*/  
Register getRD (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & RD_MASK;
	temp = temp >> 11;
	return (Register) temp;
}

/* Gets RS of a given CPU from the current ir
   IN: pointer to a CPU
   OUT: Register RS, POINTER_ERROR returned if given CPU is null
*/  
Register getRS (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & RS_MASK;
	temp = temp >> 21;
	return (Register) temp;
}

/* Gets RT of a given CPU from the current ir
   IN: pointer to a CPU
   OUT: Register RT, POINTER_ERROR returned if given CPU is null
*/  
Register getRT (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & RT_MASK;
	temp = temp >> 16;
	return (Register) temp;
}

/* Gets SHFT of a given CPU from the current ir
   IN: pointer to a CPU
   OUT: Register SHFT, POINTER_ERROR returned if given CPU is null
*/  
Register getSHFT (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & SHFT_MASK;
	temp = temp >> 6;
	return (Register) temp;
}

/* Gets FUNC of a given CPU from the current ir
   IN: pointer to a CPU
   OUT: Register FUNC, POINTER_ERROR returned if given CPU is null
*/ 
Register getFUNC (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & FUNC_MASK;
	return (Register) temp;
}

/* Gets IMMED of a given CPU from the current ir
   IN: pointer to a CPU
   OUT: Register IMMED, POINTER_ERROR returned if given CPU is null
*/ 
Register getIMMED (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & IMMED_MASK;
	return (Register) temp;
}

/* Gets a register from the given index
   IN: pointer to a CPU, index of register to get
   OUT: a register
*/
Register getRegister(CPU_p cpu, int index) {
   if (cpu == NULL) return POINTER_ERROR;
   if (index >= NO_OF_REGISTERS || index < 0) return INDEX_OUT_OF_BOUNDS;
   return cpu->reg_file[index];
}

int setSW(CPU_p cpu, int s) {
   if (cpu == NULL) return POINTER_ERROR;
   if (s >= 0x8000) { cpu->sw = N_MASK; }
   if (s == 0) { cpu->sw = Z_MASK; }
   if (s > 0 && s < 0x8000) { cpu->sw = P_MASK; }
   return 1;
}

/* Sets a given CPU's ir Register
   IN: pointer to a CPU, char input to set
   OUT: 1 if set operation is successful, -1 if POINTER_ERROR
 */
int setIR(CPU_p cpu, char* input) {
   if (cpu == NULL) return POINTER_ERROR;
   cpu->ir = strtol(input, NULL, 16);
   return 1;
}

void trapHalt(CPU_p cpu) {
	printf("\nINSTRUCTION HALTED\n");
}

/* Sets a given CPU's Register at given index
   IN: pointer to a CPU, unsigned integer input to set, index of register
   OUT: 1 if set operation is successful, -1 if POINTER_ERROR
 */
int setRegister(CPU_p cpu, unsigned int value, int index) {
   if (cpu == NULL) return POINTER_ERROR;
   cpu->reg_file[index] = value;
   return 1; 
}

int resetCPU(CPU_p cpu) {
   if (cpu == NULL) return POINTER_ERROR;
   cpu->pc = 0;
   cpu->sw = 0;
   cpu->ir = 0;
   cpu->mar = 0;
   cpu->mdr = 0;
   //cpu->sw = Z_MASK;
   cpu->alu->a = 0;
   cpu->alu->b = 0;
   cpu->alu->r = 0;
   return 1;
}

/* Prints the binary representation of this register (32-bit). */
void displayRegisterBinary(Register theR) {
    int i;
    for (i = 31; i >= 0; i--) {       
       printf("%d", (theR & (1 << i)) >> i);
       if(i % 4 == 0 && i > 1) {
          printf(" ");  
       }
    }
   printf("\n");
}

/* Prints the binary representation of this char. */
void displayByteBinary(Byte theB) {
    int i;
    for (i = 7; i >= 0; i--) {
        printf("%d", (theB & (1 << i)) >> i);
    }
   printf("\n");
}
