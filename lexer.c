#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LENGTH 100
#define MAX_TOKENS 1000
#define MAX_STACK_SIZE 100


// Token types
typedef enum {
    IDENTIFIER, KEYWORD, RESERVED_WORD, NOISE_WORD, INTEGER_LITERAL, FLOAT_LITERAL, STRING_LITERAL,
    ARITHMETIC_OP, BOOLEAN_OP, ASSIGNMENT_OP,
    PUNCTUATOR, COMMENT, INVALID, CHAR_LITERAL, LEFT_BRACKET, RIGHT_BRACKET
} TokenType; 

// Token type names
const char *tokenTypeNames[] = {
    "IDENTIFIER", "KEYWORD", "RESERVED_WORD", "NOISE_WORD", "INTEGER_LITERAL", "FLOAT_LITERAL", "STRING_LITERAL",
    "ARITHMETIC_OP", "BOOLEAN_OP", "ASSIGNMENT_OP",
    "PUNCTUATOR", "COMMENT", "INVALID", "CHAR_LITERAL", "LEFT_BRACKET", "RIGHT_BRACKET"
};

// Operators categorized
const char *arithmeticOperators[] = { "+", "-", "*", "/", "%", "++", "--", NULL };
const char *booleanOperators[] = { "&&", "||", "!", "==", "!=", "<", ">", "<=", ">=", NULL };
const char *assignmentOpperators[] = { "=",  "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", ">>=", "<<=", NULL };
const char *keywords[] = {"char", "const", "cont", "def", "do", "double", "else", "enum", "float", "for", 
                        "if", "int", "return", "void", "while", "constant", "continue", "default", "NULL", NULL }; //"constant", "continue", "default"
const char *reservedWords[] = { "ms", "sec", "after", "every", "timeout", "freeze", NULL };
const char *noiseWords[] = { "ant", "inue", "ault", NULL };

// Token structure
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
    int lineNumber;
} Token;

// Stack structure for delimiter matching
typedef struct {
    char items[MAX_STACK_SIZE];
    int top;
} Stack;

// Stack operations
void push(Stack *stack, char c) {
    if (stack->top < MAX_STACK_SIZE - 1) {
        stack->items[++(stack->top)] = c;
    } else {
        fprintf(stderr, "Error: Stack overflow while pushing '%c'\n", c);
    }
}

char pop(Stack *stack) {
    if (stack->top >= 0) {
        return stack->items[(stack->top)--];
    } else {
        fprintf(stderr, "Error: Stack underflow while popping\n");
        return '\0';
    }
}

char peek(const Stack *stack) {
    if (stack->top >= 0) {
        return stack->items[stack->top];
    } else {
        return '\0';
    }
}

int isMatchingPair(char open, char close) {
    return (open == '(' && close == ')') ||
           (open == '{' && close == '}') ||
           (open == '[' && close == ']');
}

