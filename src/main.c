/*
 * main.c
 */

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* Definitions. */
#define MAX_FILE_NAME_LENGTH    512
#define MAX_NUMBER_DIGITS       12

/* Group number strings. */
const char * group_number_strings[4] = {NULL, "THOUSAND", "MILLION", "BILLION"};

/* Single digits. */
const char * single_digits[10] = {  "", 
                                    "ONE",
                                    "TWO",
                                    "THREE",
                                    "FOUR",
                                    "FIVE",
                                    "SIX",
                                    "SEVEN",
                                    "EIGHT",
                                    "NINE"};

/* 10's digits. */
const char * tens_digits_one[10] = {"TEN",
                                    "ELEVEN",
                                    "TWELVE",
                                    "THIRTEEN",
                                    "FOURTEEN",
                                    "FIFTEEN",
                                    "SIXTEEN",
                                    "SEVENTEEN",
                                    "EIGHTEEN",
                                    "NINETEEN"};

/* 10's digits. */
const char * tens_digits[10] = {"",
                                "",
                                "TWENTY",
                                "THIRTY",
                                "FORTY",
                                "FIFTY",
                                "SIXTY",
                                "SEVENTY",
                                "EIGHTY",
                                "NINETY"};

/* Forward declarations. */
void process_number_word(const char * word, FILE * output_file);
void process_number_word_group(int long, int group_number, FILE * output_file);

/*
 * Main program entry point.
 */
int main(int argc, char ** argv)
{
    char ch;
    char input_file_name[MAX_FILE_NAME_LENGTH];
    char output_file_name[MAX_FILE_NAME_LENGTH];
    char number_word[MAX_NUMBER_DIGITS + 1];
    int number_word_len = 0;
    FILE * input_file;
    FILE * output_file;

    /* Start the program. */
    printf("number-words\n");

    /* Get the input file name. */
    printf("Enter the input file name: ");
    fgets(input_file_name, sizeof(input_file_name), stdin);

    /* Remove the \n from the input file name (fgets leaves it there). */
    input_file_name[strcspn(input_file_name, "\n")] = '\0';

    /* Make sure the user didn't just press enter. */
    if (strlen(input_file_name) == 0)
    {
        printf("Error. You didn't enter an input file name.\n");
        return 0;
    }

    /* Get the output file name. */
    printf("Enter the output file name: ");
    fgets(output_file_name, sizeof(output_file_name), stdin);

    /* Remove the \n from the output file name (fgets leaves it there). */
    output_file_name[strcspn(output_file_name, "\n")] = '\0';

    /* Make sure the user didn't just press enter. */
    if (strlen(output_file_name) == 0)
    {
        printf("Error. You didn't enter an output file name.\n");
        return 0;
    }

    /* Open the input file. */
    if ((input_file = fopen(input_file_name, "r")) == NULL)
    {
        printf("Failed to open input file name: %s\n", input_file_name);
        printf("The error is %i\n", errno);
        return 0;
    }

    /* Open the output file. */
    if ((output_file = fopen(output_file_name, "w")) == NULL)
    {
        fclose(input_file);
        printf("Failed to open output file name: %s\n", output_file_name);
        printf("The error is %i\n", errno);
        return 0;
    }

    /* Inform the user that we're processing the file. */
    printf("\nProcessing input file named: %s\n", input_file_name);

    /* Process the file. */
    while ((ch = fgetc(input_file)) != EOF)
    {
        /* If the character is a digit, add it to the word. */
        if (isdigit(ch))
        {
            /* If the number is too long, it's a fatal error. */
            if (number_word_len == MAX_NUMBER_DIGITS)
            {
                printf("Fatal Error: Encountered a number that was more than %i digits long.", MAX_NUMBER_DIGITS);
                number_word_len = 0;
                break;
            }

            /* Add the character to the word. */
            number_word[number_word_len++] = ch;
            continue;     
        }

        /* If the word length isn't 0, process the word. */
        if (number_word_len)
        {
            /* Process the word. Reset word length to 0. */
            number_word[number_word_len] = '\0';
            process_number_word(number_word, output_file);
            number_word_len = 0;
        }

        /* Write the character to the file. */
        fputc(ch, output_file);
    }

    /* Done with input file. Close it. */
    fclose(input_file);

    /* If we have a ending word, process it. */
    if (number_word_len)
    {
        number_word[number_word_len] = '\0';
        process_number_word(number_word, output_file);
    }

    /* Done with output file. Close it. */
    fclose(output_file);

    /* The program is done. */
    printf("\n");
}

/* Process a number word. */
void process_number_word(const char * number_word, FILE * output_file)
{
    long number = atol(number_word);
    if (number == 0)
    {
        fputs("ZERO", output_file);
    }
    else
    {
        process_number_word_group(number, 0, output_file);
    }
}

/* Process a number word group. */
void process_number_word_group(long number, int group_number, FILE * output_file)
{
    long this_group, next_group;
    int hundreds, tens, ones;
    bool output_a_space;

    /* Obtain this group and next group. This group is the first three
       digits (hundreds, tens, and ones). Next group is everyhing above
       this group and will be processed recursively. */
    this_group = number % 1000;
    next_group = number / 1000;

    /* If next groups exist, recursively process it. This will output the
       next group to the file before we output this group to the file so
       we process the number from left to right. */
    if (next_group)
    {
        process_number_word_group(next_group, group_number + 1, output_file);
    }

    /* Process this group. Get the hundreds, tens, and ones values. */
    hundreds = this_group / 100;
    this_group -= (hundreds * 100);
    tens = this_group / 10;
    this_group -= (tens * 10);
    ones = this_group;

    /* If there was a next group, we need to output a space. */
    output_a_space = next_group != 0;
    
    /* Process hundreds for this group. */
    if (hundreds)
    {
        fprintf(output_file, "%s%s HUNDRED", output_a_space ? " " : "", single_digits[hundreds]);
        output_a_space = true;
    }

    /* Process tens for this group. */
    if (tens == 1)
    {
        fprintf(output_file, "%s%s", output_a_space ? " " : "", tens_digits_one[ones]);
    }
    else
    {
        if (tens)
        {
            fprintf(output_file, "%s%s", output_a_space ? " " : "", tens_digits[tens]);
            output_a_space = true;
        }

        if (ones)
        {
            fprintf(output_file, "%s%s", output_a_space ? " " : "", single_digits[ones]);
        }
    }
    if (group_number && (hundreds || tens || ones))
    {
        fprintf(output_file, " %s", group_number_strings[group_number]);        
    }
}