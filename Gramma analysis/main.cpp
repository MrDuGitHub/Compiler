#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <utility>

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

typedef struct CODE
{
    string f;
    int l;
    int a;
} Code;
vector<Code> code;

/************************************ OPR ************************************/
string opra[]={"ret","+","-","*","/","#","<","<=",">",">=","=","%","read","write"};
                /*0   1   2   3   4   5   6    7   8    9   10  11   12      13*/
/************************************ OPR ***********************************/

/************* Input and output ***************/
fstream word("word1.txt");
fstream cf("code.txt");
fstream Tree("Tree.json");
string output[1000];
int output_index=1;
string tree="";
/*********************************************/

vector<pair<int,int> > SYM;
vector<string> ID;
vector<string> NUM;
stack<string> opr;  //operator stack

int index=0;
string SYM_WORD;
Descrition table[MAXN];
int table_index=0;
int TX[50];
int TX_index=0;
int level,dx;

bool vard=false; //var_defind
bool is_read=false;
bool is_write=false;
stack<int> pro_index; //procedure index
int first=0; //Entry address

bool Error=false;

//output for console
void write_output(string s)
{
    output_index--;
    output[output_index]+=s;
    output_index++;
}

void ERROR()
{
    Error=true;
    cout<<"ERROR"<<endl;
    output[output_index]+="ERROR ";
}

void ADVANCE();
void init();
bool is_relational(string s);
bool is_identifier(string s){return SYM[index-1].first==0;}
bool is_unsignnum (string s){return SYM[index-1].first==1;}
void Output();

int main()
{
    init();
    PROGRAM();
    Output();
    return 0;
}

//Read words
void init()
{
    int m,n,x;
    word>>m;
    word>>n;
    word>>x;
    int first,second;
    for (int i=0; i<m; i++)
    {
        word>>first;
        word>>second;
        SYM.push_back(make_pair(first,second));
    }
    string tem;
    for (int i=0; i<n; i++)
    {
        word>>tem;
        ID.push_back(tem);
    }
    for (int i=0; i<x; i++)
    {
        word>>tem;
        NUM.push_back(tem);
    }
    ADVANCE();
    for (int i=0; i<1000; i++) output[i]="";
}

void Output()
{
    //tree in concole
    /*for (int i=0; i<25; i++)
        cout<<output[i]<<endl;*/
    //table
    for (int i=0; i<table_index; i++)
    {
        cout<<i<<"\t"<<table[i].name<<"\t"<<table[i].kind<<"\t";
        if (table[i].kind=="CONSTANT")
            cout<<table[i].val<<endl;
        else cout<<table[i].level<<"\t"<<table[i].adr<<endl;
    }
    //code
    for (int i=0; i<code.size(); i++)
    {
        if (code[i].f=="OPR")  cout<<i<<"\t"<<code[i].f<<"\t"<<code[i].l<<"\t"<<code[i].a<<"\t"<<opra[code[i].a]<<endl;
        else
            cout<<i<<"\t"<<code[i].f<<"\t"<<code[i].l<<"\t"<<code[i].a<<endl;
        cf<<code[i].f<<"\t\t"<<code[i].l<<"\t"<<code[i].a<<endl;
    }
    cout<<"main "<<first<<endl;
    //json
    Tree<<tree<<endl;
}

int str2num(string s)
{
    int num=0;
    for (int i=0; i<s.length(); i++)
        num=num*10+s[i]-'0';
    return num;
}

void fill_table(string name,string kind,string value)
{
    if (kind=="CONSTANT")
    {
        table[table_index].kind=kind;
        table[table_index].name=name;
        table[table_index].val=str2num(value);
        table[table_index].adr=0;
    }
    if (kind=="VARIABLE")
    {
        table[table_index].kind=kind;
        table[table_index].name=name;
        table[table_index].level=level;
        table[table_index].adr=dx;
        dx++;
    }
    if (kind=="PROCEDURE")
    {
        table[table_index].kind=kind;
        table[table_index].name=name;
        table[table_index].level=level;
    }
    table_index++;
}

