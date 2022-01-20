#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
int main(){
    DIR* dirp;
    struct dirent *ptr;
    char dirpath[]="../../ArrayTest/";
    if((dirp=opendir(dirpath))==NULL){
        printf("invalid directory path.");
        exit(0);
    }
    while((ptr=readdir(dirp))!=NULL){
        char*p=strchr(ptr->d_name,'.');
        if(strcmp(p+1,"jack")==0){
            char filepath[100];
            strcpy(filepath,dirpath);
            strcat(filepath,ptr->d_name);
            FILE*fp;
            if((fp=fopen(filepath,"r"))==NULL){
                printf("invalid path.");
                exit(0);
            }
            char src[200][200];
            unsigned k=0,l=0;
            while(!feof(fp)){
                while(!feof(fp)&&(src[k][l++]=fgetc(fp))!='\n');
                src[k++][l-1]='\0';
                l=0;
            }
            unsigned kmax=k;
            fclose(fp);
            *p='\0';
            char wfilepath[100];
            strcpy(wfilepath,dirpath);
            strcat(wfilepath,ptr->d_name);
            strcat(wfilepath,"T_output.xml");
            if((fp=fopen(wfilepath,"w"))==NULL){
                printf("invalid wpath.");
                exit(0);
            }
            setbuf(fp,NULL);
            fprintf(fp,"<tokens>\n");
            k=0;l=0;
            while(k<kmax){
                //skip null chars
                if(src[k][l]=='\n'||src[k][l]=='\0'){
                    k++;l=0;
                    continue;
                }
                if(src[k][l]=='\t'||src[k][l]==' '){
                    l++;
                    continue;
                }
                //skip comments
                if(src[k][l]=='/'&&src[k][l+1]=='/'){
                    k++;l=0;
                    continue;
                }
                if(src[k][l]=='/'&&src[k][l+1]=='*'&&src[k][l+2]=='*'){
                    l+=3;
                    while(true){
                        if(src[k][l]=='\n'||src[k][l]=='\0'){
                            k++;l=0;
                            continue;
                        }
                        if(src[k][l]=='*'&&src[k][l+1]=='/'){
                            l+=2;
                            break;
                        }
                        l++;
                    }
                    continue;
                }
                //symbol
                if(src[k][l]=='{'||src[k][l]=='}'||src[k][l]=='('||src[k][l]==')'||src[k][l]=='['||
                    src[k][l]==']'||src[k][l]=='.'||src[k][l]==','||src[k][l]==';'||src[k][l]=='+'||
                    src[k][l]=='-'||src[k][l]=='*'||src[k][l]=='/'||src[k][l]=='&'||src[k][l]=='|'||
                    src[k][l]=='<'||src[k][l]=='>'||src[k][l]=='='||src[k][l]=='~'){
                    fprintf(fp,"<symbol> %c </symbol>\n",src[k][l]);
                    l++;
                    continue;
                }
                //integer constant
                if(src[k][l]>='0'&&src[k][l]<='9'){
                    unsigned intcst=0;
                    do{
                        intcst=intcst*10+src[k][l]-'0';
                        l++;
                    }while(src[k][l]>='0'&&src[k][l]<='9');
                    fprintf(fp,"<integerConstant> %d </integerConstant>\n",intcst);
                    continue;
                }
                //string constant
                if(src[k][l]=='"'){
                    char str[100];
                    unsigned strk=0;
                    do{
                        str[strk++]=src[k][++l];
                    }while(src[k][l]!='"');
                    l++;
                    str[--strk]='\0';
                    fprintf(fp,"<stringConstant> %s </stringConstant>\n",str);
                    continue;
                }
                //keyword or identifier
                if((src[k][l]>='a'&&src[k][l]<='z')||(src[k][l]>='A'&&src[k][l]<='Z')){
                    char id[100];
                    unsigned idk=0;
                    do{
                        id[idk++]=src[k][l++];
                    }while(src[k][l]=='_'||(src[k][l]>='a'&&src[k][l]<='z')||(src[k][l]>='A'&&src[k][l]<='Z'));
                    id[idk]='\0';
                    if(strcmp(id,"class")==0||strcmp(id,"constructor")==0||strcmp(id,"function")==0||strcmp(id,"method")==0||
                        strcmp(id,"field")==0||strcmp(id,"static")==0||strcmp(id,"var")==0||strcmp(id,"int")==0||
                        strcmp(id,"char")==0||strcmp(id,"boolean")==0||strcmp(id,"void")==0||strcmp(id,"true")==0||
                        strcmp(id,"false")==0||strcmp(id,"null")==0||strcmp(id,"this")==0||strcmp(id,"let")==0||
                        strcmp(id,"do")==0||strcmp(id,"if")==0||strcmp(id,"else")==0||strcmp(id,"while")==0||
                        strcmp(id,"return")==0){
                        fprintf(fp,"<keyword> %s </keyword>\n",id);
                    }
                    else{
                        fprintf(fp,"<identifier> %s </identifier>\n",id);
                    }
                    continue;
                }
                printf("impossible to reach here!");
            }
            fprintf(fp,"</tokens>\n");
            fclose(fp);
            setbuf(fp,NULL);
            FILE *orifp;
            char orifilepath[100];
            strcpy(orifilepath,dirpath);
            strcat(orifilepath,ptr->d_name);
            strcat(orifilepath,"T.xml");
            if((orifp=fopen(orifilepath,"r"))==NULL){
                printf("invalid filepath");
                exit(0);
            }
            if((fp=fopen(wfilepath,"r"))==NULL){
                printf("invalid filepath");
                exit(0);
            }
            char orifile[20000];
            char wfile[20000];
            k=0;
            while(!feof(fp)) wfile[k++]=fgetc(fp);
            wfile[k]='\0';
            k=0;
            while(!feof(orifp)) orifile[k++]=fgetc(orifp);
            orifile[k]='\0';

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
        else continue;
    }
    system("pause");
    return 0;
}