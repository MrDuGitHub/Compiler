#include <iostream>
#include <fstream>
#include <vector>

#define MAXN 1000

using namespace std;

void BLOCK(int tx,int l);
void PROGRAM();
void SUBPROGRAM();
void CONST_DESCRIPTION();
void CONST_DEFINEDS();
void CONST_DEFINED_ANY();
void CONST_DEFINED();
void UNSIGNED_INT();
void VAR_DESCRIPTION();
void VAR_DEFINEDS();
void IDENTIFIER_ANY();
void IDENTIFIER();
void PROCEDURE_DESCRITION();
void PROCEDURES();
void PROCEDURE_ANY();
void PROCEDURE();
void PROCEDURE_HEAD();
void STATEMENT();
void ASSIGN();
void COMPLEX();
void STATEMENTS();
void STATEMENT_ANY();
void CONDITION();
void EXPRESSION();
void EXPRESSIONS();
void EXPRESSION_ANY();
void ITEM();
void ITEMS();
void ITEM_ANY();
void FACTOR_ANY();
void FACTOR();
void CONDITIONAL();
void CALL();
void WHILE();
void READ();
void WRITE();

typedef struct DESCRITION
{
    string name;
    string kind;
    union
    {
        int val;
        int level;
    };
    int adr;
} Descrition;

const string KEYWORD[] = {"IDENT","NUMBER","CONST","VAR","PROCEDURE","BEGIN","END","ODD","IF","THEN",
                          "CALL","WHILE","DO","READ","WRITE"};
const string symble[] = {"+","-","*","/","#","<","<=",">",">=","=",":=",",",".",";","(",")"};
fstream word("word.txt");
vector<pair<int,int> > SYM;
vector<string> ID;
vector<string> NUM;
int index=0;
string SYM_WORD;
Descrition table[MAXN];
int TX[50];
int TX_index;
int level;
int table_index=0;

string output[1000];
int output_index=0;

void ADVANCE();
void ERROR(){cout<<"ERROR"<<endl;}

void init();
bool is_relational(string s);
bool is_identifier(string s){return SYM[index-1].first==0;}
bool is_unsignnum(string s){return SYM[index-1].first==1;}

int main()
{
    init();
    BLOCK(0,0);
    cout << "Hello World!" << endl;
    return 0;
}

void init()
{
    int m,n,x;
    word>>m;word>>n;word>>x;
    int first,second;
    for (int i=0;i<m;i++)
    {
        word>>first;
        word>>second;
        SYM.push_back(make_pair(first,second));
    }
    string tem;
    for (int i=0;i<n;i++)
    {
        word>>tem;
        ID.push_back(tem);
    }
    for (int i=0;i<x;i++)
    {
        word>>tem;
        NUM.push_back(tem);
    }
    ADVANCE();
    for (int i=0;i<1000;i++) output[i]="";
    //cout<<SYM_WORD<<endl;
    //cout<<SYM.size()<<ID.size()<<NUM.size()<<endl;
}

void ADVANCE()
{
    if (SYM[index].first==0)
        SYM_WORD=ID[SYM[index].second];
    else if (SYM[index].first==1)
        SYM_WORD=NUM[SYM[index].second];
    else if (SYM[index].first<15)
        SYM_WORD=KEYWORD[SYM[index].first];
    else
        SYM_WORD=symble[SYM[index].first-15];
    for (int i=0;i<SYM_WORD.length();i++)
        if (SYM_WORD[i]<=90&&SYM_WORD[i]>=65)
            SYM_WORD[i]+=32;
    index++;
    cout<<SYM_WORD<<endl;
}

void BLOCK(int tx,int l)
{
//   PROGRAM();
    TX_index=tx;
    level=l;
    output[output_index]+="Subprogram";
    output_index++;
    SUBPROGRAM();
    output_index--;
}

void PROGRAM()
{
}

void SUBPROGRAM()
{
    CONST_DESCRIPTION();
    VAR_DESCRIPTION();
    PROCEDURE_DESCRITION();
    STATEMENT();
}

void CONST_DESCRIPTION()
{
    if (SYM_WORD=="const")
    {
        ADVANCE();
        CONST_DEFINEDS();
    }
}

void CONST_DEFINEDS()
{
    CONST_DEFINED();
    CONST_DEFINED_ANY();
}

void CONST_DEFINED_ANY()
{
    if (SYM_WORD==",")
    {
        ADVANCE();
        CONST_DEFINED();
        CONST_DEFINED_ANY();
    }
    else if (SYM_WORD==";")
        ADVANCE();
    else ERROR();
}

void CONST_DEFINED()
{
    IDENTIFIER();
    if (SYM_WORD=="=")
    {
        ADVANCE();
        if (is_unsignnum(SYM_WORD))
            ADVANCE();
        else
            ERROR();
    }
    else
        ERROR;
}

void VAR_DESCRIPTION()
{
    if (SYM_WORD=="var")
    {
        ADVANCE();
        VAR_DEFINEDS();
    }
}

void VAR_DEFINEDS()
{
    IDENTIFIER();
    IDENTIFIER_ANY();
    if (SYM_WORD==";")
        ADVANCE();
    else ERROR();
}

void IDENTIFIER_ANY()
{
    if (SYM_WORD==",")
    {
        ADVANCE();
        IDENTIFIER();
        IDENTIFIER_ANY();
    }
}

