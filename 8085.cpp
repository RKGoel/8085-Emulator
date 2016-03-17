#include<iostream>
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<map>

using namespace std;

int mem[65536]; //2^16 = 65536
map<int,int> validmem; //maps memory locations with valid memory values
int a=0,b=0,c=0,d=0,e=0,h=0,l=0,sp=0,pc=0,m=0; //all reg have dec values
bool flags[4]; //0=Zero, 1=Carry, 2=Sign, 3=Overflow
map<string,int> mymap; //maps tags with their line numbers

int getIndex(char[]); //gives unique index to each instructions
int charToInt(char[]); //changes hex char string to dec int value
void decToHex(int,int[]); //returns int arr having digits for hex value in reverse order
void display();             //display final statuses
void goToLine(ifstream&,int); //takes file ptr to given line number
int getRegVal(char); //returns value of register passed
void putRegVal(char, int); //assigns int value to passed register

int main()
{
    //File reading for memory initialization
    ifstream m("Memory.txt");
    m.seekg(0);
    while(!m.eof())
    {
        char hexval[5], tempch;
        int decval, memval;
        m.get(hexval,5,'H');
        decval = charToInt(hexval);
        m.get(tempch); //tempch reads 'H'
        m.get(tempch); //temch reads space
        m>>memval;
        mem[decval] = memval;
        validmem[decval] = memval;
        m.get(tempch);
        while(tempch!='\n' && !m.eof())
        m.get(tempch);
    }

    // Flag initialization
    for(int i=0;i<4;i++)
        flags[i]=false;

    char ch[10], ch1, ch2,ch3, value[5], value1[3], value2[3]; //ch1 is 1st reg, ch2 is 2nd reg
    int index, val, val1,val2,pos;
    char tag[10]; int linenum=0;
    int mapvalues[10]; //will have all linenums for valid tags
    string tag1;
    ifstream f("8085.txt");

    //Block to read tags
    f.seekg(0);
    int j=0;
    while(!f.eof())
    {
        tag[0]='\0';
        string line; int i=0;
        getline(f,line);
        linenum++;
        char *ptr = &line.at(0);
        if(strchr(ptr,':')!=NULL)
        {
            while(line.at(i)!=':')
            {
                tag[i]=line.at(i);
                i++;
            }
            tag[i]='\0';
        }
        if(tag[0]!='\0')
        {
            mymap[tag]=linenum;
            mapvalues[j]=linenum;
            j++;
        }
    }
    while(j<10)
    {
        mapvalues[j]=0;
        j++;
    }

    /*char test[10] = "BCK";
    cout<<"\nfind = "<<mymap.find(test)->second;
    cout<<"\nfind2 = "<<mymap.find("AHEAD")->second;*/

    //Block to execute program
    f.seekg(0);
    linenum=1;
    while(!f.eof())
    {
        for(int j=0;mapvalues[j]!=0;j++)
        {
            if(linenum==mapvalues[j])
            {
                char chtemp[10], cht; //all these will be discarded just to move file pointer ahead
                f.get(chtemp,10,':'); //chtemp will have read tag
                f.get(ch1); //reads colon
                f.get(ch1); //reads space
                break;
            }
        }
        linenum++;
        f.get(ch,5,' ');
        index = getIndex(ch);
        switch(index)
        {
            case 0: goto ahead; //HLT
            case 1: f.get(ch1); //LDI
                    if(ch1!=' ')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                    f.get(ch1);
                    f.get(value,5,'H');
                    val = charToInt(value); //changes hex char string to dec int value
                    putRegVal(ch1,val);
                    break;

            case 2: f.get(ch1); //LDA
                    if(ch1!=' ')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                    f.get(value,6,'H');
                    val = charToInt(value);
                    a = mem[val];
                    break;

            case 3: f.get(ch1); //LXI
                    if(ch1!=' ')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                    f.get(ch1);
                    if(ch1=='S') //For SP
                        f.get(ch1); //reads 'P'
                    f.get(ch2);
                    if(ch2!=',')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                    f.get(value1,3);
                    f.get(value2,3,'H');
                    val1 = charToInt(value1);
                    val2 = charToInt(value2);
                     if(ch1=='P')
                    {
                        strcat(value1,"00");
                        val1 = charToInt(value1);
                        sp = val1+val2-1;
                    }
                    else if(ch1=='H')
                    {
                        h = val1;
                        l = val2;
                    }
                    else if(ch1=='B')
                    {
                        b = val1;
                        c = val2;
                    }
                    else if(ch1=='D')
                    {
                        d = val1;
                        e = val2;
                    }
                    else
                        cout<<"Syntax Error in line "<<linenum-1;
                    break;

            case 4: f.get(ch2); //MOV
                    if(ch2!=' ')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                    f.get(ch2);
                    f.get(ch1);
                    if(ch1!=',')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                    f.get(ch1);
                    int temp;
                    temp = getRegVal(ch1);
                    putRegVal(ch2,temp);
                    break;

            case 5: f.get(ch1); //INX
                    if(ch1!=' ')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                    f.get(ch1);
                    if(ch1=='H')
                    {
                        if(l==255)
                        {
                            l=0;
                            h=h+1;
                        }
                        else
                            l=l+1;
                    }
                    else if(ch1=='B')
                    {
                        if(c==255)
                        {
                            c=0;
                            b=h+1;
                        }
                        else
                            c=c+1;
                    }
                    else if(ch1=='D')
                    {
                        if(e==255)
                        {
                            e=0;
                            d=d+1;
                        }
                        else
                            e=e+1;
                    }
                    else
                        cout<<"Syntax Error in line "<<linenum-1;
                        break;

            case 6: f.get(ch1); //ADD
                    if(ch1!=' ')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                    f.get(ch1);
                    int te1;
                    te1 = getRegVal(ch1);
                    putRegVal('A',a+te1);
                    break;

            case 7: f.get(ch1); //STA
                    if(ch1!=' ')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                    f.get(value,6,'H');
                    val = charToInt(value);
                    mem[val] = a;
                    validmem[val]=a;
                    break;


            case 8: if(flags[0]!=true) //JNZ
                    {
                        f.get(ch1);
                        if(ch1!=' ')
                        {
                            cout<<"Syntax Error in line "<<linenum-1;
                            exit(0);
                        }
                        getline(f,tag1,'\n');
                        linenum = mymap[tag1];
                        goToLine(f,linenum);
                    }
                    break;


            case 9: f.get(ch1); //CMP
                    if(ch1!=' ')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                    f.get(ch1);
                    int te;
                    te = getRegVal(ch1);
                    if(te>a)
                        flags[1]= true; //Carry flag set
                    else
                        flags[1]= false; //Carry flag reset
                    break;

            case 10: if(flags[1]!=true) //JNC
                     {
                        f.get(ch1);
                        if(ch1!=' ')
                        {
                            cout<<"Syntax Error in line "<<linenum-1;
                            exit(0);
                        }
                        getline(f,tag1,'\n');
                        linenum = mymap[tag1];
                        goToLine(f,linenum);
                     }
                     break;

            case 11: f.get(ch1); //MVI
                     if(ch1!=' ')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                     f.get(ch1);
                     f.get(value,6,'H');
                     val = charToInt(value);
                     putRegVal(ch1,val);
                    break;

            case 12: if(flags[0]==true) //JZ
                     {
                        f.get(ch1);
                        if(ch1!=' ')
                        {
                            cout<<"Syntax Error in line "<<linenum-1;
                            exit(0);
                        }
                        getline(f,tag1,'\n');
                        linenum = mymap[tag1];
                        goToLine(f,linenum);
                     }
                     break;

            case 13: f.get(ch1); //DCR
                     if(ch1!=' ')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                     f.get(ch1);
                     int te2;
                     te2 = getRegVal(ch1);
                     putRegVal(ch1,te2-1);
                     if(te2-1==0)
                        flags[0]=true; //zero flag set
                     else
                        flags[0]=false; //zero flag reset
                    break;

            case 14: int temp1; //XCHG
                     temp1 = h;
                     h = d;
                     d = temp1;
                     temp1 = l;
                     l = e;
                     e = temp1;
                     break;

            case 15: f.get(ch1); //INR
                     if(ch1!=' ')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                     f.get(ch1);
                     te2 = getRegVal(ch1);
                     putRegVal(ch1,te2+1);
                     if(te2+1>=256){
                        flags[3]=true; //overflow flag set
                        putRegVal(ch1,te2-255);
                     }
                     else
                        flags[3]=false; //overflow flag reset
                    break;

            case 16: f.get(ch1); //DCX
                    if(ch1!=' ')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                    f.get(ch1);
                    if(ch1=='H')
                    {
                        if(h!=0 || l!=0)
                        {
                            if(l==0)
                            {
                                l=255;
                                h--;
                            }
                            else
                            l--;
                        }
                    }
                    else if(ch1=='B')
                    {
                       if(b!=0 || c!=0)
                        {
                            if(c==0)
                            {
                                c=255;
                                b--;
                            }
                            else
                            c--;
                        }
                    }
                    else if(ch1=='D')
                    {
                        if(d!=0 || e!=0)
                        {
                            if(e==0)
                            {
                                e=255;
                                d--;
                            }
                            else
                            e--;
                        }
                    }
                    else
                        cout<<"Syntax Error in line "<<linenum-1;
                        break;

            case 17: f.get(ch1); //PUSH
                     if(ch1!=' ')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                     f.get(ch1);
                     if(ch1=='H')
                     {
                         sp++;
                         mem[sp]=getRegVal(ch1);
                         validmem[sp]=mem[sp];
                         sp++;
                         mem[sp]=getRegVal('L');
                         validmem[sp]=mem[sp];
                     }
                     else if(ch1=='B')
                     {
                         sp++;
                         mem[sp]=getRegVal(ch1);
                         validmem[sp]=mem[sp];
                         sp++;
                         mem[sp]=getRegVal('C');
                         validmem[sp]=mem[sp];
                     }
                     else if(ch1=='D')
                     {
                         sp++;
                         mem[sp]=getRegVal(ch1);
                         validmem[sp]=mem[sp];
                         sp++;
                         mem[sp]=getRegVal('E');
                         validmem[sp]=mem[sp];
                     }
                     break;

            case 18: f.get(ch1); //POP
                     if(ch1!=' ')
                    {
                        cout<<"Syntax Error in line "<<linenum-1;
                        exit(0);
                    }
                     f.get(ch1);
                     putRegVal(ch1,mem[sp]);
                     sp--;
                     break;

            case 19:  //XTHL
                     temp1 = l;
                     l = mem[sp];
                     mem[sp] = temp1;
                     validmem[sp]=temp1;
                     sp--;
                     temp1 = h;
                     h = mem[sp];
                     mem[sp] = temp1;
                     validmem[sp]=temp1;
                     sp++;
                     break;

            case 20: int temparr[4],temparr1[2],temph,templ; //SPHL
                     temph = h;
                     templ = l;
                     if(sp<=255)
                     {
                         h = 0;
                         l = sp;
                     }
                     else
                     {
                         decToHex(sp,temparr);
                         l = (16*temparr[1])+temparr[0];
                         h = (16*temparr[3])+temparr[2];
                     }
                     decToHex(temph,temparr1);
                     sp = templ+(4096*temparr1[1])+(256*temparr1[0]);
                     break;

            case 21: if(flags[1]==true) //JC
                     {
                        f.get(ch1);
                        if(ch1!=' ')
                        {
                            cout<<"Syntax Error in line "<<linenum-1;
                            exit(0);
                        }
                        getline(f,tag1,'\n');
                        linenum = mymap[tag1];
                        goToLine(f,linenum);
                     }
                     break;

            default: cout<<"Syntax Error in line "<<linenum-1;
                     exit(0);

        }//end of switch
        f.get(ch3);
        while(ch3!='\n')
        f.get(ch3);
    }//end of while
    ahead: f.close();
    display();
    return(0);
}//end of main()