// Utility function to check if a string is an arithmetic operator
int isArithmeticOperator(const char *word) {
    for (int i = 0; arithmeticOperators[i] != NULL; i++) {
        if (strcmp(word, arithmeticOperators[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Utility function to check if a string is a boolean operator
int isBooleanOperator(const char *word) {
    for (int i = 0; booleanOperators[i] != NULL; i++) {
        if (strcmp(word, booleanOperators[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Utility function to check if a string is an assignment operator
int isAssignmentOperator(const char *word) {
    for (int i = 0; assignmentOpperators[i] != NULL; i++) {
        if (strcmp(word, assignmentOpperators[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to check if a word is a keyword
int isKeyword(const char *word) {
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int isReservedWord(const char *word) {
    for (int i = 0; reservedWords[i] != NULL; i++) {
        if (strcmp(word, reservedWords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to check if a string is an invalid identifier
int isInvalidIdentifier(const char *word) {
    // Check if the first character is a digit
    if (isdigit(word[0])) {
        return 1; // Invalid if it starts with a digit
    }
    // Check if the word is entirely numeric
    int i = 0;
    while (word[i] != '\0') {
        if (!isalnum(word[i]) && word[i] != '_') {
            return 1; // Invalid if it contains invalid characters
        }
        i++;
    }
    return 0; // Valid identifier
}


// Lexer function
void tokenize(const char *input) {
    int i = 0;
    int length = strlen(input);
    Token tokens[MAX_TOKENS];
    int tokenCount = 0;
    int lineNumber = 1; // Track line numbers
    Stack delimiterStack = { .top = -1 };

    while (i < length) {
        if (isspace(input[i])) {
            if (input[i] == '\n') {
                lineNumber++; // Increment line number on newline
            }
            i++;
            continue;
        }

        Token token;
        token.lineNumber = lineNumber; // Assign current line number

        // Handle identifiers, keywords, and reserved words
        if (isalpha(input[i]) || input[i] == '_') {
            int start = i;
            while (isalnum(input[i]) || input[i] == '_') {
                i++;
            }
            strncpy(token.value, &input[start], i - start);
            token.value[i - start] = '\0';

            if (isInvalidIdentifier(token.value)) {
                fprintf(stderr, "Error: Invalid identifier '%s' at line %d\n", token.value, lineNumber);
                token.type = INVALID;
            } else if (isKeyword(token.value)) {
                token.type = KEYWORD;

                // Check for noise words within the keyword
                for (int j = 0; noiseWords[j] != NULL; j++) {
                    if (strstr(token.value, noiseWords[j]) != NULL) {
                        Token noiseToken;
                        noiseToken.type = NOISE_WORD; // Mark as a specific type if needed
                        strcpy(noiseToken.value, noiseWords[j]);
                        noiseToken.lineNumber = lineNumber;
                        if (tokenCount < MAX_TOKENS) {
                        }
                    }
                }
            } else if (isReservedWord(token.value)) {
                token.type = RESERVED_WORD;
            } else {
                token.type = IDENTIFIER;
            }
            } else if (isdigit(input[i])) { // Handle numbers
                int start = i;
                while (isdigit(input[i])) { // Scan numeric characters
                    i++;
                }
                int isFloat = 0;

                if (input[i] == '.' && isdigit(input[i + 1])) { // Detect floats
                    isFloat = 1;
                    i++;
                    while (isdigit(input[i])) {
                        i++;
                    }
                }

                // Copy the entire token value including invalid characters
                int end = i;
                while (!isspace(input[i]) && input[i] != '\0' && 
                    input[i] != ';' && input[i] != ',' && input[i] != ')' && input[i] != '}' && input[i] != ']' && 
                    input[i] != '\'' && input[i] != '"' && !isdigit(input[i])) {
                    i++; // Consume invalid characters
                }

                strncpy(token.value, &input[start], i - start);
                token.value[i - start] = '\0';

                // Validate the next character
                if (i > end) { // If invalid characters were found
                    fprintf(stderr, "Error: Invalid numeric literal '%s' at line %d\n", token.value, lineNumber);
                    token.type = INVALID;
                } else {
                    token.type = isFloat ? FLOAT_LITERAL : INTEGER_LITERAL;
                }
            } else if (input[i] == '"') { // String literals
            int start = i++;
            while (input[i] != '"' && input[i] != '\0') {
                if (input[i] == '\n') lineNumber++; // Count newlines inside strings
                i++;
            } 
            if (input[i] == '"') {
                i++;
                strncpy(token.value, &input[start], i - start);
                token.value[i - start] = '\0';
                token.type = STRING_LITERAL;
            } else {
                fprintf(stderr, "Error: Unterminated string literal at line %d\n", lineNumber);
                token.type = INVALID;
                strncpy(token.value, &input[start], i - start);
                token.value[i - start] = '\0';
            }

        } else if (input[i] == '\'' && input[i + 1] == '\\' && input[i + 2] == '0' && input[i + 3] == '\'') {
            // Handle '\0' as a valid character literal
            strncpy(token.value, &input[i], 4); // Copy the full literal, including quotes
            token.value[4] = '\0'; // Null-terminate the string
            token.type = CHAR_LITERAL;
            i += 4; // Skip past the literal

        } else if (input[i] == '\'' && isprint(input[i + 1]) && input[i + 2] == '\'') { // Handle other character literals
            int start = i;
            strncpy(token.value, &input[start], 3); // Copy the full character literal, including quotes
            token.value[3] = '\0'; // Null-terminate the string
            token.type = CHAR_LITERAL;
            i += 3; // Skip past the character literal

        } else if (input[i] == '/' && input[i + 1] == '/') { // Handle single-line comments
            int start = i;
            while (input[i] != '\n' && input[i] != '\0') {
                i++;
            }
            strncpy(token.value, &input[start], i - start);
            token.value[i - start] = '\0';
            token.type = COMMENT;

        } else if (input[i] == '/' && input[i + 1] == '*') { // Handle multi-line comments
            int start = i;
            i += 2;
            while (!(input[i] == '*' && input[i + 1] == '/') && input[i] != '\0') {
                if (input[i] == '\n') lineNumber++;
                i++;
            }
            if (input[i] == '*' && input[i + 1] == '/') {
                i += 2;
                strncpy(token.value, &input[start], i - start);
                token.value[i - start] = '\0';
                token.type = COMMENT;
            } else {
                fprintf(stderr, "Error: Unterminated comment starting at line %d\n", lineNumber);
                token.type = INVALID;
                strncpy(token.value, &input[start], i - start);
                token.value[i - start] = '\0';
            }

        } else { // Handle operators, punctuators, and invalid tokens
            int start = i;

            // Check for delimiters
            if (input[i] == '(' || input[i] == '{' || input[i] == '[') {
                token.value[0] = input[i];
                token.value[1] = '\0';
                token.type = LEFT_BRACKET;
                push(&delimiterStack, input[i]);
                i++;
            } else if (input[i] == ')' || input[i] == '}' || input[i] == ']') {
                token.value[0] = input[i];
                token.value[1] = '\0';
                token.type = RIGHT_BRACKET;
                if (delimiterStack.top < 0 || !isMatchingPair(pop(&delimiterStack), input[i])) {
                    fprintf(stderr, "Error: Mismatched delimiter '%c' at line %d\n", input[i], lineNumber);
                }
                i++;
            } else if ((input[i] == '+' || input[i] == '-') && input[i + 1] == input[i]) {
                // Handle "++" and "--"
                token.value[0] = input[i];
                token.value[1] = input[i + 1];
                token.value[2] = '\0';
                token.type = ARITHMETIC_OP;
                i += 2;
            } else if (input[i] == ';' || input[i] == ',') {
                // Handle single-character punctuators
                token.value[0] = input[i];
                token.value[1] = '\0';
                token.type = PUNCTUATOR;
                i++;
            } else {
                while (!isspace(input[i]) && !isalnum(input[i]) && input[i] != '\0') {
                    i++;
                }
                strncpy(token.value, &input[start], i - start);
                token.value[i - start] = '\0';

                if (isArithmeticOperator(token.value)) {
                    token.type = ARITHMETIC_OP;
                } else if (isBooleanOperator(token.value)) {
                    token.type = BOOLEAN_OP;
                } else if (isAssignmentOperator(token.value)) {
                    token.type = ASSIGNMENT_OP;
                } else {
                    token.type = INVALID;
                    fprintf(stderr, "Error: Invalid token '%s' at line %d\n", token.value, lineNumber);
                }
            }
        }

        if (tokenCount < MAX_TOKENS) {
            tokens[tokenCount++] = token;
        } else {
            fprintf(stderr, "Error: Token limit exceeded.\n");
            break;
        }
    }

    // Check for unmatched delimiters
    while (delimiterStack.top >= 0) {
        fprintf(stderr, "Error: Unmatched delimiter '%c'\n", pop(&delimiterStack));
    }

    // Print tokens
    printf("Token Table:\n");
    printf("| %-15s | %-20s | %-15s |\n", "Type", "Value", "Line Number");
    printf("|-----------------|----------------------|-----------------|\n");
    for (int j = 0; j < tokenCount; j++) {
        printf("| %-15s | %-20s | %-15d |\n",
               tokenTypeNames[tokens[j].type], tokens[j].value, tokens[j].lineNumber);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename.cig>\n", argv[0]);
        return 1;
    }

    // Check if the file has the ".cig" extension
    const char *filename = argv[1];
    const char *extension = strrchr(filename, '.'); // Get the file extension
    if (!extension || strcmp(extension, ".cig") != 0) {
        fprintf(stderr, "Error: Only '.cig' files are allowed.\n");
        return 1;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *code = (char *)malloc(fileSize + 1);
    if (!code) {
        perror("Memory allocation failed");
        fclose(file);
        return 1;
    }

    fread(code, 1, fileSize, file);
    code[fileSize] = '\0';
    fclose(file);

    tokenize(code);
    free(code); 
    return 0; 
}
