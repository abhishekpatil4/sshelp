#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define ll long long int

typedef struct SYMTAB
{
    char label[30];
    ll addr;
} symtab;

ll locctr = 0;
int search_optab(char symbol[])
{
    FILE *fp = fopen("optab.txt", "r");
    while (!feof(fp))
    {
        char buffer[100];
        fgets(buffer, 100, fp);
        char *tokens = strtok(buffer, " \n");
        if (strcmp(tokens, symbol) == 0)
            return 1;
    }
    return 0;
}

long long int hex_to_int(char hstr[])
{
    // converts hexadecimal to decimal integer
    ll ans = 0;
    // there is a catch in this oftern the hstr has endline i.e '\n' at end so check that as well
    for (int i = 0; hstr[i] != '\0' && hstr[i] != '\n'; i++)
    {
        ans = (ans * 16) + (hstr[i] - '0');
    }
    return ans;
}

int main()
{
    FILE *input = fopen("input.txt", "r");
    FILE *sym = fopen("symtab.txt", "w");
    FILE *output = fopen("output_pass1.txt", "w");
    int lines = 1; // to print error message in a particular line

    symtab smtb[100];
    int smtb_len = 0;

    while (!feof(input))
    {
        char buffer[1000];
        fgets(buffer, 1000, input);
        char *tokens = strtok(buffer, " \n"); // tokenising string read using space as delimeter

        int word_count = 0;

        char label[20], mnemonic[20], operand[20];

        while (tokens)
        {
            if (word_count == 0)
                strcpy(label, tokens);
            if (word_count == 1)
                strcpy(mnemonic, tokens);
            if (word_count == 2)
                strcpy(operand, tokens);
            tokens = strtok(NULL, " ");
            word_count++;
        }
        if (word_count == 2)
        {
            if (strcmp("START", label) == 0)
            {
                fprintf(output, "%llX %s %s", locctr, label, mnemonic);
                locctr = hex_to_int(mnemonic);
            }
            // only opcode and operand no label
            else if (search_optab(label)) // I am using label to search in optab as when 2 words only the first one will be stored in label variable
            {
                fprintf(output, "%llX %s %s", locctr, label, mnemonic);

                locctr += 3;
            }
            else
            {
                printf("ERROR at line %d , NO OPCODE FOUND\n", lines);
            }
        }
        else if (word_count == 3)
        {
            if (strcmp("START", mnemonic) == 0)
                locctr = hex_to_int(operand);
            // that means 3 words , label , mnemonic , operand
            int flag_dup_sym = 0;
            // search in symbol table
            for (int i = 0; i < smtb_len; i++)
            {
                if (strcmp(smtb[i].label, label) == 0)
                {
                    printf("ERROR at line %d , DUPLICATE symbol\n", lines);
                    flag_dup_sym = 1;
                    break;
                }
            }
            if (!flag_dup_sym)
            {
                // inserting into symbol table data structure (array)
                smtb[smtb_len].addr = locctr;
                strcpy(smtb[smtb_len].label, label);
                smtb_len++;
                // writing into output file as well as symbol table file
                if (strcmp("START", mnemonic) == 0)
                {
                    fprintf(output, "%llx %s %s %s", 0LL, label, mnemonic, operand);
                    fprintf(sym, "%s %llX\n", label, 0LL);
                }
                else
                {
                    fprintf(output, "%llx %s %s %s", locctr, label, mnemonic, operand);
                    fprintf(sym, "%s %llX\n", label, locctr);
                }

                // calculation for locctr increment
                if (strcmp(mnemonic, "BYTE") == 0)
                    locctr += 1;
                else if (strcmp(mnemonic, "WORD") == 0)
                    locctr += 3;
                else if (strcmp(mnemonic, "RESW") == 0)
                    locctr = locctr + (3 * hex_to_int(operand));
                else if (strcmp(mnemonic, "RESB") == 0)
                {
                    if (operand[0] == 'c' || operand[0] == 'C')
                        locctr += (strlen(operand) - 3);
                    else
                        locctr += hex_to_int(operand);
                }
            }
        }
        else
        {
            if (search_optab(label) == 1)
            {
                fprintf(sym, "%s %llX\n", label, locctr);
                fprintf(output, "%llX %s\n", locctr, label);
            }
            else
                printf("ERROR at line %d , NO OPCODE FOUND\n", lines);
            locctr += 3;
        }
        lines++;
    }
    fclose(input);
    fclose(sym);
    fclose(output);
}
