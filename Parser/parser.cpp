#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

void compose_class();
void compose_classVarDec();
void compose_type();
void compose_subroutineDec();
void compose_parameterList();
void compose_subroutineBody();
void compose_varDec();
void compose_className();
void compose_subroutineName();
void compose_varName();

void compose_statements();
void compose_statement();
void compose_letStatement();
void compose_ifStatement();
void compose_whileStatement();
void compose_doStatement();
void compose_returnStatement();

void compose_expression();
void compose_term();
void compose_subroutineCall();
void compose_expressionList();
void compose_op();
void compose_unaryOp();
void compose_KeywordConstant();

void compose_integerConstant();
void compose_StringConstant();
void compose_identifier();

void eat();
unsigned peek(const char*);
unsigned peek_kind();//0 kw;1 symbol;2 intC;3 StrC;4 ident
void initialize();
FILE*wfp;
char src[900][100];
unsigned indent=0;
unsigned readline=1;
int main(){
    DIR*dirp;
    dirent*ptr;
    char dirpath[]="../../ExpressionLessSquare/";
    if((dirp=opendir(dirpath))==NULL){
        printf("invalid directory:%s",dirpath);
        exit(-1);
    }
    while(ptr=readdir(dirp)){
        char*pc=strchr(ptr->d_name,'_');
        if(pc){
            initialize();
            char filepath[100];
            strcpy(filepath,dirpath);
            strcat(filepath,ptr->d_name);
            FILE*fp;
            if((fp=fopen(filepath,"r"))==NULL){
                printf("invalid path:%s",filepath);
                exit(-1);
            }
            else{
                printf("open filepath:%s\n",filepath);
            }
            char wfilepath[100];
            strcpy(wfilepath,dirpath);
            *pc='@';
            pc=strchr(ptr->d_name,'T');
            *pc='F';
            strcat(wfilepath,ptr->d_name);
            if((wfp=fopen(wfilepath,"w"))==NULL){
                printf("invalid path:%s",wfilepath);
                exit(-1);
            }
            unsigned k=0,l=0;
            while(!feof(fp)){
                char ch;
                do{ 
                    ch=fgetc(fp);
                    src[k][l++]=ch;
                }while(ch!='\n'&&ch!=-1);
                src[k][--l]='\0';
                k++;l=0;
            }
            fprintf(wfp,"<class>\n");
            indent+=2;
            compose_class();
            indent-=2;
            fprintf(wfp,"</class>\n");
            fclose(fp);
            fclose(wfp);
            //compare
            *pc='\0';
            strcpy(filepath,dirpath);
            strcat(filepath,ptr->d_name);
            strcat(filepath,".xml");
            if((fp=fopen(filepath,"r"))==NULL){
                printf("invalid filepath:%s",filepath);
                exit(-1);
            }
            if((wfp=fopen(wfilepath,"r"))==NULL){
                printf("invalid filepath:%s",wfilepath);
                exit(-1);
            }
            char orifile[50000];
            char wfile[50000];
            k=0;
            while(!feof(wfp)) wfile[k++]=fgetc(wfp);
            wfile[k]='\0';
            k=0;
            while(!feof(fp)) orifile[k++]=fgetc(fp);
            orifile[k]='\0';
            fclose(wfp);
            fclose(fp);
            k=0;
            char*wp=strchr(wfile,'\n');
            char*op=strchr(orifile,'\n');
            *wp='\0';
            *op='\0';
            char*wpn=wp+1;
            char*opn=op+1;
            int cprs=strcmp(wfile,orifile);
            if(cprs!=0) printf("line %d: %s || %s\n",k,wfile,orifile);
            while(*wpn!=-1){
                k++;
                wp=strchr(wpn,'\n');
                op=strchr(opn,'\n');
                *wp='\0';
                *op='\0';
                int cprs=strcmp(wpn,opn);
                if(cprs!=0) printf("line %d: %s || %s\n",k,wpn,opn);
                wpn=wp+1;opn=op+1;
            }
        }
    }
    system("pause");
    return 0;
}
void initialize(){
    for(int i=0;i<900;i++) src[i][0]='\0';
    readline=1;
}
void compose_class(){
    eat();//'class'
    compose_className();
    eat();//'{'
    unsigned peekrs;
    while((peekrs=peek("static field"))==1){
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<classVarDec>\n");
        indent+=2;
        compose_classVarDec();
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</classVarDec>\n");
    }
    while((peekrs=peek("constructor function method"))==1){
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<subroutineDec>\n");
        indent+=2;
        compose_subroutineDec();
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</subroutineDec>\n");
    }
    eat();//'}'
}
void compose_classVarDec(){
    eat();//('static'|'field')
    compose_type();
    compose_varName();
    unsigned peekrs;
    while((peekrs=peek(","))==1){
        eat();//','
        compose_varName();
    }
    eat();//';'
}
void compose_type(){
    eat();//'int'|'char'|'boolean'|className
}
void compose_subroutineDec(){
    eat();
    eat();
    compose_subroutineName();
    eat();
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"<parameterList>\n");
    indent+=2;
    compose_parameterList();
    indent-=2;
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"</parameterList>\n");
    eat();
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"<subroutineBody>\n");
    indent+=2;
    compose_subroutineBody();
    indent-=2;
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"</subroutineBody>\n");
}
void compose_parameterList(){
    unsigned peekrs,peek_k;
    if((peekrs=peek("int char boolean"))==1||(peek_k=peek_kind())==4){
        eat();
        compose_varName();
    }
    while((peekrs=peek(","))==1){
        eat();
        eat();
        eat();
    }
}
void compose_subroutineBody(){
    eat();
    unsigned peekrs;
    while((peekrs=peek("var"))==1){
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<varDec>\n");
        indent+=2;
        compose_varDec();
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</varDec>\n");
    }
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"<statements>\n");
    indent+=2;
    compose_statements();
    indent-=2;
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"</statements>\n");
    eat();
}
void compose_varDec(){
    eat();
    eat();
    compose_varName();
    unsigned peekrs;
    while((peekrs=peek(","))==1){
        eat();
        eat();
    }
    eat();
}
void compose_className(){
    eat();
}
void compose_subroutineName(){
    eat();
}
void compose_varName(){
    eat();
}