int getIndex(char ch[])
{
    if(strcmp(ch,"HLT")==0)
        return(0);
    else if(strcmp(ch,"LDI")==0)
        return(1);
    else if(strcmp(ch,"LDA")==0)
        return(2);
    else if(strcmp(ch,"LXI")==0)
        return(3);
    else if(strcmp(ch,"MOV")==0)
        return(4);
    else if(strcmp(ch,"INX")==0)
        return(5);
    else if(strcmp(ch,"ADD")==0)
        return(6);
    else if(strcmp(ch,"STA")==0)
        return(7);
    else if(strcmp(ch,"JNZ")==0)
        return(8);
    else if(strcmp(ch,"CMP")==0)
        return(9);
    else if(strcmp(ch,"JNC")==0)
        return(10);
    else if(strcmp(ch,"MVI")==0)
        return(11);
    else if(strcmp(ch,"JZ")==0)
        return(12);
    else if(strcmp(ch,"DCR")==0)
        return(13);
    else if(strcmp(ch,"XCHG")==0)
        return(14);
    else if(strcmp(ch,"INR")==0)
        return(15);
    else if(strcmp(ch,"DCX")==0)
        return(16);
    else if(strcmp(ch,"PUSH")==0)
        return(17);
    else if(strcmp(ch,"POP")==0)
        return(18);
    else if(strcmp(ch,"XTHL")==0)
        return(19);
    else if(strcmp(ch,"SPHL")==0)
        return(20);
    else if(strcmp(ch,"JC")==0)
        return(21);
    else
        return(-1);
}

