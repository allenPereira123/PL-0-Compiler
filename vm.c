// Allen Pereira
// Allec Pereira.

#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#define MAX_STACK_HEIGHT 50
#define MAX_CODE_LENGTH 100

int base(int stack[], int level, int BP)
{
	int base = BP;
	int counter = level;
	while (counter > 0)
	{
		base = stack[base];
		counter--;
	}
	return base;
}

void execute(instruction *code, int vflag)
{
	//FILE * file_ptr = fopen(argv[1], "r");
	//instruction code[MAX_CODE_LENGTH];
	int static_links[MAX_STACK_HEIGHT];
	int stack[MAX_STACK_HEIGHT];
	int count = 0;
	int sp = -1;
	int bp = 0;
	int old_pc = 0;
	int pc = 0;
	int halt = 1;
	instruction *IR = NULL;

	/*// read in instructions into an array of structs
  while (feof(file_ptr) == 0)
  {
    fscanf(file_ptr, "%d %d %d", &code[count].opcode, &code[count].l, &code[count].m);
    count++;

  }*/
	// initialize stack array and static links array to keep track of the position of the static links
	for (int i = 0; i < MAX_STACK_HEIGHT; i++)
	{
		static_links[i] = 0;
		stack[i] = 0;
	}

	if (vflag)
	{
		printf("\t\t  PC\tBP\tSP\tstack\n");
		printf("Initial values:  %2d\t%2d\t%2d\n", pc, bp, sp);
	}

	while (halt)
	{
		// fetch cycle
		old_pc = pc;
		pc++;
		IR = &code[old_pc];
		// Execution Cycle
		switch (IR->opcode)
		{
		// push constant value on top of stack
		case 1:
			sp++;
			stack[sp] = code[old_pc].m;
			if (vflag)
			{
				printf("%2d LIT %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
				printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
			}

			for (int i = 0; i <= sp; i++)
			{
				if (vflag)
				{
					if (static_links[i] == 1)
						printf("|");
					printf("%d", stack[i]);
				}
			}
			if (vflag)
				printf("\n");
			break;
		// Operation to be performed on data at the top of the stack
		case 2:
			switch (code[old_pc].m)
			{
			// Return to caller
			case 0:
				stack[bp - 1] = stack[sp];
				sp = bp - 1;
				static_links[bp] = 0;
				bp = stack[sp + 2];
				if (vflag)
					printf("%2d OPR %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
				pc = stack[sp + 3];

				if (vflag)
					printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);

				for (int i = 0; i <= sp; i++)
				{
					if (vflag)
					{
						if (static_links[i])
							printf("|");
						printf("%d", stack[i]);
					}
				}
				if (vflag)
					printf("\n");
				break;

			// switch the sign of the number at the top of the stack
			case 1:
				stack[sp] = -1 * stack[sp];
				if (vflag)
				{
					printf("%2d OPR %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
					printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
				}

				for (int i = 0; i <= sp; i++)
				{
					if (vflag)
					{
						if (static_links[i])
							printf("|");
						printf("%d", stack[i]);
					}
				}
				if (vflag)
					printf("\n");
				break;

			// add two numbers that are on top of stack
			case 2:
				sp = sp - 1;
				stack[sp] = stack[sp] + stack[sp + 1];
				if (vflag)
				{
					printf("%2d OPR %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
					printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
				}

				for (int i = 0; i <= sp; i++)
				{
					if (vflag)
					{
						if (static_links[i])
							printf("|");
						printf("%d", stack[i]);
					}
				}
				if (vflag)
					printf("\n");
				break;

			// subtract
			case 3:
				sp = sp - 1;
				stack[sp] = stack[sp] - stack[sp + 1];
				if (vflag)
				{
					printf("%2d OPR %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
					printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
				}

				for (int i = 0; i <= sp; i++)
				{
					if (vflag)
					{
						if (static_links[i])
							printf("|");
						printf("%d", stack[i]);
					}
				}

				if (vflag)
					printf("\n");
				break;

			// multiply
			case 4:
				sp = sp - 1;
				stack[sp] = stack[sp] * stack[sp + 1];
				if (vflag)
				{
					printf("%2d OPR %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
					printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
				}

				for (int i = 0; i <= sp; i++)
				{
					if (vflag)
					{
						if (static_links[i])
							printf("|");
						printf("%d", stack[i]);
					}
				}
				if (vflag)
					printf("\n");
				break;

			// divide
			case 5:
				sp = sp - 1;
				stack[sp] = stack[sp] / stack[sp + 1];
				if (vflag)
				{
					printf("%2d OPR %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
					printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
				}

				for (int i = 0; i <= sp; i++)
				{
					if (vflag)
					{
						if (static_links[i])
							printf("|");
						printf("%d", stack[i]);
					}
				}

				if (vflag)
					printf("\n");
				break;

			// odd
			case 6:
				stack[sp] = stack[sp] % 2;

				if (vflag)
				{
					printf("%2d OPR %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
					printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
				}

				for (int i = 0; i <= sp; i++)
				{
					if (vflag)
					{
						if (static_links[i])
							printf("|");
						printf("%d", stack[i]);
					}
				}
				if (vflag)
					printf("\n");
				break;

			// MOD
			case 7:
				sp = sp - 1;
				stack[sp] = stack[sp] % stack[sp + 1];

				if (vflag)
				{
					printf("%2d OPR %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
					printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
				}

				for (int i = 0; i <= sp; i++)
				{
					if (vflag)
					{
						if (static_links[i])
							printf("|");
						printf("%d", stack[i]);
					}
				}
				if (vflag)
					printf("\n");
				break;

			// EQL
			case 8:
				sp = sp - 1;
				stack[sp] = stack[sp] == stack[sp + 1];

				if (vflag)
				{
					printf("%2d OPR %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
					printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
				}

				for (int i = 0; i <= sp; i++)
				{
					if (vflag)
					{
						if (static_links[i])
							printf("|");
						printf("%d", stack[i]);
					}
				}
				if (vflag)
					printf("\n");
				break;

			// NEQ
			case 9:
				sp = sp - 1;
				stack[sp] = stack[sp] != stack[sp + 1];

				if (vflag)
				{
					printf("%2d OPR %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
					printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
				}
				for (int i = 0; i <= sp; i++)
				{
					if (vflag)
					{
						if (static_links[i])
							printf("|");
						printf("%d", stack[i]);
					}
				}

				if (vflag)
					printf("\n");
				break;

			// LSS
			case 10:
				sp = sp - 1;
				stack[sp] = stack[sp] < stack[sp + 1];

				if (vflag)
				{
					printf("%2d OPR %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
					printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
				}

				for (int i = 0; i <= sp; i++)
				{
					if (vflag)
					{
						if (static_links[i])
							printf("|");
						printf("%d", stack[i]);
					}
				}

				if (vflag)
					printf("\n");
				break;

			// LEQ
			case 11:
				sp = sp - 1;
				stack[sp] = stack[sp] <= stack[sp + 1];

				if (vflag)
				{
					printf("%2d OPR %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
					printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
				}

				for (int i = 0; i <= sp; i++)
				{
					if (vflag)
					{
						if (static_links[i])
							printf("|");
						printf("%d", stack[i]);
					}
				}

				if (vflag)
					printf("\n");
				break;

			// GTR
			case 12:
				sp = sp - 1;
				stack[sp] = stack[sp] > stack[sp + 1];

				if (vflag)
				{
					printf("%2d OPR %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
					printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
				}

				for (int i = 0; i <= sp; i++)
				{
					if (vflag)
					{
						if (static_links[i])
							printf("|");
						printf("%d", stack[i]);
					}
				}

				if (vflag)
					printf("\n");
				break;

			// GEQ
			case 13:
				sp = sp - 1;
				stack[sp] = stack[sp] >= stack[sp + 1];

				if (vflag)
				{
					printf("%2d OPR %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
					printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
				}

				for (int i = 0; i >= sp; i++)
				{
					if (vflag)
					{
						if (static_links[i])
							printf("|");
						printf("%d", stack[i]);
					}
				}

				if (vflag)
					printf("\n");
				break;
			}
			break;

		// Load number to the top of the stack
		case 3:
			sp = sp + 1;
			stack[sp] = stack[base(stack, code[old_pc].l, bp) + code[old_pc].m];

			if (vflag)
			{
				printf("%2d LOD %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
				printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
			}

			for (int i = 0; i <= sp; i++)
			{
				if (vflag)
				{
					if (static_links[i])
						printf("|");
					printf("%d", stack[i]);
				}
			}

			if (vflag)
				printf("\n");
			break;

		// Store number that is on top of the stack at the specified location
		case 4:
			stack[base(stack, code[old_pc].l, bp) + code[old_pc].m] = stack[sp];
			sp = sp - 1;

			if (vflag)
			{
				printf("%2d STO %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
				printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
			}

			for (int i = 0; i <= sp; i++)
			{
				if (vflag)
				{
					if (static_links[i])
						printf("|");
					printf("%d", stack[i]);
				}
			}

			if (vflag)
				printf("\n");
			break;

		// function call
		case 5:
			stack[sp + 1] = base(stack, code[old_pc].l, bp);
			static_links[sp + 1] = 1;
			stack[sp + 2] = bp;
			stack[sp + 3] = old_pc + 1;
			stack[sp + 4] = stack[sp];
			bp = sp + 1;

			if (vflag)
				printf("%2d CAL %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
			pc = code[old_pc].m;
			if (vflag)
				printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);

			for (int i = 0; i <= sp; i++)
			{
				if (vflag)
				{
					if (static_links[i])
						printf("|");
					printf("%d", stack[i]);
				}
			}

			if (vflag)
				printf("\n");
			break;

		// increment stack pointer by specified amount
		case 6:
			sp += code[old_pc].m;

			if (vflag)
			{
				printf("%2d INC %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
				printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
			}

			for (int i = 0; i <= sp; i++)
			{
				if (vflag)
				{
					if (static_links[i] == 1)
						printf("|");
					printf("%d", stack[i]);
				}
			}

			if (vflag)
				printf("\n");
			break;

		// jump to specified instruction
		case 7:
			if (vflag)
				printf("%2d JMP %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
			pc = code[old_pc].m;

			if (vflag)
				printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);

			for (int i = 0; i <= sp; i++)
			{
				if (vflag)
				{
					if (static_links[i] == 1)
						printf("|");
					printf("%d", stack[i]);
				}
			}

			if (vflag)
				printf("\n");
			break;

		// if top of stack is zero, jump to specified instruction
		case 8:
			if (vflag)
				printf("%2d JPC %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
			if (stack[sp] == 0)
				pc = code[old_pc].m;

			sp = sp - 1;

			if (vflag)
				printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
			for (int i = 0; i <= sp; i++)
			{
				if (vflag)
				{
					if (static_links[i] == 1)
						printf("|");

					printf("%d", stack[i]);
				}
			}

			if (vflag)
				printf("\n");
			break;

		// SYS
		case 9:
			switch (code[old_pc].m)
			{
			// print value that is at the top of stack
			case 1:
				if (vflag)
				{
					printf("Top of Stack Value: %d\n", stack[sp]);
					sp--;
					if (vflag)
					{
						printf("%2d SYS %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
						printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
					}

					for (int i = 0; i <= sp; i++)
					{
						if (vflag)
						{
							if (static_links[i] == 1)
								printf("|");
							printf("%d", stack[i]);
						}
					}

					printf("\n");
				}

				else
				{
					printf("Top of Stack Value: %d\n", stack[sp]);
					sp--;
				}
				break;

			// read in value from user and add it to the top of the stack
			case 2:
				sp++;
				if (vflag)
				{
					printf("Please Enter an Integer: ");
					scanf("%d", &stack[sp]);
					if (vflag)
					{
						printf("\n");
						printf("%2d SYS %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
						printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
					}

					for (int i = 0; i <= sp; i++)
					{
						if (vflag)
						{
							if (static_links[i] == 1)
								printf("|");
							printf("%d", stack[i]);
						}
					}

					printf("\n");
				}

				else
				{
					printf("Please Enter an Integer: ");
					scanf("%d", &stack[sp]);
					//printf("\n");
				}
				break;

			// ends program
			case 3:
				if (vflag)
				{
					printf("%2d SYS %2d %2d", old_pc, code[old_pc].l, code[old_pc].m);
					printf("\t %2d\t%2d\t%2d\t", pc, bp, sp);
				}

				for (int i = 0; i <= sp; i++)
				{
					if (vflag)
					{
						if (static_links[i] == 1)
							printf("|");
						printf("%d", stack[i]);
					}
				}

				if (vflag)
					printf("\n");
				halt = 0;
				break;
			}
			break;
		}
	}
}