//look up table and return Level difference and offset
void lookup(string name,int* tl,int* tdx)
{
    bool findit=false;*tl=0;*tdx=3;
    for (int i=TX_index;i>=0;i--)
    {
        for (int j=TX[i];j<=table_index;j++)
        {
            if (table[j].kind=="PROCEDURE") break;
            if (table[j].name==name)
            {
                if (findit)ERROR();
                findit=true;
                if (table[j].kind=="CONSTANT")
                {
                    *tl=table[j].val;
                    *tdx=0;
                }
                if (table[j].kind=="VARIABLE")
                    *tdx=table[j].adr;
            }
         }
         if (findit)break;
         (*tl)++;
    }
    if (!findit) ERROR();
}

void emit(string s,int l,int a)
{
    cout<<s<<" "<<l<<" "<<a<<endl;
    Code t;
    t.f=s;t.l=l;t.a=a;
    code.push_back(t);
}

void putonstack(string s)
{
    if (is_identifier(s))
    {
        int tl=0;int tdx=3;
        lookup(s,&tl,&tdx);
        if(tdx==0)emit("LIT",0,tl);
        else emit("LOD",tl,tdx);
    }else
    if(is_unsignnum(s))
        emit("LIT",0,str2num(s));
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
    for (int i=0; i<SYM_WORD.length(); i++)
        if (SYM_WORD[i]<=90&&SYM_WORD[i]>=65)
            SYM_WORD[i]+=32;
    index++;
    cout<<index<<" "<<SYM_WORD<<endl;
}

void BLOCK(int tx,int l)
{
    TX[TX_index]=tx;
    level=l;dx=3;
    output[output_index]+="Subprogram\t";
    tree+="\"Subprogram\":{";
    output_index++;
    SUBPROGRAM();
    tree+="}";
    output_index--;
}

void PROGRAM()
{
    emit("CAL",0,0);
    tree+="{\"Program\":{";
    BLOCK(0,0);
    if (SYM_WORD==".")
        {output[output_index]+=".\t";tree+=",\"Terminal\":\".\"";}
    else ERROR();
    tree+="}}";
    if (index<SYM.size())
        ERROR();
}

void SUBPROGRAM()
{
    if (Error)return;
    tree+="\"Const Description\":{";
    output[output_index]+="ConstDT\t";
    output_index++;
    CONST_DESCRIPTION();
    tree+="},";
    output_index--;
    output[output_index]+="VarDT\t";
    output_index++;
    tree+="\"Var Description\":{";
    VAR_DESCRIPTION();
    tree+="},";
    output_index--;
    output[output_index]+="ProcedureDT\t";
    output_index++;
    tree+="\"Pocedure Descrition\":{";
    PROCEDURE_DESCRITION();
    tree+="},";
    output_index--;
    output[output_index]+="Statement\t";
    output_index++;
    if (!pro_index.empty())
    {
        int i=pro_index.top();
        table[i].adr=code.size();
        pro_index.pop();
    }
    else
        {first=code.size();code[0].a=first;}
    int num=0;
    for (int i=TX[TX_index];i<table_index;i++)
    {
        if (table[i].kind=="VARIABLE") num++;
        if (table[i].kind=="PROCEDURE") break;
    }
    emit("INT",0,num+3);
    tree+="\"Statement\":{";
    STATEMENT();
    tree+="}";
    emit("OPR",0,0);
    output_index--;
}

void CONST_DESCRIPTION()
{
    if (Error)return;
    if (SYM_WORD=="const")
    {
        ADVANCE();
        output[output_index]+="const ConstDF\t";
        tree+="\"Terminal\":\"const\",\"Const Defined\":{";
        output_index++;
        CONST_DEFINEDS();
        tree+="}";
        output_index--;
    }
    else
        output[output_index]+="NULL\t";
}

void CONST_DEFINEDS()
{
    if (Error)return;
    output[output_index]+="ConstD ConsetDA\t";
    output_index++;
    tree+="\"Const Defined\":{";
    CONST_DEFINED();
    tree+="},\"Const Defined Any\":{";
    CONST_DEFINED_ANY();
    tree+="}";
    output_index--;
}

