#include "anlex.c"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define CANT_COMP_LEX 10

void json();
void element();
void array();
void arrayB();
void element_list_a();
void element_list_b();
void object_a();
void object_b();
void attribute();
void attributes_list();
void attributes_listB();
void attribute_name();
void attribute_value(int []);
void match(int);
void error_sint(int []);
int accept=1;

void match(int expectedToken){
    if(t.compLex == expectedToken){
        sigLex();
    }
  
}

void error_sint(int sincronizacion []){
    accept=0;
   	printf("\nError sintactico en linea: %d. No se esperaba %s.\n",numLinea,t.componente);
	int i = 0;
    while(t.compLex != sincronizacion[i] && t.compLex != EOF){   
        if (sincronizacion[i] == '\0'){
            sigLex();
            i=0;        
        }
        i++;
    }
    sigLex();
    return;
}

void check_input(int primero[], int siguiente[]){
    int syncset[CANT_COMP_LEX];
    int i=0;
    if(t.compLex == EOF) return;    
    while(primero[i] != '\0') 
    {
        if(t.compLex == primero[i])
        {
            return;
        }
        i++;
    }
    i=0;    
    while(primero[i] != '\0'){ 
        syncset[i] = primero[i];
        i++;
    }
    int j=0;
    while(siguiente[j] != '\0'){
        syncset[i] = siguiente[j];
        i++;
        j++;
    }
    error_sint(syncset);

}
 
void json(){
    int primero[] = {L_CORCHETE,L_LLAVE,'\0'};
    int siguiente[] = {COMA,R_CORCHETE,R_LLAVE, '\0'};
    element(siguiente);
}

void element(int syncset[]){

    int primero[] = {L_CORCHETE,L_LLAVE,'\0'};
    int siguiente[] = {COMA,R_CORCHETE,R_LLAVE, '\0'};
    check_input(primero,syncset);

    if(t.compLex == L_LLAVE){
        object_a(siguiente);
    }
    else if(t.compLex == L_CORCHETE){
        array(siguiente);
    }
    check_input(siguiente,primero);
}



void array(int syncset[]){
    int primero[] = {L_CORCHETE,'\0'};
    int siguiente[] = {COMA,R_CORCHETE,R_LLAVE, '\0'};
    check_input(primero,syncset);
   
    if(t.compLex == L_CORCHETE){
        match(L_CORCHETE);
        arrayB(siguiente);
    }
    check_input(siguiente,primero);
}


void arrayB(int syncset[]){   
    int primero[]={L_CORCHETE,R_CORCHETE,L_LLAVE,'\0'};
    int siguiente[] = {L_CORCHETE,COMA,R_CORCHETE,R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
        element_list_a(siguiente);
        match(R_CORCHETE);
    }
    else if(t.compLex == R_CORCHETE){
        match(R_CORCHETE);
    }
    check_input(siguiente,primero);

}


void element_list_a(int syncset[]){
    
    int primero[]={L_CORCHETE,L_LLAVE,'\0'};
    int siguiente[] = {R_CORCHETE, '\0'};
    check_input(primero,syncset);
    if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
        element(siguiente);
        element_list_b(siguiente);
    }
    check_input(siguiente,primero);
}



void element_list_b(int syncset[]){
    if(t.compLex == R_CORCHETE){ 
        return;
    }
    int primero[]={COMA,'\0'};
    int siguiente[] = {R_CORCHETE, '\0'};
    check_input(primero,syncset);
    if(t.compLex == COMA){
        match(COMA);
        element(siguiente);
        element_list_b(siguiente);
    }
    check_input(siguiente,primero);
}



void object_a(int syncset[]){
 
    int primero[]={L_LLAVE,'\0'};
    int siguiente[] = {COMA,R_CORCHETE,R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(t.compLex == L_LLAVE){
        match(L_LLAVE);
        object_b(siguiente);
    }
    check_input(siguiente,primero);

}


void object_b(int syncset[]){
  
    int primero[]={LITERAL_CADENA,R_LLAVE,'\0'};
    int siguiente[] = {LITERAL_CADENA,COMA,R_CORCHETE,R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(t.compLex == LITERAL_CADENA){
        attributes_list(siguiente);
        match(R_LLAVE);
    }
    else if(t.compLex == R_LLAVE){
        match(R_LLAVE);
    }
    check_input(siguiente,primero);
}


void attributes_list(int syncset[]){

    int primero[]={LITERAL_CADENA,'\0'};
    int siguiente[] = {R_LLAVE, '\0'};
    check_input(primero,syncset);

    if(t.compLex == LITERAL_CADENA){
        attribute(siguiente);
        attributes_listB(siguiente);
    }
    check_input(siguiente,primero);
}


void attributes_listB(int syncset[]){
 
    if (t.compLex == R_LLAVE){
        return;
    }
    int primero[]={COMA,'\0'};
    int siguiente[] = {R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(t.compLex == COMA){
        match(COMA);
        attribute(siguiente);
        attributes_listB(siguiente);
    }
    
    check_input(siguiente,primero);
}


void attribute(int syncset[]){

    int primero[]={LITERAL_CADENA,'\0'};
    int siguiente[] = {COMA,R_LLAVE, '\0'};
    check_input(primero,siguiente);
    if(t.compLex == LITERAL_CADENA){
        attribute_name(siguiente);
        match(DOS_PUNTOS);
        attribute_value(siguiente);
    }
    check_input(siguiente,primero);
}


void attribute_name(int syncset[]){
   
    int primero[]={LITERAL_CADENA,'\0'};
    int siguiente[] = {DOS_PUNTOS, '\0'};
    check_input(primero,syncset);
    if(t.compLex == LITERAL_CADENA){
       
        match(LITERAL_CADENA);
    }
    check_input(siguiente,primero);
}


void attribute_value(int syncset[]){

    int primero[]={L_CORCHETE,L_LLAVE,LITERAL_CADENA,LITERAL_NUM,PR_TRUE,PR_FALSE,PR_NULL,'\0'};
    int siguiente[] = {COMA,R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
        element(siguiente);
    }
    else if(t.compLex == LITERAL_CADENA){
        match(LITERAL_CADENA);
    }
    else if(t.compLex == LITERAL_NUM){
        match(LITERAL_NUM);
    }
    else if(t.compLex == PR_TRUE){
        match(PR_TRUE);
    }
    else if(t.compLex == PR_FALSE){
        match(PR_FALSE);
    }
    else if(t.compLex == PR_NULL){
        match(PR_NULL);
    }
    check_input(siguiente,primero);

}


/* int main (int argc,char* args[]){
    if(argc > 1){
        if (!(archivo=fopen(args[1],"rt"))){
            printf("El archivo no ha sido encontrado.\n");
            exit(1);
        }
        sigLex();
        json();
        if(accept) printf("Archivo correctamente sintactico \n");
        fclose(archivo);
    }
    else{
        printf("Debe pasar como parametro la ruta al archivo fuente.\n");
        exit(1);
    }

    return 0;
} */