void compose_statements(){
    unsigned peekrs;
    while((peekrs=peek("let if while do return"))==1){
        compose_statement();
    }
}
void compose_statement(){
    unsigned peekrs;
    if((peekrs=peek("let"))==1){
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<letStatement>\n");
        indent+=2;
        compose_letStatement();
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</letStatement>\n");
    }
    else if((peekrs=peek("if"))==1){
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<ifStatement>\n");
        indent+=2;
        compose_ifStatement();
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</ifStatement>\n");
    }
    else if((peekrs=peek("while"))==1){
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<whileStatement>\n");
        indent+=2;
        compose_whileStatement();
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</whileStatement>\n");
    }
    else if((peekrs=peek("do"))==1){
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<doStatement>\n");
        indent+=2;
        compose_doStatement();
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</doStatement>\n");
    }
    else{
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<returnStatement>\n");
        indent+=2;
        compose_returnStatement();
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</returnStatement>\n");
    }
}
void compose_letStatement(){
    eat();
    compose_varName();
    unsigned peekrs;
    if((peekrs=peek("["))==1){
        eat();
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<expression>\n");
        indent+=2;
        compose_expression();
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</expression>\n");
        eat();
    }
    eat();
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"<expression>\n");
    indent+=2;
    compose_expression();
    indent-=2;
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"</expression>\n");
    eat();
}
void compose_ifStatement(){
    eat();//'if'
    eat();//'('
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"<expression>\n");
    indent+=2;
    compose_expression();
    indent-=2;
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"</expression>\n");
    eat();//')'
    eat();//'{'
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"<statements>\n");
    indent+=2;
    compose_statements();
    indent-=2;
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"</statements>\n");
    eat();//'}'
    unsigned peekrs;
    if((peekrs=peek("else"))==1){
        eat();
        eat();
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<statements>\n");
        indent+=2;
        compose_statements();
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</statements>\n");
        eat();
    }
}
void compose_whileStatement(){
    eat();
    eat();
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"<expression>\n");
    indent+=2;
    compose_expression();
    indent-=2;
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"</expression>\n");
    eat();
    eat();
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"<statements>\n");
    indent+=2;
    compose_statements();
    indent-=2;
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"</statements>\n");
    eat();
}
void compose_doStatement(){
    eat();
    /*
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"<subroutineCall>\n");
    indent+=2;
    */
    compose_subroutineCall();
    /*
    indent-=2;
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"</subroutineCall>\n");
    */
    eat();
}
void compose_returnStatement(){
    eat();
    unsigned peekrs;
    if((peekrs=peek(";"))==0){
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<expression>\n");
        indent+=2;
        compose_expression();
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</expression>\n");
    }
    eat();
}