void IDENTIFIER()
{
    if (is_identifier(SYM_WORD))
        ADVANCE();
    else ERROR();
}

void PROCEDURE_DESCRITION()
{
    if (SYM_WORD=="procedure")
        PROCEDURES();
}

void PROCEDURES()
{
    PROCEDURE();
    PROCEDURE_ANY();
}

void PROCEDURE_ANY()
{
    if (SYM_WORD=="procedure")
    {
        PROCEDURE();
        PROCEDURE_ANY();
    }
}

void PROCEDURE()
{
    PROCEDURE_HEAD();
   // cout<<index<<SYM_WORD<<"!"<<endl;
    SUBPROGRAM();
    if (SYM_WORD==";")
        ADVANCE();
    else
        ERROR();
}

void PROCEDURE_HEAD()
{
    if (SYM_WORD=="procedure")
    {
        ADVANCE();
        IDENTIFIER();
        if (SYM_WORD==";")
            ADVANCE();
        else
            ERROR();
    }
    else
        ERROR();
}

void STATEMENT()
{
    if (is_identifier(SYM_WORD))
        ASSIGN();
    else if (SYM_WORD=="if")
        CONDITIONAL();
    else if (SYM_WORD=="while")
        WHILE();
    else if (SYM_WORD=="call")
        CALL();
    else if (SYM_WORD=="write")
        WRITE();
    else if (SYM_WORD=="read")
        READ();
    else if (SYM_WORD=="begin")
        COMPLEX();
}

void ASSIGN()
{
    IDENTIFIER();
    if (SYM_WORD==":=")
    {
        ADVANCE();
        EXPRESSION();
    }
    else ERROR();
}

void COMPLEX()
{
    if (SYM_WORD=="begin")
    {
        ADVANCE();
        STATEMENTS();
        if (SYM_WORD=="end")
            ADVANCE();
        else
            ERROR;
    }
    else ERROR();
}

void STATEMENTS()
{
    STATEMENT();
    STATEMENT_ANY();
}

void STATEMENT_ANY()
{
    if (SYM_WORD==";")
    {
        ADVANCE();
        STATEMENT();
        STATEMENT_ANY();
    }
}

void CONDITION()
{
    if (SYM_WORD=="odd")
    {
        ADVANCE();
        EXPRESSION();
    }
    else
    {
        EXPRESSION();
        if (is_relational(SYM_WORD))
        {
            ADVANCE();
            EXPRESSION();
        }
        else ERROR();
    }
}

void EXPRESSION()
{
    if (SYM_WORD=="+"||SYM_WORD=="-") ADVANCE();
    ITEMS();
}

void EXPRESSIONS()
{
    EXPRESSION();
    EXPRESSION_ANY();
}

void EXPRESSION_ANY()
{
    if (SYM_WORD==",")
    {
        ADVANCE();
        EXPRESSION();
        EXPRESSION_ANY();
    }
}

void ITEMS()
{
    ITEM();
    ITEM_ANY();
}

void ITEM_ANY()
{
    if (SYM_WORD=="+"||SYM_WORD=="-")
    {
        ADVANCE();
        ITEM();
        ITEM_ANY();
    }
}

void ITEM()
{
    FACTOR();
    FACTOR_ANY();
}

void FACTOR_ANY()
{
    if (SYM_WORD=="*"||SYM_WORD=="/")
    {
        ADVANCE();
        FACTOR();
        FACTOR_ANY();
    }
}

void FACTOR()
{
    if (is_identifier(SYM_WORD)||is_unsignnum(SYM_WORD))
        ADVANCE();
    else
        if (SYM_WORD=="(")
        {
            ADVANCE();
            EXPRESSION();
            if (SYM_WORD==")")
                ADVANCE();
            else
                ERROR();
        }
        else ERROR();
}

void CONDITIONAL()
{
    if (SYM_WORD=="if")
    {
        ADVANCE();
        CONDITION();
        if (SYM_WORD=="then")
        {
            ADVANCE();
            STATEMENT();
        }
        else ERROR();
    }
    else ERROR();
}

void CALL()
{
    if (SYM_WORD=="call")
    {
        ADVANCE();
        if (is_identifier(SYM_WORD))
            ADVANCE();
        else
            ERROR();
    }
    else
        ERROR();
}

void WHILE()
{
    if (SYM_WORD=="while")
    {
        ADVANCE();
        CONDITION();
        if (SYM_WORD=="do")
        {
            ADVANCE();
            STATEMENT();
        }
        else ERROR();
    }
    else
        ERROR();
}

void READ()
{
    if (SYM_WORD=="read")
    {
        ADVANCE();
        if (SYM_WORD=="(")
        {
            ADVANCE();
            IDENTIFIER();
            IDENTIFIER_ANY();
            if (SYM_WORD==")")
            {
                ADVANCE();
            }
            else  ERROR();
        }
    }
    else ERROR();
}

void WRITE()
{
    if (SYM_WORD=="write")
    {
        ADVANCE();
        if (SYM_WORD=="(")
        {
            ADVANCE();
            EXPRESSION();
            EXPRESSION_ANY( );
            if (SYM_WORD==")")
            {
                ADVANCE();
            }
            else  ERROR();
        }
    }
    else ERROR();
}

bool is_relational(string s)
{
    return (s=="="||s=="#"||s=="<"||s=="<="||s==">"||s==">=");
}