void CONST_DEFINED_ANY()
{
    if (Error)return;
    if (SYM_WORD==",")
    {
        ADVANCE();
        output[output_index]+=", ConstD ConsetDA\t";
        tree+="\"Terminal\":\",\"";
        output_index++;
        tree+="\"Const Defined\":{";
        CONST_DEFINED();
        tree+="},\"Const Defined Any\":{";
        CONST_DEFINED_ANY();
        tree+="}";
        output_index--;
    }
    else if (SYM_WORD==";")
    {
        ADVANCE();
        tree+="\"Terminal\":\";\"";
        output[output_index]+=";\t";
    }
    else ERROR();
}

void CONST_DEFINED()
{
    if (Error)return;
    output[output_index]+="ID ";
    output_index++;
    string name=SYM_WORD;
    tree+="\"ID\":";
    IDENTIFIER();
    tree+=",";
    output_index--;
    if (SYM_WORD=="=")
    {
        ADVANCE();
        output[output_index]+="= ";
        tree+="\"Terminal_0\":\"=\",";
        if (is_unsignnum(SYM_WORD))
        {
            fill_table(name,"CONSTANT",SYM_WORD);
            tree+="\"Terminal_1\":\"";
            tree+=SYM_WORD;
            tree+="\"";
            output[output_index]+=SYM_WORD;
            output[output_index]+="\t";
            ADVANCE();
        }
        else
            ERROR();
    }
    else
        ERROR;
}

void VAR_DESCRIPTION()
{
    if (Error)return;
    vard=true;
    if (SYM_WORD=="var")
    {
        output[output_index]+="var VarD\t";
        ADVANCE();
        output_index++;
        tree+="\"Terminal\":\"var\",\"Var Defineds\":{";
        VAR_DEFINEDS();
        tree+="}";
        output_index--;
    }
    else
        output[output_index]+="NULL\t";
    vard=false;
}

void VAR_DEFINEDS()
{
    if (Error)return;
    output[output_index]+="ID IDAny";
    output_index++;
    fill_table(SYM_WORD,"VARIABLE","");
    tree+="\"ID\":";
    IDENTIFIER();
    tree+=",\"ID Any\":{";
    IDENTIFIER_ANY();
    tree+="},";
    output_index--;
    if (SYM_WORD==";")
    {
        tree+="\"Terminal\":\";\"";
        ADVANCE();
        output[output_index]+=";\t";
    }
    else ERROR();
}

void IDENTIFIER_ANY()
{
    if (Error)return;
    if (SYM_WORD==",")
    {
        tree+="\"Terminal\":\",\",";
        output[output_index]+=", ID IDAny\t";
        ADVANCE();
        output_index++;
        if (vard)fill_table(SYM_WORD,"VARIABLE","");
        tree+="\"ID\":";
        IDENTIFIER();
        tree+=",\"ID Any\":{";
        IDENTIFIER_ANY();
        tree+="}";
        output_index--;
    }
    else
        output[output_index]+="NULL\t";
}

void IDENTIFIER()
{
    if (Error)return;
    if (is_identifier(SYM_WORD))
    {
        if (is_read)
        {
            emit("OPR",0,12);
            int l,a;
            lookup(SYM_WORD,&l,&a);
            emit("STO",l,a);
        }
        tree+="\"";
        tree+=SYM_WORD;
        tree+="\"";
        output[output_index]+=SYM_WORD;
        output[output_index]+="\t";
        ADVANCE();
    }
    else ERROR();
}

void PROCEDURE_DESCRITION()
{
    if (Error)return;
    if (SYM_WORD=="procedure")
    {
        output[output_index]+="Procedures\t";
        output_index++;
        tree+="\"Procedures\":{";
        PROCEDURES();
        tree+="}";
        output_index--;
    }
    else
        output[output_index]+="NULL\t";
}

void PROCEDURES()
{
    if (Error)return;
    output[output_index]+="Procedure ProcedureA\t";
    output_index++;
    tree+="\"Procedure\":{";
    PROCEDURE();
    tree+="},\"Procedure Any\":{";
    PROCEDURE_ANY();
    tree+="}";
    output_index--;
}