void compose_expression(){
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"<term>\n");
    indent+=2;
    compose_term();
    indent-=2;
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"</term>\n");
    unsigned peekrs;
    while((peekrs=peek("+ - * / & | < > ="))==1){
        eat();
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<term>\n");
        indent+=2;
        compose_term();
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</term>\n");
    }
}
void compose_term(){
    unsigned peek_k,peekrs;
    peek_k=peek_kind();
    if(peek_k==2||peek_k==3){
        eat();
        return;
    }
    else if(peek_k==4){
        readline++;
        peekrs=peek("[");
        readline--;
        if(peekrs==1){
            eat();
            eat();
            for(int i=0;i<indent;i++) fprintf(wfp," ");
            fprintf(wfp,"<expression>\n");
            indent+=2;
            compose_expression();
            indent-=2;
            for(int i=0;i<indent;i++) fprintf(wfp," ");
            fprintf(wfp,"</expression>\n");
            eat();
            return;
        }
        readline++;
        peekrs=peek(". (");
        readline--;
        if(peekrs==1){
            /*
            for(int i=0;i<indent;i++) fprintf(wfp," ");
            fprintf(wfp,"<subroutineCall>\n");
            indent+=2;
            */
            compose_subroutineCall();
            /*
            indent-=2;
            for(int i=0;i<indent;i++) fprintf(wfp," ");
            fprintf(wfp,"</subroutineCall>\n");
            */
            return;
        }
        eat();
        return;
    }
    if((peekrs=peek("true false null this"))==1){
        eat();
        return;
    }
    if((peekrs=peek("("))==1){
        eat();
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<expression>\n");
        indent+=2;
        compose_expression();
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</expression>\n");
        eat();
        return;
    }
    if((peekrs=peek("- ~"))==1){
        eat();
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<term>\n");
        indent+=2;
        compose_term();
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</term>\n");
        return;
    }
    printf("control never reach here!");
    return;
}
void compose_subroutineCall(){
    eat();
    unsigned peekrs=peek("(");
    if(peekrs==1){
        eat();
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<expressionList>\n");
        indent+=2;
        if((peekrs=peek(")"))!=1){
            compose_expressionList();
        }
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</expressionList>\n");
        eat();
        return;
    }
    else{
        eat();
        eat();
        eat();
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<expressionList>\n");
        indent+=2;
        if((peekrs=peek(")"))!=1){
            compose_expressionList();
        }
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</expressionList>\n");
        eat();
        return;
    }
}
void compose_expressionList(){
    
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"<expression>\n");
    indent+=2;
    compose_expression();
    indent-=2;
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"</expression>\n");
    unsigned peekrs;
    while((peekrs=peek(","))==1){
        eat();
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"<expression>\n");
        indent+=2;
        compose_expression();
        indent-=2;
        for(int i=0;i<indent;i++) fprintf(wfp," ");
        fprintf(wfp,"</expression>\n");
    }

}


void eat(){
    for(int i=0;i<indent;i++) fprintf(wfp," ");
    fprintf(wfp,"%s\n",src[readline]);
    readline++;
}
unsigned peek(const char* str){

    char cur[100];
    unsigned k=0,l=0;
    while(src[readline][k]!='>') k++;
    k+=2;
    while(src[readline][k]!=' ') cur[l++]=src[readline][k++];
    cur[l]='\0';

    char strc[100];
    strcpy(strc,str);
    char*pc,*pstr;
    pstr=strc;
    while((pc=strchr(pstr,' '))!=NULL){
        *pc='\0';
        if(strcmp(cur,pstr)==0){
            return 1;
        }
        pstr=pc+1;
    }
    if(strcmp(cur,pstr)==0){
        return 1;
    }
    return 0;
}
unsigned peek_kind(){//0 kw;1 symbol;2 intC;3 StrC;4 ident
    char str[100];
    unsigned k=1,l=0;
    while(src[readline][k]!='>') str[l++]=src[readline][k++];
    str[l]='\0';
    if(strcmp(str,"keyword")==0) return 0;
    else if(strcmp(str,"symbol")==0) return 1;
    else if(strcmp(str,"integerConstant")==0) return 2;
    else if(strcmp(str,"StringConstant")==0) return 3;
    else return 4;
}