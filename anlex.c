
#include "anlex.h"

char cad[5*TAMLEX];
token t;
FILE *archivo;			// fuuente de pascal
char id[TAMLEX];		// variable a ser utilizada por el parser
int numLinea=1;			// lineas
char c=0;

void error( string mensaje)
{
	printf("\nLin %d: ERROR LEXICO. %s.\n",numLinea,mensaje);
		while(fgetc(archivo) != '\n'){
            fgetc(archivo);    	 
    }
    numLinea++;    	
}

void sigLex()
{
	int i=0;
	int estado=0;
    int acepto=0;
	char msg[5*TAMLEX];   
	while((c=fgetc(archivo))!=EOF){
		if (c==' ')
            continue;    
        else if(c=='\t')        
            continue;
		else if(c=='\n'){
			//incremento de la linea
			numLinea++;
		}
		else if (isalpha(c))
		{
			//Se debe recibir una palabra reservada
			i=0;
			do {
				id[i]=c;
				i++;
				c=fgetc(archivo);       
			}while(isalpha(c));
			id[i]='\0';
			if (c != EOF){
				ungetc(c,archivo);}
			else
			    c=0;
			palabra_reservada(id);
            t.lexema = id;
			break;
		}

        else if (c=='"'){
            //Se espera un tipo LITERAL_CADENA
            i=0;
            estado=0;
            acepto=0;
            id[i]=c;
            while(!acepto){
             	switch(estado){
             		case 0:

             			c=fgetc(archivo);
             			if(i>=TAMLEX){
             				estado = -1;
                    	}
                    	
             			else if(c=='\t' || c=='\n' || c==EOF){
             				estado = -1;
             			}
             			else if(c=='"'){
             				id[++i] = c;
             				estado = 1;
             			}
             			else{
             				id[++i]= c;
             				estado=0;
             			}
             			break;
             		case 1://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico						
						c=0;
						id[++i]='\0';
						acepto=1;
						t.compLex=LITERAL_CADENA;
		                t.lexema = id;
		                t.componente = nombres_comp[LITERAL_CADENA - 256]; 
						break;
					case -1:						
						if (c==EOF)
							error("No se esperaba el fin de archivo");
						else{
							ungetc(c,archivo);
							if(i>= TAMLEX) error("Longitud de Identificador excede tamaño de buffer");
							else if(c=='\n')error("Salto de linea no esperado");
							else if(c=='\t')error("Tabulacion no esperada");
							t.componente = 0;
		                    acepto=1;
		                    break;
		                }					
             	} 
            }
            break;
        }

		else if (isdigit(c))
		{
                
			//es un numero
			i=0;
			estado=0;
			acepto=0;
			id[i]=c;
			while(!acepto)
			{
				switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;
					
					case 1://un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
						c=fgetc(archivo);						
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							id[++i]=c;
		                    printf("%c = ",c);
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
							id[++i]='\0';
							acepto=1;
							t.compLex=LITERAL_NUM;
		                    t.lexema = id;
		                    t.componente = nombres_comp[LITERAL_NUM - 256]; 
						break;
					case -1:
						if (c==EOF)
							error("No se esperaba el fin de archivo");
						else{
							t.componente = 0;
		                    sprintf(msg,"%c no esperado",c);
			                error(msg);
		                    acepto=1;
		                }					
		                break;
		        }
			}
        	break;
		}

		else if (c==':')
		{
			t.compLex= DOS_PUNTOS;
            t.lexema = ":";			
            t.componente = nombres_comp[DOS_PUNTOS - 256];
			break;
		}
		else if (c==',')
		{
			t.compLex= COMA;
            t.lexema = "," ;
            t.componente = nombres_comp[COMA - 256];            
			break;
		}		
        else if (c=='[')
		{
			t.compLex=L_CORCHETE;
			t.lexema = "[";
            t.componente = nombres_comp[L_CORCHETE - 256];
			break;
		}
		else if (c==']')
		{
			t.compLex=R_CORCHETE;
			t.lexema = "]";
            t.componente = nombres_comp[R_CORCHETE - 256];
			break;
		}
		else if (c=='{')
		{
            t.compLex= L_LLAVE;
            t.lexema = "{";            
            t.componente = nombres_comp[L_LLAVE - 256];
			break;		
        }
        else if (c=='}')
        {
            t.compLex= R_LLAVE;
            t.lexema = "}";            
            t.componente = nombres_comp[R_LLAVE - 256];
			break;		
        }
		else if (c!=EOF)
		{
			sprintf(msg,"%c no esperado",c);
			error(msg);
		}
	}
	
    if (c==EOF)
	{
		t.compLex=EOF;
	}
	
}