void PROCEDURE_ANY()
{
    if (Error)return;
    if (SYM_WORD=="procedure")
    {
        output[output_index]+="Procedure ProcedureA\t";
        output_index++;
        tree+="\"Procedure\":{";
        PROCEDURE();
        tree+="},\"Procesure Any\":{";
        PROCEDURE_ANY();
        tree+="}";
        output_index--;
    }
    else
        output[output_index]+="NULL\t";
}

void PROCEDURE()
{
    if (Error)return;
    output[output_index]+="ProcedureHead Subprogram";
    output_index++;
    pro_index.push(table_index);
    tree+="\"Procesure Head\":{";
    PROCEDURE_HEAD();
    tree+="},";
    TX_index++;
    level++;
    BLOCK(table_index,level);
    level--;
    TX_index--;
    output_index--;
    if (SYM_WORD==";")
    {
        output[output_index]+=";\t";
        tree+=",\"Terminal\":\";\"";
        ADVANCE();
    }
    else
        ERROR();
}

void PROCEDURE_HEAD()
{
    if (Error)return;
    if (SYM_WORD=="procedure")
    {
        ADVANCE();
        output[output_index]+="procedure ID ";
        output_index++;
        fill_table(SYM_WORD,"PROCEDURE","");
        tree+="\"Terminal_0\":\"procedure\",\"ID\":";
        IDENTIFIER();
        tree+=",";
        output_index--;
        if (SYM_WORD==";")
        {
            tree+="\"Terminal_1\":\";\"";
            output[output_index]+=";\t";
            ADVANCE();
        }
        else
            ERROR();
    }
    else
        ERROR();
}

void STATEMENT()
{
    if (Error)return;
    if (is_identifier(SYM_WORD))
    {
        output[output_index]+="Assign\t";
        output_index++;
        tree+="\"Assign\":{";
        ASSIGN();
        tree+="}";
        output_index--;
    }
    else if (SYM_WORD=="if")
    {
        output[output_index]+="Conitional\t";
        output_index++;
        tree+="\"Conditional\":{";
        CONDITIONAL();
        tree+="}";
        output_index--;
    }
    else if (SYM_WORD=="while")
    {
        output[output_index]+="While\t";
        output_index++;
        tree+="\"While\":{";
        WHILE();
        tree+="}";
        output_index--;
    }
    else if (SYM_WORD=="call")
    {
        output[output_index]+="Call\t";
        output_index++;
        tree+="\"Call\":{";
        CALL();
        tree+="}";
        output_index--;
    }
    else if (SYM_WORD=="write")
    {
        output[output_index]+="Write\t";
        output_index++;
        tree+="\"Write\":{";
        WRITE();
        tree+="}";
        output_index--;
    }
    else if (SYM_WORD=="read")
    {
        output[output_index]+="Read\t";
        output_index++;
        tree+="\"Read\":{";
        READ();
        tree+="}";
        output_index--;
    }
    else if (SYM_WORD=="begin")
    {
        output[output_index]+="Complex\t";
        output_index++;
        tree+="\"Complex\":{";
        COMPLEX();
        tree+="}";
        output_index--;
    }
    else output[output_index]+="NULL\t";
}

void ASSIGN()
{
    if (Error)return;
    output[output_index]+="ID ";
    output_index++;
    int tl;int tdx;
    lookup(SYM_WORD,&tl,&tdx);
    tree+="\"ID\":";
    IDENTIFIER();
    tree+=",";
    output_index--;
    if (SYM_WORD==":=")
    {
        ADVANCE();
        output[output_index]+=":= Expression\t";
        output_index++;
        tree+="\"Terminal\":\":=\",\"Expression\":{";
        EXPRESSION();
        tree+="}";
        emit("STO",tl,tdx);
        output_index--;
    }
    else ERROR();
}

void COMPLEX()
{
    if (Error)return;
    if (SYM_WORD=="begin")
    {
        tree+="\"Terminal_0\":\"begin\",";
        output[output_index]+="begin Statements ";
        output_index++;
        ADVANCE();
        tree+="\"Statement\":{";
        STATEMENTS();
        tree+="},";
        output_index--;
        if (SYM_WORD=="end")
        {
            tree+="\"Terminal_1\":\"end\"";
            ADVANCE();
            output[output_index]+="end\t";
        }
        else
            ERROR();
    }
    else ERROR();
}

