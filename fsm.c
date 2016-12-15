/*
	fsm.c
	
   Austin Ingraham
   Gabriel Summers
	Date: 12/12/16
	Description:
		This file contains the implementation code for simulating a Finite State Machine
        based on the MIPs language.
*/

#include "cpu.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FETCH 0
#define DECODE 1
#define EXECUTE 2
#define MEMACCESS 3
#define WRITEBACK 4
#define HALTINSTRUCTION 5
#define MEMORY_SIZE 0xFFFF

unsigned int memory[MEMORY_SIZE]; 

void testProgram1() {
    memory[0] = 0x8C280000;
    memory[1] = 0x8C290004;
    memory[2] = 0x01095020;
    memory[3] = 0xAC2A0008;
}

void testProgram2(CPU_p cpu) {
    cpu->reg_file[8] = 2;
    cpu->reg_file[9] = 3;
    cpu->reg_file[10] = 7;
    memory[0] = 0x812a4020;
}

void testProgram3(CPU_p cpu) {
    cpu->reg_file[8] = 2;
    cpu->reg_file[9] = 3;
    cpu->reg_file[10] = 7;
    cpu->reg_file[11] = 4;
    memory[0] = 0x014b4820;
}

void testProgram4(CPU_p cpu) {
    cpu->reg_file[8] = 2;
    cpu->reg_file[9] = 3;
    cpu->reg_file[10] = 7;
    cpu->reg_file[11] = 4;
    memory[0] = 0x8C08000A;
}

void testProgram5(CPU_p cpu) {
    //memory[0] = 0x8C08002C;
    memory[1] = 0x8C090030;
    memory[2] = 0x014b4820;
    memory[10] = 0xFFFFFFFF;
    memory[11] = 15;
    memory[12] = 9;
}

void testProgram6(CPU_p cpu) {
    //memory[0] = 0x8C08002C;
    memory[0] = 0x8C0A002C;
    memory[1] = 0x8C0B0030;
    memory[2] = 0x014b4820;
    memory[3] = 0xAC090034; //store register 9 into memory address 13
    memory[4] = 0xFFFFFFFF;
    memory[11] = 15;
    memory[12] = 9;
}

void purgeBuffer() {
   char c; 
   c = getchar();

   while(c != '\n' && c != EOF) {
      c = getchar();  
   }
}

void displayMemory(CPU_p cpu, int m) {
   int i = 0;
   char c = 'N';
   printf("\nRegisters\tMemory\n==================================\n");
   for(i; i < NO_OF_REGISTERS; i++, m += 2) {
      printf("R%d: %.8X\t", i, getRegister(cpu, i));
      printf("0x%.4X: %.8X  %.8X\n", m, memory[m], memory[m + 1]);
   }
   if (cpu->sw == 1) { 
      c = 'P'; 
   } else if (cpu->sw == 2) {
      c = 'Z';      
   } 
   printf("\nPC: %.4X   SW: %c   IR: %.8X\n", cpu->pc, c, getIR(cpu));
}

FILE* openFile(char filename[]) {
    FILE* file = fopen(filename, "r");

    if (!file) {
        printf("\tCould not open filename \"%s\"\n\n", filename);
        return getInputFile();
    } else {
        return file;
    }
}

FILE* getInputFile() {
    int i = 0, size = 50; //max allowed filename
    char ch, filename[size];  

    printf("\tEnter the name of the file to load:\n\t> ");
    scanf("%c", &ch);
    while (ch != '\n' && i < size - 1) { //builds string from chars entered
        filename[i] = ch;
        ch = getchar();
        i++;
    }    
    filename[i] = '\0';
    return openFile(filename);
}

int loadData() {
	int i = 0;
   char line[10];
   FILE* file = getInputFile();
   printf("\tFound the file, loading data... ");
   // fgets(line, sizeof(line), file);
   // Register startingIndex = strtol(line, NULL, 16);
   // int i = startingIndex;
   while (fgets(line, sizeof(line), file) && i < MEMORY_SIZE) {
      memory[i] = strtoul(line, NULL, 16);
      i++;
   }
   printf("done.\n");
   //return startingIndex;  
   return 1;
}

