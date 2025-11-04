#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef enum {
    T_IF, T_ELSE, T_WHILE, T_RETURN, T_INT, T_VOID,
    T_ID, T_NUM,
    T_PLUS, T_MINUS, T_TIMES, T_OVER,
    T_LT, T_LE, T_GT, T_GE, T_EQ, T_NE, T_ASSIGN,
    T_SEMI, T_COMMA,
    T_LPAREN, T_RPAREN, T_LBRACKET, T_RBRACKET, T_LBRACE, T_RBRACE,
    T_ENDFILE, T_ERROR
} TokenType;

const char *tokenNames[] = {
    "IF", "ELSE", "WHILE", "RETURN", "INT", "VOID",
    "ID", "NUM",
    "PLUS", "MINUS", "TIMES", "OVER",
    "LT", "LE", "GT", "GE", "EQ", "NE", "ASSIGN",
    "SEMI", "COMMA",
    "LPAREN", "RPAREN", "LBRACKET", "RBRACKET", "LBRACE", "RBRACE",
    "ENDFILE", "ERROR"
};

int isReserved(const char *str) {
    if (!strcmp(str, "if")) return T_IF;
    if (!strcmp(str, "else")) return T_ELSE;
    if (!strcmp(str, "while")) return T_WHILE;
    if (!strcmp(str, "return")) return T_RETURN;
    if (!strcmp(str, "int")) return T_INT;
    if (!strcmp(str, "void")) return T_VOID;
    return 0;
}

TokenType getToken(FILE *source, char *lexeme) {
    int c;
    int i = 0;

    while ((c = fgetc(source)) != EOF) {
        if (isspace(c)) continue;

        if (isalpha(c)) {
            lexeme[i++] = c;
            while (isalnum(c = fgetc(source))) lexeme[i++] = c;
            lexeme[i] = '\0';
            ungetc(c, source);
            int res = isReserved(lexeme);
            return res ? res : T_ID;
        }

        if (isdigit(c)) {
            lexeme[i++] = c;
            while (isdigit(c = fgetc(source))) lexeme[i++] = c;
            lexeme[i] = '\0';
            ungetc(c, source);
            return T_NUM;
        }

        switch (c) {
            case '+': strcpy(lexeme, "+"); return T_PLUS;
            case '-': strcpy(lexeme, "-"); return T_MINUS;
            case '*': strcpy(lexeme, "*"); return T_TIMES;
            case '/': {
                int next = fgetc(source);
                if (next == '*') {
                    int prev = 0;
                    while ((c = fgetc(source)) != EOF) {
                        if (prev == '*' && c == '/') break;
                        prev = c;
                    }
                    continue;
                } else {
                    ungetc(next, source);
                    strcpy(lexeme, "/");
                    return T_OVER;
                }
            }
            case '<': {
                int next = fgetc(source);
                if (next == '=') { strcpy(lexeme, "<="); return T_LE; }
                ungetc(next, source);
                strcpy(lexeme, "<"); return T_LT;
            }
            case '>': {
                int next = fgetc(source);
                if (next == '=') { strcpy(lexeme, ">="); return T_GE; }
                ungetc(next, source);
                strcpy(lexeme, ">"); return T_GT;
            }
            case '=': {
                int next = fgetc(source);
                if (next == '=') { strcpy(lexeme, "=="); return T_EQ; }
                ungetc(next, source);
                strcpy(lexeme, "="); return T_ASSIGN;
            }
            case '!': {
                int next = fgetc(source);
                if (next == '=') { strcpy(lexeme, "!="); return T_NE; }
                ungetc(next, source);
                strcpy(lexeme, "!"); return T_ERROR;
            }
            case ';': strcpy(lexeme, ";"); return T_SEMI;
            case ',': strcpy(lexeme, ","); return T_COMMA;
            case '(': strcpy(lexeme, "("); return T_LPAREN;
            case ')': strcpy(lexeme, ")"); return T_RPAREN;
            case '[': strcpy(lexeme, "["); return T_LBRACKET;
            case ']': strcpy(lexeme, "]"); return T_RBRACKET;
            case '{': strcpy(lexeme, "{"); return T_LBRACE;
            case '}': strcpy(lexeme, "}"); return T_RBRACE;
            default:
                lexeme[0] = c;
                lexeme[1] = '\0';
                return T_ERROR;
        }
    }
    strcpy(lexeme, "EOF");
    return T_ENDFILE;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo.c->\n", argv[0]);
        return 1;
    }

    FILE *source = fopen(argv[1], "r");
    if (!source) {
        printf("Erro ao abrir o arquivo: %s\n", argv[1]);
        return 1;
    }

    char lexeme[100];
    TokenType token;
    while ((token = getToken(source, lexeme)) != T_ENDFILE) {
        printf("Token: %-10s | Lexema: %s\n", tokenNames[token], lexeme);
    }

    fclose(source);
    return 0;
}