void STATEMENTS()
{
    if (Error)return;
    output[output_index]+="Statement StatementA\t";
    output_index++;
    tree+="\"Statement\":{";
    STATEMENT();
    tree+="},\"Statement Any\":{";
    STATEMENT_ANY();
    tree+="}";
    output_index--;
}

void STATEMENT_ANY()
{
    if (Error)return;
    if (SYM_WORD==";")
    {
        tree+="\"Terminal\":\";\",";
        output[output_index]+=";Statement StatementA\t";
        output_index++;
        ADVANCE();
        tree+="\"Statement\":{";
        STATEMENT();
        tree+="},\"Statement Any\":{";
        STATEMENT_ANY();
        tree+="}";
        output_index--;
    }
    else output[output_index]+="NULL\t";

}

void CONDITION()
{
    if (Error)return;
    if (SYM_WORD=="odd")
    {
        tree+="\"Terminal\":\"odd\",";
        output[output_index]+="odd Expression\t";
        output_index++;
        ADVANCE();
        tree+="\"Expression\":{";
        EXPRESSION();
        tree+="}";
        emit("LIT",0,2);
        emit("OPR",0,11);
        output_index--;
    }
    else
    {
        output[output_index]+="Expression ";
        output_index++;
        tree+="\"Expression_0\":{";
        EXPRESSION();
        tree+="},";
        output_index--;
        if (is_relational(SYM_WORD))
        {
            tree+="\"Terminal\":\"";
            tree+=SYM_WORD;
            tree+="\",";
            output[output_index]+=SYM_WORD;
            output[output_index]+=" Expression\t";
            int rela=0;
            for(int i=0;i<12;i++)
                if (SYM_WORD==opra[i]){cout<<"debug\t"<<SYM_WORD<<" "<<i<<endl; rela=i;}
            ADVANCE();
            output_index++;
            tree+="\"Expression_1\":{";
            EXPRESSION();
            tree+="}";
            emit("OPR",0,rela);
            output_index--;
        }
        else ERROR();
    }
}

void EXPRESSION()
{
    if (Error)return;
    if (SYM_WORD=="-")
    {
        opr.push(SYM_WORD);
        emit("LIT",0,0);
    }
    if (SYM_WORD=="+"||SYM_WORD=="-")
    {
        tree+="\"Terminal\":\"";
        tree+=SYM_WORD;
        tree+="\",";
        output[output_index]+=SYM_WORD;
        ADVANCE();
    }
    output[output_index]+=" Items \t";
    output_index++;
    tree+="\"Items\":{";
    ITEMS();
    tree+="}";
    output_index--;
    if (is_write)
        emit("OPR",0,13);
}

void EXPRESSIONS()
{
    if (Error)return;
    output[output_index]+="Expression ExpressionA\t";
    output_index++;
    tree+="\"Expression\":{";
    EXPRESSION();
    tree+="},\"Expression Any\":{";
    EXPRESSION_ANY();
    tree+="}";
    output_index--;
}

void EXPRESSION_ANY()
{
    if (Error)return;
    if (SYM_WORD==",")
    {
        ADVANCE();
        output[output_index]+=",Expression ExpressionA\t";
        output_index++;
        tree+="\"Terminal\":\",\",";
        tree+="\"Expression\":{";
        EXPRESSION();
        tree+="},\"Expression Any\":{";
        EXPRESSION_ANY();
        tree+="}";
        output_index--;
    }
    else output[output_index]+="NULL\t";
}

void ITEMS()
{
    if (Error)return;
    output[output_index]+="Item ItemA\t";
    output_index++;
    tree+="\"Item\":{";
    ITEM();
    tree+="},";
    if (!opr.empty())
    {
        if (opr.top()=="-")
        {
            emit("OPR",0,3);
            opr.pop();
        }
    }
    tree+="\"Item Any\":{";
    ITEM_ANY();
    tree+="}";
    output_index--;
}

