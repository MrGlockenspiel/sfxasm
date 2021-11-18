// Source file for sfxasm compiler
// (c) mohhingman
// v1.1

// Updated by MrGlockenspiel

#include "sfx.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

  int end_of_file = 0;
  int program_counter = 0;
  int old_program_counter = 0;
  int label_index = 0;
  const int maxinstructionsize = 8;
  const int numberofinstructions = 86;
  const int maxnumberoflabels = 512;
  const int maxlabelsize = 32;
  const int maxargumentsize = 32;
  char label_names[maxnumberoflabels][maxlabelsize];
  long unsigned int label_addresses[maxnumberoflabels];

  int verbosemode = 1;
  int i = 0;
  int linedone = 0;
  int line_number = 0;
  char current_label[32];
  int argconvert(char *argument_string);

  char output_string[60];

  printf("Nintendo SuperFX assembler 1.0 - (c) 2015 mohhingman\n");
  if (argc <= 2) {
    printf("usage: sfxasm sourcefile outputfile\n");

    return 0;
  }

  FILE *fp, *of;
  int pass = 1;

  while (pass <= 2) {
    program_counter = 0;
    linedone = 0;
    line_number = 0;

    fp = fopen(argv[1], "r");

    of = fopen(argv[2], "w+");

    if (verbosemode == 1) {
      printf("\nStarting compilation of '%s' into binary file '%s'...\n\n",
             argv[1], argv[2]);
    }

    if (fp == 0) {
      printf("Error opening source file\n");
      //	fclose(of);
      return 0;
    }
    char current_line[60];

    while (end_of_file == 0) {

      if (fgets(current_line, 60, fp) != NULL) {
        printf("");
      } else {
        end_of_file = 1;
        break; // end_of_file=1;
      }
      line_number++;
      // 1 indent
      // Begin processing - first pass

      // comment directive (;) or newline
      if (current_line[0] == ';' || current_line[0] == '\n') {
        linedone = 1;
        // else it's an instruction or spam
      } else { // 2 deep
        // Begin lookup in the SFX database
        char current_instruction[10];
        char current_operation[10];
        int i = 0;
        int j = 0;
        int passes = 0;
        int nomorearguments = 0;
        int arg_start = 0;
        linedone = 0;
        int x = 0;
        char arg_1[32];
        char arg_2[32];
        int arg_1_valid = 0;
        int arg_2_valid = 0;
        int arg_1_ispointer = 0;
        int arg_2_ispointer = 0;

        arg_1[0] = '\0';
        arg_2[0] = '\0';
        while (i < maxinstructionsize && linedone == 0) {

          current_instruction[i] = current_line[i];

          if (current_line[i] == ' ') // at least 1 argument
          {
            arg_start = i + 1;

            current_instruction[i] = '\0'; // lock in instruction
            // collect arguments
            for (j = 0; j < maxargumentsize; j++) {
              arg_1[j] = current_line[arg_start + j];
              if (arg_1[j] == ',') // 2 arguments
              {
                arg_1[j] = '\0';
                arg_start = arg_start + j + 1;
                arg_1_valid = 1;
                nomorearguments = 0;
                break;
              } else if (arg_1[j] == '\n' || arg_1[j] == ';') {
                arg_start = 0;
                arg_1[j] = '\0';
                nomorearguments = 1;
                arg_1_valid = 1;
                arg_2_valid = 0;
                break;
              }
              if (j == (maxargumentsize - 1)) {
                printf("FATAL ERROR: Argument too long at Line %d",
                       line_number);
                fclose(fp);
                fclose(of);
                return 0;
              }
            }

            if (nomorearguments == 0) {

              for (j = 0; j < maxargumentsize; j++) {
                arg_2[j] = current_line[arg_start + j];
                if (current_line[arg_start + j] == '\n') {
                  arg_2[j] = '\0';
                  arg_start = j;
                  nomorearguments = 1;
                  arg_2_valid = 1;
                  break;
                }
                if (j == (maxargumentsize - 1)) {
                  printf("FATAL ERROR: Argument too long at Line %d",
                         line_number);
                  fclose(fp);
                  fclose(of);
                  return 0;
                }
              }
            }
          } else if (current_line[i] == '\n') // no arguments
          {
            nomorearguments = 1;
            arg_start = 0;
            current_instruction[i] = '\0';
            linedone = 1;
            break;
          }
          i++;
        }
        char tempstring[32];
        if (pass == 2) {

          if (arg_1_valid == 1 && arg_1[0] != '%' && arg_1[0] != '$' &&
              arg_1[0] != '0' && arg_1[0] != '1' && arg_1[0] != '2' &&
              arg_1[0] != '3' && arg_1[0] != '4' && arg_1[0] != '5' &&
              arg_1[0] != '6' && arg_1[0] != '7' && arg_1[0] != '8' &&
              arg_1[0] != '9') {
            for (x = 0; x <= label_index; x++) {
              if (strcmp(arg_1, label_names[x]) == 0) { // found it!
                arg_1_ispointer = 1;
                // itoa(x, arg_1,10);
                sprintf(arg_1, "%d", x);
                break;
              }
              if (x == label_index) {
                printf("FATAL ERROR. Reference was not found at line %i",
                       line_number);
                fclose(fp);
                fclose(of);
                return 0;
              }
            }
          }

          if (arg_2_valid == 1 && arg_2[0] != '%' && arg_2[0] != '$' &&
              arg_2[0] != '0' && arg_2[0] != '1' && arg_2[0] != '2' &&
              arg_2[0] != '3' && arg_2[0] != '4' && arg_2[0] != '5' &&
              arg_2[0] != '6' && arg_2[0] != '7' && arg_2[0] != '8' &&
              arg_2[0] != '9') {
            for (x = 0; x <= label_index; x++) {
              if (strcmp(arg_2, label_names[x]) == 0) { // found it!
                arg_2_ispointer = 1;
                // itoa(x, arg_2,10);
                sprintf(arg_2, "%d", x);
                break;
              }
              if (x == label_index) {
                printf("FATAL ERROR. Reference was not found at line %i",
                       line_number);
                fclose(fp);
                fclose(of);
                return 0;
              }
            }
          }
        }

        // look up instruction

        unsigned int op_type = 0;
        char op_code[32];
        op_code[0] = '\0';

        for (x = 0; x < numberofinstructions * 3; x++) {
          if (strcmp(current_instruction, sfx_instructions[x]) == 0) {
            if (verbosemode == 1) {

              printf("\n>%s", current_line);
              printf("Line %d,PC %d: '%s' type %s, hex '%s'\n", line_number,
                     program_counter, sfx_instructions[x],
                     sfx_instructions[x + 1], sfx_instructions[x + 2]);
            }

            op_type = atoi(sfx_instructions[x + 1]);
            strcpy(op_code, sfx_instructions[x + 2]);

            char code_temp[8];

            strcpy(code_temp,
                   sfx_instructions[x + 2]); // get raw opcode sequence

            int num_of_cycles = 0;
            int code_complete = 0;
            unsigned long hexcode_int = 0;
            int i = 0;
            old_program_counter = program_counter;
            for (i = 0; i < 4; i++) // advance program counter
            {
              if (code_temp[2 * i] == '\0' || code_temp[(2 * i) + 1] == '\0') {
                linedone = 1;
                break;
              }
              program_counter++;
              num_of_cycles++;
            }
            linedone = 1;
            break;
          }

          if (x == ((numberofinstructions * 3) - 1)) {
            printf("FATAL ERROR: Unrecognised Instruction %s at Line %d",
                   current_instruction, line_number);
            fclose(fp);
            fclose(of);
            return 0;
          }
        } // for numberofinstructions

        // begin combining opcodes with arguments
        // op_type, op_code, arg_1, arg_2

        // OPCODE TYPES REFERENCE
        // 0 - INSTRUCTION WITH NO ARGUMENTS
        // 1 - BRANCH - SINGLE INSTRUCTION WITH 8-BIT ARGUMENT (ADDRESS)
        // 2 - COMPACT - INSTRUCTION WITH 4-BIT ARGUMENT
        // 3 - COMPACT_DOUBLE - INSTRUCTION WITH 4-BIT ARGUMENT AND THEN 16-BIT
        // ARGUMENT 4 - DOUBLE-COMPACT - TWO COMPACT INSTRUCTIONS SEQUENTIALLY
        // 5 - COMPACT_SINGLE - INSTRUCTION WITH 4-BIT ARGUMENT AND THEN 8-BIT
        // ARGUMENT

        unsigned long outputcode = 0;
        unsigned long temp = 0;
        int output_needed = 0;
        long signed int branch_temp = 0;
        char temp_output[32];
        output_string[0] = '\n';

        if (pass == 2) {

          switch (op_type) {
          case 0: // Single instruction with no arguments
            strcpy(output_string, op_code);
            outputcode = strtol(op_code, NULL, 16);
            output_needed = 1;
            snprintf(output_string, 5, "%02X", outputcode);
            break;
          case 1: // BRANCH-wait for second pass - search for labels
            if (arg_1_ispointer == 1) {
              branch_temp = label_addresses[argconvert(arg_1)];
            } else {
              branch_temp = argconvert(arg_1);
            }

            branch_temp = branch_temp + 128 - old_program_counter;
            //	printf("btemp:%d",branch_temp);
            if (branch_temp > 255 || branch_temp < 0) {
              printf("FATAL ERROR. Branch instruction out of range at line %i",
                     line_number);
              fclose(fp);
              fclose(of);
              return 0;
            }
            outputcode = strtol(op_code, NULL, 16) + branch_temp;
            snprintf(output_string, 14, "0%X", outputcode);
            output_needed = 1;
            break;

          case 2:
            outputcode =
                strtol(op_code, NULL, 16); // Compact - Add to integer opcode.
            if (arg_1_ispointer == 1) {
              temp = label_addresses[argconvert(arg_1)];
            } else {
              temp = argconvert(arg_1);
            }

            if (temp > 15) {
              temp = 15;
            }
            outputcode = outputcode + temp;
            output_needed = 1;
            snprintf(output_string, 12, "%X", outputcode);
            break;
          case 3: // Compact-double
            outputcode = strtoul(op_code, NULL, 16);
            if (arg_1_ispointer == 1) {
              temp = label_addresses[argconvert(arg_1)];
            } else {
              temp = argconvert(arg_1);
            }

            if (temp > 15) {
              temp = 15;
            }
            temp = temp * 256 * 256; // add compact instruction to outputcode
            outputcode = outputcode + temp;
            if (arg_2_ispointer == 1) {
              temp = label_addresses[argconvert(arg_2)];
            } else {
              temp = argconvert(arg_2);
            }

            if (temp > 65535) // limit
            {
              temp = 65535;
            }
            outputcode = outputcode + temp;
            output_needed = 1;
            snprintf(output_string, 12, "%08X", outputcode);
            break;
          case 4: // double compact
            outputcode =
                strtol(op_code, NULL, 16); // Compact - Add to integer opcode.
            if (arg_1_ispointer == 1) {
              temp = label_addresses[argconvert(arg_1)];
            } else {
              temp = argconvert(arg_1);
            }

            if (temp > 15) {
              temp = 15;
            }
            temp = temp * 256;
            outputcode = outputcode + temp;
            if (arg_2_ispointer == 1) {
              temp = label_addresses[argconvert(arg_2)];
            } else {
              temp = argconvert(arg_2);
            }

            if (temp > 15) {
              temp = 15;
            }

            outputcode = outputcode + temp;
            output_needed = 1;
            snprintf(output_string, 12, "%04X", outputcode);
            break;
          case 5: // Compact-single
            outputcode = strtoul(op_code, NULL, 16);
            if (arg_1_ispointer == 1) {
              temp = label_addresses[argconvert(arg_1)];
            } else {
              temp = argconvert(arg_1);
            }

            if (temp > 15) {
              temp = 15;
            }
            temp = temp * 256; // add compact instruction to outputcode
            outputcode = outputcode + temp;
            if (arg_2_ispointer == 1) {
              temp = label_addresses[argconvert(arg_2)];
            } else {
              temp = argconvert(arg_2);
            }               // add 16-bit argument
            if (temp > 255) // limit
            {
              temp = 255;
            }
            outputcode = outputcode + temp;
            output_needed = 1;
            snprintf(output_string, 12, "%06X", outputcode);
            break;
          }
        } else { // first pass
          switch (op_type) {

          case 8: // LABEL statement
            strcpy(label_names[label_index], arg_1);
            label_addresses[label_index] = program_counter;
            label_index++;
            output_needed = 0;
            break;
          case 9: // DEF statement
            strcpy(label_names[label_index], arg_1);
            label_addresses[label_index] = argconvert(arg_2);
            label_index++;
            output_needed = 0;
            break;
          }
        }

        if (pass == 2 && output_needed == 1) {
          if (verbosemode == 1) {
            printf("HEX out: %s\n", output_string);
          }
          int output_size = 0;
          unsigned int ascii_char = 0;
          char returnstring[4];
          returnstring[0] = '\0';
          char cur_hex[2];
          cur_hex[2] = '\0';
          char cur_ascii[1];
          cur_ascii[1] = '\0';
          int i = 0;
          for (i = 0; i < 11; i++)

            output_size = strlen(output_string) / 2;
          cur_ascii[0] = '\0';
          for (i = (output_size - 1); i >= 0; i--) {

            cur_hex[0] = output_string[(2 * i)];
            cur_hex[1] = output_string[(2 * i) + 1];
            cur_hex[2] = '\0';

            cur_ascii[0] = strtol(cur_hex, NULL, 16);
            cur_ascii[1] = '\0';

            returnstring[i] = cur_ascii[0];
          }
          if (verbosemode == 1) {
            printf(">");
          }
          for (i = 0; i < output_size; i++) {
            fprintf(of, "%c", returnstring[i]);
            if (verbosemode == 1) {
              printf("%c", returnstring[i]);
            }
          }
          if (verbosemode == 1) {
            printf("<\n");
            printf("-----------------------------------------------\n");
          }
        }
      } // 2. else for instructions

    } // 1. while end of file =0
    if (verbosemode == 1 && pass == 1) {
      printf("\nList of references with program counter/value\n");
      for (i = 0; i < label_index; i++) {
        printf("%i:%s\n", label_addresses[i], label_names[i]);
      }
    }
    pass++;
    end_of_file = 0;
    fclose(fp);
    fclose(of);
  }
  if (verbosemode == 1)
    printf("\nDone.\n");

  return 0;
}

// Converts the many following expressions to an integer
// 1000 - decimal
//$100 - hexadecimal
//%100 - binary

int argconvert(char *argument_string) {
  int j = 0;
  int returnvalue = 0;
  char tempstring[32];
  unsigned int n;
  int dec = 0;
  int i = 0;
  int rem;

  switch (argument_string[0]) {
  case '$': // hexadecimal
    strcpy(tempstring, argument_string);
    tempstring[0] = '0';
    n = strtol(tempstring, NULL, 16);
    break;
  case '%': // binary
    strcpy(tempstring, argument_string);
    tempstring[0] = '0';
    n = atoi(tempstring);

    while (n != 0) {
      rem = n % 10;
      n /= 10;
      dec += rem * pow(2, i);
      ++i;
    }
    n = dec;
    break;
  default:
    n = atoi(argument_string);
  }

  return n;
}