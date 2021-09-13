/*
 *	Analizador Léxico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Práctica de Programación Nro. 1
 *	
 *	Descripcion:
 *	Implementa un analizador léxico que reconoce números, identificadores, 
 * 	palabras reservadas, operadores y signos de puntuación para un lenguaje
 * 	con sintaxis tipo Pascal.
 *	
 */

/*********** Inclusión de cabecera **************/
#include "anlex.h"


/************* Variables globales **************/

int consumir;			/* 1 indica al analizador lexico que debe devolver
						el sgte componente lexico, 0 debe devolver el actual */

char cad[5*TAMLEX];		// string utilizado para cargar mensajes de error
token t;				// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico

FILE *archivo;			
FILE * fichero;  		//Archivo de Salida

char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
char id[TAMLEX];		// Utilizado por el analizador lexico
int delantero=-1;		// Utilizado por el analizador lexico
int fin=0;				// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea
int x;
char addEspacio[500];
/**************** Funciones **********************/


// Rutinas del analizador lexico

void error(const char* mensaje)
{
	fprintf(fichero,"Lin %d: Error Lexico. %s.\n",numLinea,mensaje);
	printf("Lin %d: Error Lexico. %s.\n",numLinea,mensaje);	
}

void sigLex(){
	int cierre=0; //0 = Inicio; 1 = Cierre 
	x=0;
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	entrada e;
	
	char palabra_reservada[10];// null,true,false
	while((c=fgetc(archivo))!=EOF){
		
		if (c==' '){
			while (c== ' '){//Copiamos todos los espacios en blanco
				
				addEspacio[x]= ' ';
				x++;
				c=fgetc(archivo);
			}
			c= ungetc(c,archivo);
		}else if (c=='\t'){//Copiamos las tabulaciones 
			while (c=='\t'){
				fprintf(fichero,"%c",c);
				c=fgetc(archivo);
			}
		}
		else if(c=='\n'){
			//incrementar el numero de linea
			numLinea++;
			fprintf(fichero,"\n");
			x=0;
			continue;
		}
		else if(c=='\"'){
			i=0;
			id[i]=c;
			i++;
			while(isascii(c) || cierre == 0 ){
				//Leemos hasta encontrar el \" de cierre 
				c=fgetc(archivo);
				if (c=='\"'){ //forzamos la salida
					id[i]=c;
					i++;
					cierre=1;
					break;
				}else if(c==':' || c=='\n' || c==EOF ){
					sprintf(msg,"No se esperaba '%c'",c);
                    fprintf(fichero," %s","Se esperaba que finalize el literal");
					error(msg);
					while (c != '\n')
					{
						c=fgetc(archivo);
					}
					ungetc(c,archivo);
					x=0;
					break;

				}else{
					id[i]=c;
					i++;
				}
			}
			id[i]='\0';
			t.pe=buscar(id);
			t.compLex=t.pe->compLex;
	        if (t.pe->compLex==-1)
			{
				strcpy(e.lexema,id);
			    e.compLex= LITERAL_CADENA;
	            insertar(e);
				t.pe=buscar(id);
        	    t.compLex=LITERAL_CADENA;
			}
			break;
		}
		else if (isdigit(c)){
		
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
						/*else if(c=='.')
						{
							i--;
							fseek(archivo,-1,SEEK_CUR);
							estado=6;
						}*/
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
						t.pe=buscar(id);
						if (t.pe->compLex==-1)
						{
							strcpy(e.lexema,id);
							e.compLex=LITERAL_NUM;
							insertar(e);
							t.pe=buscar(id);
						}
						t.compLex=LITERAL_NUM;
						break;
					case -1:
						if (c==EOF)
							error("No se esperaba el fin de archivo");
						else
							error(msg);
						//exit(1);
						acepto=1;
						t.compLex=1;
						while(c!='\n')
							c=fgetc(archivo);
						ungetc(c,archivo);
						break;
					}
				}
			break;
		}
		else if (c==':')
		{
			//puede ser un : o un operador de asignacion
			t.compLex=DOS_PUNTOS;
			t.pe=buscar(":");
			break;
		}
		else if (c==',')
		{
			t.compLex=COMA;
			t.pe=buscar(",");
			break;
		}
		else if (c=='[')
		{
			t.compLex=L_CORCHETE;
			t.pe=buscar("[");
			break;
		}
		else if (c==']')
		{
			t.compLex=R_CORCHETE;
			t.pe=buscar("]");
			break;
		}
		else if(isalpha(c)){
			//Verificamos que se trate de una palabra reservada
			//Como: true,false,null
			i=0;
			while (isalpha(c)){	
				id[i]=c;
				i++;
				c= fgetc(archivo);
			}
			id[i]='\0'; // Agregamos el clausula de fin
			sprintf( palabra_reservada ,"%s",id); //carga la palabra la variable {0}
			//if(strcmp(tolower(palabra_reservada),"null")==0 || strcmp(tolower(palabra_reservada),"false")==0 || strcmp(tolower(palabra_reservada),"true")==0 ){
			if(strcmp(palabra_reservada,"null")==0 || strcmp(palabra_reservada,"false")==0 || strcmp(palabra_reservada,"true")==0 ){
				t.pe=buscar(id);
			    t.compLex=t.pe->compLex;
			} else {
				while(c!='\n'){
			        c=fgetc(archivo);
		        }
			    error(palabra_reservada);
			    continue;
			}
			if (c!=EOF){
				ungetc(c,archivo);
			}else{
				c=0;
			}
			break;
		} else if (c=='{'){

			t.compLex=L_LLAVE;
			t.pe=buscar("{");
			break;
		} else if (c=='}'){

			t.compLex=R_LLAVE;
			t.pe=buscar("}");
			break;
		} else if (c!=EOF){

			sprintf(msg,"%c no esperado",c);
			error(msg);
			t.compLex=1;
			while(c!='\n'){
				c=fgetc(archivo);
			}
			strcpy(addEspacio, " ");
			x=0;
			ungetc(c,archivo);
		}
	}
	if (c==EOF)
	{
		t.compLex=EOF;
		// strcpy(e.lexema,"EOF");
		sprintf(e.lexema,"EOF");
		t.pe=&e;
	}
	
}