void palabra_reservada(char id []){
   if (strcmp(id,"TRUE") == 0 || strcmp(id,"true") == 0 ) {
       t.compLex = PR_TRUE;
       t.lexema = id;
       t.componente = nombres_comp[PR_TRUE - 256]; 
   	}else if(strcmp(id,"FALSE") == 0 || strcmp(id,"false") == 0 ) {
       t.compLex = PR_FALSE;
       t.lexema = id;
       t.componente = nombres_comp[PR_FALSE - 256]; 
   	}else if(strcmp(id,"NULL") == 0 || strcmp(id,"null") == 0 ) {
       t.compLex = PR_NULL;
       t.lexema = id;
       t.componente = nombres_comp[PR_NULL - 256]; 
   	}
   	else{
   		ungetc(c,archivo);
   		sprintf(cad,"No se esperaba '%s'",id);
   		t.componente =0;
       	error(cad);
    }
}
int main(int argc,char* args[])
{
	// inicializar analizador lexico
	
	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		int cantTabs = 0;
		sigLex();
		int anterior = -1;
		while (t.compLex!=EOF){
			
			if(t.componente){
				if(t.compLex == L_LLAVE){
					if(anterior != DOS_PUNTOS){
						for(int i = 0; i < cantTabs; i++){
							printf("\t");
						}						
					}
					printf("%s\n", t.componente);
					anterior = t.compLex;
					cantTabs++;
				}
				else if(t.compLex == R_LLAVE){
					cantTabs--;
					printf("\n");
					for(int i = 0; i < cantTabs; i++){
						printf("\t");
					}
					printf("%s", t.componente);
					anterior = t.compLex;
				}
				else if(t.compLex == COMA){
					printf(" %s\n", t.componente);
					anterior = t.compLex;
				}
				else if(t.compLex == DOS_PUNTOS){
					printf(" %s ", t.componente);
					anterior = DOS_PUNTOS;
				}
				else if(t.compLex == LITERAL_CADENA) {
					if(anterior == DOS_PUNTOS){
						printf("%s", t.componente);	
					}
					else if(anterior == COMA){
						for(int i = 0; i < cantTabs; i++){
							printf("\t");
						}
						printf("%s", t.componente);
					}
					else if(anterior == R_LLAVE) {
						printf("\n");
						for(int i = 0; i < cantTabs; i++){
							printf("\t");
						}
						printf("%s", t.componente);
					}
					else if(anterior == L_LLAVE){
						for(int i = 0; i < cantTabs; i++){
							printf("\t");
						}
						printf("%s", t.componente);						
					}
					else if(anterior == R_CORCHETE) {
						printf("\n");
						for(int i = 0; i < cantTabs; i++){
							printf("\t");
						}
						printf("%s", t.componente);
					}
					else if(anterior == R_CORCHETE){
						for(int i = 0; i < cantTabs; i++){
							printf("\t");
						}
						printf("%s", t.componente);						
					}
					anterior = LITERAL_CADENA;
				}
				else if(t.compLex == L_CORCHETE){
					cantTabs++;
					printf("%s\n", t.componente);
					anterior = t.compLex;			
				}
				else if(t.compLex == R_CORCHETE){
					cantTabs--;
					printf("\n");
					for(int i = 0; i < cantTabs; i++){
						printf("\t");
					}
					printf("%s", t.componente);
					anterior = t.compLex;
				}
			}
			sigLex();
		}
		fclose(archivo);
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}

	return 0;
}