void ITEM_ANY()
{
    if (Error)return;
    if (SYM_WORD=="+"||SYM_WORD=="-")
    {
        tree+="\"Terminal\":\"";
        tree+=SYM_WORD;
        tree+="\",";
        output[output_index]+=SYM_WORD;
        output[output_index]+="Item ItemA\t";
        opr.push(SYM_WORD);
        ADVANCE();
        output_index++;
        tree+="\"Item\":{";
        ITEM();
        tree+="},";
        if (opr.top()=="+")
            emit("OPR",0,1);
        else
        if (opr.top()=="-")
            emit("OPR",0,2);
        else cout<<"opr.top()"<<opr.top()<<endl;;
        opr.pop();
        tree+="\"Item Any\":{";
        ITEM_ANY();
        tree+="}";
        output_index--;
    }
    else output[output_index]+="NULL\t";
}

void ITEM()
{
    if (Error)return;
    output[output_index]+="Factor FactorA\t";
    output_index++;
    tree+="\"Factor\":{";
    FACTOR();
    tree+="},\"Factor Any\":{";
    FACTOR_ANY();
    tree+="}";
    output_index--;
}

void FACTOR_ANY()
{
    if (Error)return;
    if (SYM_WORD=="*"||SYM_WORD=="/")
    {
        opr.push(SYM_WORD);
        tree+="\"Terminal\":\"";
        tree+=SYM_WORD;
        tree+="\",";
        output[output_index]+=SYM_WORD;
        output[output_index]+="Factor FactorA\t";
        output_index++;
        tree+="\"Facotr\":{";
        ADVANCE();
        FACTOR();
        tree+="},\"Factor Any\":{";
        FACTOR_ANY();
        tree+="}";
        output_index--;
    }
    else output[output_index]+="NULL\t";
}

void FACTOR()
{
    if (Error)return;
    if (is_identifier(SYM_WORD)||is_unsignnum(SYM_WORD))
    {
        if (!opr.empty()&&(opr.top()=="*"||opr.top()=="/"))
        {
            putonstack(SYM_WORD);
            if (opr.top()=="*")
                emit("OPR",0,3);
            if (opr.top()=="/")
                emit("OPR",0,4);
            opr.pop();
        }
        else
            putonstack(SYM_WORD);
        tree+="\"Terminal\":\"";
        tree+=SYM_WORD;
        tree+="\"";
        output[output_index]+=SYM_WORD;
        output[output_index]+="\t";
        ADVANCE();
    }
    else if (SYM_WORD=="(")
    {
        opr.push(SYM_WORD);
        tree+="\"Terminal_0\":\"";
        tree+=SYM_WORD;
        tree+="\",";
        output[output_index]+="(";
        output[output_index]+="Expression ";
        ADVANCE();
        output_index++;
        tree+="\"Expression\":{";
        EXPRESSION();
        tree+="},";
        output_index--;
        if (SYM_WORD==")")
        {
            opr.pop();
            tree+="\"Terminal_1\":\"";
            tree+=SYM_WORD;
            tree+="\"";
            output[output_index]+=")\t";
            ADVANCE();
        }
        else
            ERROR();
    }
    else ERROR();
}

void CONDITIONAL()
{
    if (Error)return;
    if (SYM_WORD=="if")
    {
        output[output_index]+="if ";
        output[output_index]+="Condition ";
        output_index++;
        tree+="\"Terminal_0\":\"";
        tree+=SYM_WORD;
        tree+="\",";
        ADVANCE();
        tree+="\"Condition\":{";
        CONDITION();
        tree+="},";
        emit("JPC",0,0);
        int index=code.size()-1;
        output_index--;
        if (SYM_WORD=="then")
        {
            tree+="\"Terminal_1\":\"";
            tree+=SYM_WORD;
            tree+="\",";
            output[output_index]+="then Statement\t";
            ADVANCE();
            output_index++;
            tree+="\"Statement\":{";
            STATEMENT();
            tree+="}";
            code[index].a=code.size();
            output_index--;
        }
        else ERROR();
    }
    else ERROR();
}