int charToInt(char ch[])
{
    int len, i, temp=0, temp1;
    for(len=0;ch[len]!='\0';len++);
    for(len=0;ch[len]!='\0';len++);
    for(i=len-1;i>=0;i--)
    {
        temp1 = pow(16,len-i-1);
        if(ch[i]>=48 && ch[i]<=57) //ASCII value of 0 is 48 and 9 is 57
            temp = temp+((ch[i]-48)*temp1);
        else if(ch[i]>=65 && ch[i]<=70)
            temp = temp+((ch[i]-55)*temp1);
    }
    return(temp);
}

void decToHex(int a, int hex[]) //returns in reverse order
{
    int i,j=0;
    if(a>255)
        i=3;
    else
        i=1;

        while(i>=0)
        {
            hex[i] = a/pow(16,i);
            a = a-(hex[i]*pow(16,i));
            i--;
        }

}

void display()
{
    int temp[4];
    char temp1[4];
    cout<<"Memory Map:\n";
    for(map<int,int>::iterator it=validmem.begin();it!=validmem.end();it++)
    {
        decToHex(it->first,temp);
        for(int i=3;i>=0;i--)
        {
            if(temp[i]<10)
                temp1[i]=temp[i]+48;
            else
                temp1[i]=temp[i]+55;
            cout<<temp1[i];
        }
        cout<<"H => "<<it->second<<"\n";
    }

    int hex[7][2],i,j;
    char chex[7][2];
    decToHex(a, hex[0]);
    decToHex(b,hex[1]);
    decToHex(c,hex[2]);
    decToHex(d,hex[3]);
    decToHex(e,hex[4]);
    decToHex(h,hex[5]);
    decToHex(l,hex[6]);
    for(i=0;i<7;i++)
    {
        for(j=0;j<2;j++)
        {
            if(hex[i][j]<10)
                chex[i][j] = hex[i][j]+48;
            else
                chex[i][j] = hex[i][j]+55;
        }
    }
    cout<<"\nRegisters Status:\n";
    cout<<"A="<<chex[0][1]<<chex[0][0]<<"H\nB="<<chex[1][1]<<chex[1][0]<<"H\nC="<<chex[2][1]<<chex[2][0];
    cout<<"H\nD="<<chex[3][1]<<chex[3][0]<<"H\nE="<<chex[4][1]<<chex[4][0]<<"H\nH="<<chex[5][1]<<chex[5][0]<<"H\nL="<<chex[6][1]<<chex[6][0]<<"H\n";
    cout<<"Flags Status:\n";
    cout<<"Zero Flag:"<<(int)flags[0]<<"\nCarry Flag:"<<(int)flags[1]<<"\nSign Flag:"<<(int)flags[2]<<"\nOverflow Flag:"<<(int)flags[3];
}