int saveData(int m) {
	FILE* outFile = fopen("output.hex", "w");
	int i = 0;
	if (!outFile) {
    	printf("\tERROR OPENING FILE\n");
		return FILE_ERROR;
    }
    for(i; i <= m; i++) {
	   fprintf(outFile, "%.8X\n", memory[i]); 		   
    }
	fclose(outFile);
	return 1;
}

int debug(CPU_p cpu) {
   int m = cpu->pc, input = 8, repeat = 1, run = 0;;
   unsigned short reg = 0;
   char* memoryLoc = malloc(sizeof(char) * 20);
   displayMemory(cpu, m);
   while(input > 0) {      
      if (m > (MEMORY_SIZE - 16)) {
         m = MEMORY_SIZE - 16;   
      }
      printf("\n1 Load, 2 Save, 3 Run, 4 Step, 5 Memory Dump, 6 Memory Edit, 7 Reset\n> ");
      scanf(" %d", &input);
      purgeBuffer();
      switch(input) {
         case 1: //LOAD OBJECT FILE
			loadData();
			displayMemory(cpu, m);
            // cpu->pc = loadData();   
            // displayMemory(cpu, cpu->pc);
            break;
		 case 2:
			printf("\tChoose terminating index of memory to output, (0 - %d)\n\t> ", (MEMORY_SIZE - 1));
			scanf(" %d", &m);
            if(m < 0) {
               m = 0;
            } else if (m > (MEMORY_SIZE - 1)) {
               m = MEMORY_SIZE - 1;   
            }
			printf("\tSaving memory into output.hex ... ");
			saveData(m);
			printf("done.\n");
			break;
		 case 3:
		    printf("\tRunning...\n");
			run = 1;
			input = 0;
			break;
         case 4: //STEP
            input = 0;
            break;
         case 5: //MEMORY DUMP
            printf("\tChoose memory dump starting index, (0 - %d)\n\t> ", (MEMORY_SIZE - 16));
            scanf(" %d", &m);
            if(m < 0) {
               m = 0;
            } else if (m > (MEMORY_SIZE - 16)) {
               m = MEMORY_SIZE - 16;   
            }
            displayMemory(cpu, m);

            break;
         case 6: //MEMORY EDIT
            repeat = 1;
            while(repeat) {
               printf("\tEnter memory location to edit,\n\t> ");
               scanf(" %s", memoryLoc);
               m = strtol(memoryLoc, NULL, 0);
               if(m < 0 || m >= MEMORY_SIZE) {
                  printf("\tIndex out of range, please enter value between 0 and %d.\n",
                         (MEMORY_SIZE - 1));
               } else {
                  printf("\t%d/%.4X location chosen. Please enter new value:\n\t> ", m, m);
                  scanf(" %s", memoryLoc);
                  memory[m] = strtol(memoryLoc, NULL, 16);
                  printf("\tContinue editing data? y/n\n\t> ");
                  scanf(" %s", memoryLoc);
                  if(memoryLoc[0] == 'y' || memoryLoc[0] == 'Y') {} else { repeat = 0;}
               }
            }
            
            break;
         case 7: //RESET
            printf("\tResetting simulator, y to confirm,\n\t> ");
            scanf(" %s", memoryLoc);
            if(memoryLoc[0] == 'y' || memoryLoc[0] == 'Y') {
               printf("\tResetting... ");
               initMemory();
               initRegisters(cpu);
               resetCPU(cpu);
               printf("done.\n");
               displayMemory(cpu, m);
            } 
            break;
         default:
            printf("\tInvalid choice, please enter value 1-7\n");
            input = 8;
            break;
      }
      
   }
   free(memoryLoc);
   return run;
}

void initRegisters(CPU_p cpu) {
   int i = 0;
   for(i; i < NO_OF_REGISTERS; i++) {
	   cpu->reg_file[i] = 0;	
   }
}

void initMemory() {
   int i = 0;
   for(i; i < MEMORY_SIZE; i++) {
	   memory[i] = 0x00000000;	
   }
   memory[i] = 0xFFFFFFFF;
}

