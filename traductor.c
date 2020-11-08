#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "parser.c"
#define CANT_COMP_LEX 10

FILE *file_output;

void json_trad();
void element_trad();
void array_1_trad();
void array_2_trad();
void element_list_1_trad();
void element_list_2_trad();
void object_1_trad();
void object_2_trad();
void attribute_trad();
void attributes_list_1_trad();
void attributes_list_2_trad();
void attribute_name_trad();
void attribute_value_trad();
void match(int);
void string_sin_comillas(char* cadena);
int tabulacion=0;
void print_tab();

void string_sin_comillas(char* cadena){
    char string[TAMLEX];
    strcpy(string,cadena);
    int i=1;
    while(string[i] != '"'){
        printf("%c",string[i]);
        fprintf(file_output,"%c",string[i]); 
        i++;
    }
}

void print_tab(){
    int i=0;
    for (i=1;i<=tabulacion;i++){
        printf(" ");
        fprintf(file_output," "); 
    }
}

void json_trad(){
    element_trad();
}

void element_trad(){

    if(t.compLex == L_LLAVE){
        object_1_trad();
    }
    else if(t.compLex == L_CORCHETE){
        array_1_trad();
    }
    
}

void array_1_trad(){  
    if(t.compLex == L_CORCHETE){    
        match(L_CORCHETE);
        array_2_trad();
    	print_tab();
    }
}

void array_2_trad(){
    
    if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
        printf("\n");
        fprintf(file_output,"\n"); 
        print_tab();
        printf("<item>\n");
        fprintf(file_output,"<item>\n"); 
        tabulacion+=4;
        element_list_1_trad();
        match(R_CORCHETE);
        print_tab();
        printf("</item>\n");
        fprintf(file_output,"</item>\n"); 
	    tabulacion-=4;
    }
    else if(t.compLex == R_CORCHETE){
        tabulacion-=4;
        match(R_CORCHETE);
    }
}

void element_list_1_trad(){
    if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
        element_trad();
        element_list_2_trad();        
    }
}

void element_list_2_trad(){
    if(t.compLex == R_CORCHETE){ 
        return;
    }    
    if(t.compLex == COMA){  
        match(COMA);
        print_tab();
        printf("</item>\n");
        fprintf(file_output,"</item>\n"); 
        print_tab();
        printf("<item>\n");
        fprintf(file_output,"<item>\n"); 
        tabulacion+=4;
        element_trad();
        element_list_2_trad();        
    }    
}

void object_1_trad(){
    if(t.compLex == L_LLAVE){
        match(L_LLAVE);
        object_2_trad();
    }
}

void object_2_trad(){
    if(t.compLex == LITERAL_CADENA){
        attributes_list_1_trad();
        match(R_LLAVE);

    }
    else if(t.compLex == R_LLAVE){
        match(R_LLAVE);
    }
}

void attributes_list_1_trad(){
    if(t.compLex == LITERAL_CADENA){
        attribute_trad();
        attributes_list_2_trad();
    }
}

void attributes_list_2_trad(){
    if (t.compLex == R_LLAVE){
        tabulacion-=4;
        return;
    }
    if(t.compLex == COMA){
        match(COMA);
        attribute_trad();
        attributes_list_2_trad();
    }
}

void attribute_trad(){
    if(t.compLex == LITERAL_CADENA){
        print_tab();
        char lexema[TAMLEX];
        strcpy(lexema,t.lexema);
        printf("<");
        fprintf(file_output,"<"); 	
        attribute_name_trad();
        printf(">");
        fprintf(file_output,">"); 
        match(DOS_PUNTOS);
        attribute_value_trad();
        printf("</");
        fprintf(file_output,"</"); 
        string_sin_comillas(lexema);
        printf(">\n"); 
        fprintf(file_output,">\n");   
    }
}

void attribute_name_trad(){
    if(t.compLex == LITERAL_CADENA){    
        string_sin_comillas(t.lexema);
        match(LITERAL_CADENA);
    }
}

void attribute_value_trad(){
    if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
        tabulacion +=4;
        element_trad();
    }
    else if(t.compLex == LITERAL_CADENA){
        string_sin_comillas(t.lexema);
        match(LITERAL_CADENA);
    }
    else if(t.compLex == LITERAL_NUM){
        printf("%s",t.lexema);
        fprintf(file_output,"%s",t.lexema);
        match(LITERAL_NUM);
    }
    else if(t.compLex == PR_TRUE){
        printf("%s",t.lexema);
        fprintf(file_output,"%s",t.lexema);
        match(PR_TRUE);
    }
    else if(t.compLex == PR_FALSE){
        printf("%s",t.lexema);
        fprintf(file_output,"%s",t.lexema);
        match(PR_FALSE);
    }
    else if(t.compLex == PR_NULL){
        printf("%s",t.lexema);
        fprintf(file_output,"%s",t.lexema);
        match(PR_NULL);
    }
}

int main (int argc,char* args[]){
    if(argc > 1){
        if (!(archivo=fopen(args[1],"rt"))){
            printf("El archivo no existe\n");
            exit(1);
        }
        sigLex();
        json();
    	file_output=fopen("output.xml","a");
        if(accept){
            fclose(archivo);
            archivo=fopen(args[1],"rt");    
            sigLex();
            json_trad();
        }
    	else {	
    		fprintf(file_output,"\n Error sintáctico");		
	    }
    	fclose(file_output);
    }
    else{
        printf("ERROR: Se debe proveer la ruta al archivo fuente.\n");
        exit(1);
    }
    return 0;
}