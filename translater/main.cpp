#include <iostream>
#include <map>
#include <vector>
#include <fstream>

using namespace std;

/********************************* defind keywords ********************************/
const int KWNUM=15;
const string KEYWORD[] = {"IDENT","NUMBER","CONST","VAR","PROCEDURE","BEGIN","END","ODD","IF","THEN",
                          "CALL","WHILE","DO","READ","WRITE"};
const string symble[] = {"+","-","*","/","#","<","<=",">",">=","=",":=",",",".",";","(",")"};
/**********************************************************************************/

map<string,int> KWTABLE; //map to find the kind of word

vector<pair<int,int> > SYM;
vector<string> ID;
vector<string> NUM;

fstream file("test.txt");     //source code
ofstream result("word1.txt"); //output word file

// look up the map for kind
int Reserve(string tem){if (KWTABLE.find(tem)!=KWTABLE.end())return KWTABLE.find(tem)->second;else return 0;}
void GETSYM();

int main()
{
    for (int i = 0;i < KWNUM;i++) KWTABLE.insert(make_pair(KEYWORD[i],i)); //make the map
    GETSYM(); //main work
    /************************** display on console ********************************/
    cout<<" ----------------------------------------------- "<<endl;
    cout<<"|      SYM      |       ID      |      NUM      |"<<endl;
    cout<<"|-----------------------------------------------|"<<endl;
    for (int i=0;i<SYM.size();i++)
    {
        cout <<"|<" << SYM[i].first << "," << SYM[i].second << ">\t";
        if (SYM[i].first<15)
            cout << KEYWORD[SYM[i].first].substr(0,7) <<"\t|";
        else
            cout << symble[SYM[i].first-15]<<"\t|";
        if (i<ID.size())
            cout<< i << "\t" << ID[i].substr(0,7) << "\t|";
        else cout<<i<<"\t\t|";
        if (i<NUM.size())
            cout<< i << "\t" << NUM[i] << "\t|"<< endl;
        else cout<<i<<"\t\t|"<<endl;
    }
    cout<<"-------------------------------------------------"<<endl;
    /*******************************************************************************/

    /*******************************  write to file ********************************/
    result<<SYM.size()<<" "<<ID.size()<<" "<<NUM.size()<<endl;
    for (int i=0;i<SYM.size();i++)
        result<< SYM[i].first << " " << SYM[i].second<<endl;
    for (int i=0;i<ID.size();i++)
        result<< ID[i] <<endl;
    for (int i=0;i<NUM.size();i++)
        result<< NUM[i] <<endl;
    /*******************************************************************************/
    return 0;
}

void GETSYM()
{
    char c;
    int code,value;
    string strToken;
    while (!file.eof())
    {
        strToken = "";
        file.get(c);
        while (c == ' ' || c == '\n' || c == '\t')
        {
            file.get(c);
            if (file.eof()) return;
        }
        if (file.eof()) return;
        /*
        GetChar();
        GetBC();
        */
        if (isalpha(c)) //isLetter()
        {
            while(isalnum(c)) //isLetter() or isDigit()
            {
                strToken.push_back(c); //Concat();
                file.get(c);           //GetChar();
                if (file.eof()) break;
                //Retract();
            }
            if (!file.eof())file.unget();
            /**************** Case conversion ******************/
            string tem="";
            for (int i=0;i<strToken.length();i++)
                if (strToken[i]<=122&&strToken[i]>=97) tem+=strToken[i]-32;
                    else tem+=strToken[i];
            /***************************************************/
            code = Reserve(tem);
            if (code != 0)
                SYM.push_back(make_pair(code,0));  //keyword
            else
            {
                if (strToken.length()>10)
                {
                    cout << "The length of the identifier exceeds ten." << endl;
                    return;
                }
                /************* Filter duplicate words ************/
                bool f=false;
                for (int i=0;i<ID.size();i++)
                    if (ID[i]==strToken){f=true;value=i;}
                if (!f)
                {
                    value = ID.size();
                    ID.push_back(strToken);
                }
                /**************************************************
                value = ID.size();
                ID.push_back(strToken);
                /**************************************************/
                SYM.push_back(make_pair(0,value));
            }
        }else if(isdigit(c))
        {
            while(isdigit(c)) //isLetter() or isDigit()
            {
                strToken.push_back(c); //Concat();
                file.get(c);           //GetChar();
                if (file.eof()) break;
                //Retract();
            }
            if (!file.eof())file.unget();
            /**************** Filter duplicate numbers ****************/
            bool f=false;
            for (int i=0;i<NUM.size();i++)
                if (NUM[i]==strToken){ f=true;value=i;}
            if (!f)
            {
                value = NUM.size();
                NUM.push_back(strToken);
            }
            /**********************************************************
            value = NUM.size();
            NUM.push_back(strToken);
            /**********************************************************/
            SYM.push_back(make_pair(1,value));
        }else if (c == '+')
            SYM.push_back(make_pair(15,0));
        else if (c == '-')
            SYM.push_back(make_pair(16,0));
        else if (c == '*')
            SYM.push_back(make_pair(17,0));
        else if (c == '/')
            SYM.push_back(make_pair(18,0));
        else if (c == '#')
            SYM.push_back(make_pair(19,0));
        else if (c == '<' || c == '>')
        {
            char cc = file.get();
            if (cc == '=')
            {
                if (c == '<') SYM.push_back(make_pair(21,0));
                if (c == '>') SYM.push_back(make_pair(23,0));
            }
            else
            {
                if (c == '<') SYM.push_back(make_pair(20,0));
                if (c == '>') SYM.push_back(make_pair(22,0));
                file.unget();
            }
        }else if (c == '=')
            SYM.push_back(make_pair(24,0));
        else if (c == ':')
        {
            char cc = file.get();
            if (cc == '=') SYM.push_back(make_pair(25,0));
            else return;
        }else if (c == ',')
            SYM.push_back(make_pair(26,0));
        else if (c == '.')
            SYM.push_back(make_pair(27,0));
        else if (c == ';')
            SYM.push_back(make_pair(28,0));
        else if (c == '(')
            SYM.push_back(make_pair(29,0));
        else if (c == ')')
            SYM.push_back(make_pair(30,0));
        else return;
    }
}