void CALL()
{
    if (Error)return;
    if (SYM_WORD=="call")
    {
        tree+="\"Terminal_0\":\"";
        tree+=SYM_WORD;
        tree+="\",";
        output[output_index]+="call ";
        ADVANCE();
        if (is_identifier(SYM_WORD))
        {
            bool findit=false;
            int l=0,a=0;
            for (int i=TX[TX_index];i<table_index;i++)
                if(table[i].name==SYM_WORD&&table[i].kind=="PROCEDURE"&&table[i].level==level)
                {
                    if (findit) {ERROR();break;}
                    findit=true;
                    l=level-table[i].level;
                    a=table[i].adr;
                }
            for (int i=TX[TX_index-1];i<TX[TX_index];i++)
                if(table[i].name==SYM_WORD&&table[i].kind=="PROCEDURE"&&table[i].level==(level-1))
                {
                    if (findit) {ERROR();break;}
                    findit=true;
                    l=level-table[i].level;
                    a=table[i].adr;
                }
            if (!findit) {ERROR();return;}
            emit("CAL",l,a);
            tree+="\"Terminal_1\":\"";
            tree+=SYM_WORD;
            tree+="\"";
            output[output_index]+=SYM_WORD;
            output[output_index]+="\t";
            ADVANCE();
        }
        else
            ERROR();
    }
    else
        ERROR();
}

void WHILE()
{
    if (Error)return;
    if (SYM_WORD=="while")
    {
        output[output_index]+="while ";
        output[output_index]+="Condition ";
        output_index++;
        tree+="\"Terminal_0\":\"";
        tree+=SYM_WORD;
        tree+="\",";
        ADVANCE();
        int index0=code.size();
        tree+="\"Condition\":{";
        CONDITION();
        tree+="},";
        int index=code.size();
        emit("JPC",0,0);
        output_index--;
        if (SYM_WORD=="do")
        {
            output[output_index]+="do ";
            output[output_index]+="Statement\t";
            output_index++;
            tree+="\"Terminal_1\":\"";
            tree+=SYM_WORD;
            tree+="\",";
            ADVANCE();
            tree+="\"Statement\":{";
            STATEMENT();
            tree+="}";
            emit("JMP",0,index0);
            code[index].a=code.size();
            output_index--;
        }
        else ERROR();
    }
    else
        ERROR();
}

void READ()
{
    if (Error)return;
    is_read=true;
    if (SYM_WORD=="read")
    {
        tree+="\"Terminal_0\":\"";
        tree+=SYM_WORD;
        tree+="\",";
        output[output_index]+="read ";
        ADVANCE();
        if (SYM_WORD=="(")
        {
            tree+="\"Terminal_1\":\"";
            tree+=SYM_WORD;
            tree+="\",";
            output[output_index]+="(";
            output[output_index]+="Identifier IdentifierA";
            output_index++;
            ADVANCE();
            tree+="\"ID\":";
            IDENTIFIER();
            tree+=",\"ID Any\":{";
            IDENTIFIER_ANY();
            tree+="},";
            output_index--;
            if (SYM_WORD==")")
            {
                tree+="\"Terminal_2\":\"";
                tree+=SYM_WORD;
                tree+="\"";
                output[output_index]+=")\t";
                ADVANCE();
            }
            else  ERROR();
        }
    }
    else ERROR();
    is_read=false;
}

void WRITE()
{
    is_write=true;
    if (Error)return;
    if (SYM_WORD=="write")
    {
        tree+="\"Terminal_0\":\"";
        tree+=SYM_WORD;
        tree+="\",";
        output[output_index]+="write";
        ADVANCE();
        if (SYM_WORD=="(")
        {
            tree+="\"Terminal_1\":\"";
            tree+=SYM_WORD;
            tree+="\",";
            output[output_index]+="( Expression ExpressionA ";
            output_index++;
            ADVANCE();
            tree+="\"Expression\":{";
            EXPRESSION();
            tree+="},\"Expression Any\":{";
            EXPRESSION_ANY( );
            tree+="},";
            output_index--;
            if (SYM_WORD==")")
            {
                tree+="\"Terminal_2\":\"";
                tree+=SYM_WORD;
                tree+="\"";
                output[output_index]+=")\t";
                ADVANCE();
            }
            else  ERROR();
        }
    }
    else ERROR();
    is_write=false;
}

bool is_relational(string s)
{
    return (s=="="||s=="#"||s=="<"||s=="<="||s==">"||s==">=");
}