typedef struct {
  int codigo;
  const char* descripcion;
//} nombreString;
} nombres;

nombres nombreString[] = {
  {L_CORCHETE, "L_CORCHETE" },
  {R_CORCHETE, "R_CORCHETE"},
  {L_LLAVE,    "L_LLAVE" },
  {R_LLAVE,    "R_LLAVE"},

 // {L_CORCHETE, "L_CORCHETE" },
  {COMA, 	   			"COMA"},
  {DOS_PUNTOS, 			"DOS_PUNTOS" },
  {LITERAL_CADENA,	   "LITERAL_CADENA"},
  
  {LITERAL_NUM,     "LITERAL_NUM" },
  {PR_TRUE,    		"PR_TRUE"},
  {PR_FALSE,   		"PR_FALSE" }
};
void imprimir(int valor){
	int i;
	char encontrado='n';
//	for(i=0;nombreString[i] !=NULL; i++){
	//printf("%d \t", valor);
	for(i=0;i<11 ; i++){
		if(nombreString[i].codigo==valor){//EncontrÃ³ el valor del index
			encontrado='s';
			break;
		}
	}
	if (encontrado=='s'){
		fprintf(fichero,"%s ",nombreString[i].descripcion);
	}else{
	
	}
}

int main(int argc,char* args[])
{
	fichero = fopen("output.txt" , "w"); //Abrimos el archivo de salida

	// inicializar analizador lexico

	initTabla();
	initTablaSimbolos();
	
	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		while (t.compLex!=EOF){
			sigLex();
		
			if(x>=0){
				int j;
				for (j = 0; j < x; j++){
						fprintf(fichero,"%c",addEspacio[j]);
				}
			}
			imprimir(t.compLex);
				
		}
		fclose(archivo);
		fclose(fichero);
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}

	return 0;
}