//=========================================================================================
int controller (CPU_p cpu) {
    if (cpu == NULL) return POINTER_ERROR;
    if (cpu->alu == NULL) return POINTER_ERROR;
	 unsigned int state = FETCH;
	 Byte opcode = 0, rd = 0, rs = 0, rt = 0, immed;
	 int run = 0;
	 initMemory();
	 initRegisters(cpu);
     
     testProgram6(cpu);
      
    for(;;) {   // efficient endless loop
        switch (state) {
            case FETCH: 
				if (run == 0) {
					run = debug(cpu);
				}
                cpu->mar = cpu->pc;  
                cpu->mdr = memory[cpu->mar];   
                cpu->ir = cpu->mdr;      
                cpu->pc++;       
                state = DECODE;
                break;
            case DECODE:
                opcode = getOPCODE(cpu);
                switch (opcode) {
                    case ADD:
                        printf("ADD operation\n");
                        rd = getRD(cpu);
                        rs = getRS(cpu);
                        rt = getRT(cpu);
                        printf("rd: %d, rs: %d, rt: %d\n", rd, rs, rt);
                        break;
                    case LW:
                        printf("LW operation\n");
                        rs = getRS(cpu);			
                        rt = getRT(cpu);
                        immed = getIMMED(cpu);
                        printf("rs: %d, rt: %d, immed: %d\n", rs, rt, immed);
                        break;
                    case LA:		
                        printf("LA operation\n");
                        rt = getRT(cpu);
                        immed = getIMMED(cpu);
                        break;
                    case SW:
                        printf("SW operation\n");
                        rt = getRT(cpu);
                        rs = getRS(cpu);
                        immed = getIMMED(cpu);
                        break;
                    default: break;
                }
                state = EXECUTE;
                break;
            case EXECUTE:
                switch (opcode) {
                   case ADD: 
                      cpu->alu->a = cpu->reg_file[rs];
                      cpu->alu->b = cpu->reg_file[rt];
                      cpu->mdr = alu_ADD(cpu);
                      break;
                   case LW:
                      printf("rs: %d\n", rs);
                      cpu->alu->a = cpu->reg_file[rs];
                      cpu->alu->b = immed/4;
                      cpu->mar = alu_ADD(cpu);
                      break;
                   case LA:
                      cpu->mar = immed;
                      break;
                   case SW:
                      cpu->alu->a = cpu->reg_file[rs];
                      cpu->alu->b = immed/4;
                      cpu->mar = alu_ADD(cpu);
                      break;
                   default: break;
                }
              
                state = MEMACCESS;
                break;
            case MEMACCESS:
                switch (opcode) {
                   case ADD: break;
                   case LW:
                      /*Load the target register with the data in cpu->mdr. */
                      cpu->mdr = memory[cpu->mar];
                      printf("mar: %d, mdr: %d\n", cpu->mar, cpu->mdr);
                      break;
                   case LA:
                      /*Load the target register with the value of alu->r. */
                      cpu->mdr = memory[getALU_R(cpu->alu)];
                      break;
                   case SW:
                      memory[cpu->mar] = cpu->reg_file[rt];
                      printf("mar: %d, rt: %d\n", cpu->mar, rt);
                      break;
                   default: break;
                }

                state = WRITEBACK;
                break;
            case WRITEBACK:
                switch (opcode) {
                   case ADD:
                      cpu->reg_file[rd] = cpu->mdr;
                      break;
                   case LW:
                      cpu->reg_file[rt] = cpu->mdr;
                      printf("rt: %d\n", rt);
                      break;
                   case LA:
                      cpu->reg_file[rd] = cpu->mar;
                      break;
                   case SW: break;
                   default:
                      if(cpu->ir == 0xFFFFFFFF){
                          printf("HALT REACHED\n");
                          exit(0);
                      }
                      break;
                }                        
                //zero-out everything                          
                rd = 0;
                rs = 0;
                rt = 0;
                opcode = 0;
                cpu->alu->a = 0;
                cpu->alu->b = 0;
                state = FETCH;   
                break;
        }
    }
    displayMemory(cpu, 0);
    return 1;
}


/* Driver for FSM */
int main (int argc, char *argv[]) {
    //PREPARE CPU 
    CPU_p cpu = constructCPU();
    initCPU(cpu);

    if(controller(cpu) == 1) {
        printf("Operations completed successfully.\n");	
    } else {
        printf("Something went wrong.\n");
    } 

    return 0;
}