void goToLine(ifstream &x, int n)
{
    x.seekg(0);
    string line;
    for(int i=0; i<n-2;i++)
        getline(x,line);
}

int getRegVal(char reg)
{
    if(reg=='A')
        return (a);
    else if(reg=='B')
        return (b);
    else if(reg=='C')
        return (c);
    else if(reg=='D')
        return (d);
    else if(reg=='E')
        return (e);
    else if(reg=='H')
        return (h);
    else if(reg=='L')
        return (l);
    else if(reg=='M')
    {
        int arrh[2], arrl[2], t=0, x=3;
        decToHex(h,arrh);
        decToHex(l,arrl);
        while(x>=0)
        {
            if(x>1)
                t += arrh[x-2]*pow(16,x);
            else
                t += arrl[x]*pow(16,x);
            x--;
        }
        return(mem[t]);
    }
    else return(-1);
}

void putRegVal(char reg, int val)
{
    if(reg=='A')
        a=val;
    else if(reg=='B')
        b=val;
    else if(reg=='C')
        c=val;
    else if(reg=='D')
        d=val;
    else if(reg=='E')
        e=val;
    else if(reg=='H')
        h=val;
    else if(reg=='L')
        l=val;
    else if(reg=='M')
    {
        int arrh[2], arrl[2], t=0, x=3;
        decToHex(h,arrh);
        decToHex(l,arrl);
        while(x>=0)
        {
            if(x>1)
                t += arrh[x-2]*pow(16,x);
            else
                t += arrl[x]*pow(16,x);
            x--;
        }
       mem[t]=val;
       validmem[t]=val;
    }
}
