#include <iostream>
#include <fstream>
#include <stack>
#include <vector>

using namespace std;

typedef struct CODE
{
    string f;
    int l;
    int a;
} Code;
vector<Code> code;

fstream CODE("code.txt");
string opra[]={"ret","+","-","*","/","#","<","<=",">",">=","=","%","read","write"};
vector<int> Stack;
int P=0;
Code I;
int T;
int B;

void exec(Code I);
void output();

int main()
{
    string opr;
    int l,a;
    while (!CODE.eof())
    {
         Code tem;
         CODE>>tem.f;
         CODE>>tem.l;
         CODE>>tem.a;
         code.push_back(tem);
    } //read code
    /***************  jump to the entry and initialize  ******************/
    P=code[0].a; I=code[P];
    P++; B=0;
    for (int i=0;i<I.a;i++)Stack.push_back(0);
    T=I.a;
    Stack[0]=0;
    Stack[1]=0;
    Stack[2]=0;
    /********************************************************************/
    while (!(B==0&&T==0))
    {
        I=code[P];
        P++;
        output();
        exec(I);
    }
    output();
    return 0;
}

void output()
{
    cout<<"P:"<<P<<"\tT:"<<T<<"\tB:"<<B<<"\tI:"<<I.f<<" "<<I.l<<" "<<I.a<<"\t";
    for (int i=0;i<T;i++)
        cout<<Stack[i]<<" ";
    cout<<endl;
}

void exec(Code I)
{
    if (I.f=="LIT")
    {
        if (Stack.size()>T)
            Stack[T]=I.a;
        else Stack.push_back(I.a);
        T++;
    }else
    if (I.f=="LOD")
    {
        int base=B;
        for (int i=0;i<I.l;i++)
            base=Stack[base];
        if (Stack.size()>T)
            Stack[T]=Stack[base+I.a];
        else Stack.push_back(Stack[base+I.a]);
        T++;
    }else
    if (I.f=="STO")
    {
        int base=B;
        for (int i=0;i<I.l;i++)
            base=Stack[base];
        T--;
        Stack[base+I.a]=Stack[T];
    }else
    if (I.f=="CAL")
    {
        int ret=P;
        int level=I.l;
        P=I.a;
        I=code[P];
        P++;
        for (int i=0;i<I.a;i++)
        {
            if (Stack.size()>T) Stack[T]=0;
            else Stack.push_back(0);
            T++;
        }
        if (level==0)Stack[T-I.a]=B;
        else if (level==1)Stack[T-I.a]=Stack[B];
        Stack[T-I.a+1]=B;
        Stack[T-I.a+2]=ret;
        B=T-I.a;
    }else
    if (I.f=="INT")
    {
        /***NULL***/
    }else
    if (I.f=="JMP")
    {
        P=I.a;
    }else
    if (I.f=="JPC")
    {
        T--;
        if (!Stack[T]) P=I.a;
    }else
    if (I.f=="OPR")
    {
        int base;
        switch (I.a)
        {
        case 0:
            P=Stack[B+2];
            base=B;
            B=Stack[B+1];
            T=base;
            break;
        case 1:
            T-=1;
            Stack[T-1]=Stack[T-1]+Stack[T];
            break;
        case 2:
            T-=1;
            Stack[T-1]=Stack[T-1]-Stack[T];
            break;
        case 3:
            T-=1;
            Stack[T-1]=Stack[T-1]*Stack[T];
            break;
        case 4:
            T-=1;
            Stack[T-1]=Stack[T-1]/Stack[T];
            break;
        case 5:
            T-=1;
            if (Stack[T-1]!=Stack[T]) Stack[T-1]=1;
            else Stack[T-1]=0;
            break;
        case 6:
            T-=1;
            if (Stack[T-1]<Stack[T]) Stack[T-1]=1;
            else Stack[T-1]=0;
            break;
        case 7:
            T-=1;
            if (Stack[T-1]<=Stack[T]) Stack[T-1]=1;
            else Stack[T-1]=0;
            break;
        case 8:
            T-=1;
            if (Stack[T-1]>Stack[T]) Stack[T-1]=1;
            else Stack[T-1]=0;
            break;
        case 9:
            T-=1;
            if (Stack[T-1]>=Stack[T]) Stack[T-1]=1;
            else Stack[T-1]=0;
            break;
        case 10:
            T-=1;
            if (Stack[T-1]==Stack[T]) Stack[T-1]=1;
            else Stack[T-1]=0;
            break;
        case 11:
            T-=1;
            Stack[T-1]=Stack[T-1]%Stack[T];
            break;
        case 12:
            if (Stack.size()<=T) Stack.push_back(0);
            cin>>Stack[T];
            T++;
            break;
        case 13:
            T--;
            cout<<Stack[T]<<endl;
            break;
        }
    }
}
