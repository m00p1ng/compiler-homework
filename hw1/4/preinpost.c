#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

static FILE *f;
static int ch;
static unsigned int val;

enum { plus, minus, times, divide, mod, lparen, rparen, number, eof, illegal };

typedef struct NodeDesc *Node;
typedef struct NodeDesc {
    char kind;
    int val;
    Node left, right;
} NodeDesc;

static void SInit(char* filename) {
    ch = EOF;
    f = fopen( filename, "r+t" );
    if( f != NULL ) ch = getc(f);
}

static void Number() {
    val = 0;
    while('0' <= ch && ch <= '9') {
        val = val * 10 + ch - '0';
        ch = getc(f);
    }
}

static int SGet() {
    register int sym;
    while(ch != EOF && ch <= ' ') ch = getc(f);
    switch(ch) {
        case EOF : sym = eof; break;
        case '+' : sym = plus; ch = getc(f); break;
        case '-' : sym = minus; ch = getc(f); break;
        case '*' : sym = times; ch = getc(f); break;
        case '/' : sym = divide; ch = getc(f); break;
        case '%' : sym = mod; ch = getc(f); break;
        case '(' : sym = lparen; ch = getc(f); break;
        case ')' : sym = rparen; ch = getc(f); break;
        case '0' : case '1' : case '2' : case '3' : case '4' :
        case '5' : case '6' : case '7' : case '8' : case '9' :
                   sym = number; Number(); break;
        default : sym = illegal;
    }
    return sym;
}
static int sym;
static Node Expr();

static Node Factor() {
    Node result;
    assert(sym == number || sym == lparen);
    if(sym == number) {
        result = malloc(sizeof(NodeDesc));
        result->kind  = number;
        result->val   = val;
        result->left  = NULL;
        result->right = NULL;

        sym = SGet();
    } else {
        sym = SGet();
        result = Expr();
        assert(sym == rparen);
        sym = SGet();
    }
    return result;
}

static Node Term() {
    Node root, result;

    root = Factor();
    while(sym == times || sym == divide || sym == mod) {
        result = malloc(sizeof(NodeDesc));
        result->kind = sym;

        sym = SGet();
        result->left = root;
        result->right = Factor();
        root = result;
    }
    return root;
}

static Node Expr() {
    Node result, root;

    root = Term();
    while(sym == plus || sym == minus) {
        result = malloc(sizeof(NodeDesc));
        result->kind = sym;

        sym = SGet();
        result->left = root;
        result->right = Term();
        root = result;
    }
    return root;
}

static char op_to_char(char kind) {
    switch(kind) { 
        case plus:   return '+';
        case minus:  return '-';
        case times:  return '*';
        case divide: return '/';
        case mod:    return '%';
    }
    return '.';
}

static void prefix(Node root) {
    if(root) {
        switch(root->kind) {
            case plus: case minus: case times:
            case divide: case mod:
                putchar(op_to_char(root->kind)); break;
            case number: printf("%d" ,root->val); break;
        }
        putchar(' ');

        prefix(root->left);
        prefix(root->right);
    }
}

static void postfix(Node root) {
    if(root) {
        postfix(root->left);
        postfix(root->right);
        switch(root->kind) {
            case plus: case minus: case times:
            case divide: case mod:
                putchar(op_to_char(root->kind)); break;
            case number: printf("%d" ,root->val); break;
        }
        putchar(' ');
    }
}

static void infix(Node root, int depth) {
    if(root) {
        if(root->left != NULL && root->right != NULL) 
            printf("( ");
        infix(root->left, depth+1);
        switch(root->kind) {
            case plus: case minus: case times:
            case divide: case mod:
                putchar(op_to_char(root->kind)); break;
            case number: printf("%d" ,root->val); break;
        }
        putchar(' ');
        infix(root->right, depth+1);
        if(root->left != NULL && root->right != NULL) 
            printf(") ");
    }
}

int main(int argc, char* argv[]) {
    Node root;
    if(argc == 2) {
        SInit(argv[1]);
        sym = SGet();
        root = Expr();
        assert(sym == eof);
        puts("Prefix: ");
        prefix(root); puts("\n");

        puts("Infix: ");
        infix(root, 0); puts("\n");

        puts("Postfix: ");
        postfix(root); puts("\n");
    } else {
        printf("usage: expreval <filename>\n");
    }
    return 0;
}

