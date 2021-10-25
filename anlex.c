/*
 *	Analizador Lexico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Tarea 2
 *	Alumno: Alexis Gonzalez
 */

#include "anlex.h"

int consumir;			/* 1 indica al analizador lexico que debe devolver
				el siguiente componente lexico, 0 debe devolver el actual */

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

int valido = 1;
/**************** Funciones **********************/

// Rutinas del analizador lexico

void error(const char* mensaje)
{
	fprintf(fichero,"\nLin %d: Error Lexico. %s.\n",numLinea,mensaje);
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
				t.comp="LITERAL_CADENA";
				t.tipo_lexema = "LITERAL_CADENA";
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
							t.comp="LITERAL_NUM";
							t.tipo_lexema = "LITERAL_NUM";
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
			t.comp="DOS_PUNTOS";
			t.tipo_lexema=":";
			t.pe=buscar(":");
			break;
		}
		else if (c==',')
		{
			t.compLex=COMA;
			t.comp="COMA";
			t.tipo_lexema=",";
			t.pe=buscar(",");
			break;
		}
		else if (c=='[')
		{
			t.compLex=L_CORCHETE;
			t.comp="L_CORCHETE";
			t.tipo_lexema="[";
			t.pe=buscar("[");
			break;
		}
		else if (c==']')
		{
			t.compLex=R_CORCHETE;
			t.comp="R_CORCHETE";
			t.tipo_lexema="]";
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
				if ( strcmp(palabra_reservada,"false")==0)
				{
					
					t.compLex=PR_NULL;
					t.comp="PR_NULL";
					t.tipo_lexema = "PR_NULL";

				} else if (strcmp(palabra_reservada,"false")==0)
				{
					
					t.compLex=PR_FALSE;
					t.comp="PR_FALSE";
					t.tipo_lexema = "PR_FALSE";
				}
				
				{
					t.compLex=PR_TRUE;
					t.comp="PR_TRUE";
					t.tipo_lexema = "PR_TRUE";
				}
				
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
			t.comp="L_LLAVE";
			t.tipo_lexema="{";
			t.pe=buscar("{");
			break;
		} else if (c=='}'){

			t.compLex=R_LLAVE;
			t.comp="R_LLAVE";
			t.tipo_lexema="}";
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
		
		t.comp="EOF";
		t.tipo_lexema = "EOF";
		sprintf(e.lexema,"EOF");
		t.pe=&e;
	}
	
}

/*
	Seccion del analisador sintactico
*/
void imprimir_error(){
    valido = 0;
    printf("\nEn la Linea %d: se registro un Error Sintactico. No se esperaba %s. ", numLinea, t.comp);
}

void machear (int actual) {
    if (t.compLex = actual) {
    	sigLex();
    }
}

int encontrar_token(int vector[], int N, int comp){
    int i = 0;
    while (i < N) {
        if (vector[i] == comp) {
            return 1;
        }
        i++;
    }
    return 0;
}

void buscar_token(int sincro[], int N) {
    while (encontrar_token(sincro, N, t.compLex) == 0 && t.compLex != EOF) {
    	sigLex();
    }
    sigLex();
}


void unir_arrays(int primero[], int siguiente[], int N1, int N2) {
    int nuevo[N1+N2];
    int i = 0, j = 0;
    while (i < N1) {
        if (t.compLex == primero[i]) { return;}
        i++;
    }
    i = 0;
    while (i < N1) {
        nuevo[i] = primero[i];
        i++;
    }
    while (j < N2) {
        nuevo[i] = siguiente[j];
        i++;
        j++;
    }
    imprimir_error();
    buscar_token(nuevo, N1+N2);
}



void initJson(){
    int primero[2] = {L_CORCHETE, L_LLAVE};
    int siguiente[1] = {EOF};
    unir_arrays(primero, siguiente, 2, 1);//Inicializamos 
    json_elemento(siguiente, 1);
}


void json_elemento(int sincro[], int N){
    int primero[2] = {L_CORCHETE, L_LLAVE};
    int siguiente[4] = {COMA, R_CORCHETE, R_LLAVE, EOF};
    int i = 0, is = 0;
    unir_arrays(primero, sincro, 2, N);//Agregamos el siguiente componente
    while (i < N) {
        if (t.compLex == sincro[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {//Si es un objeto
        if (t.compLex == L_LLAVE) {
            json_objectos(siguiente, 4);
        }
        else if (t.compLex == L_CORCHETE) { // si es un array
            json_array(siguiente, 4);
        }
    }
    else {
        imprimir_error();
    }
    unir_arrays(siguiente, primero, 4, 2);
}




void json_name(int sincro[], int N){
    int primero[1] = {LITERAL_CADENA};
    int siguiente[1] = {DOS_PUNTOS};
    int i = 0, is = 0;
    unir_arrays(primero, sincro, 1, N);
    while (i < N) {
        if (t.compLex == sincro[i]) {
            is = 1;
        }
        i++;
}
    if (is == 0) {
        if (t.compLex == LITERAL_CADENA){
            machear(LITERAL_CADENA);
        }
    }
    else {
        imprimir_error();
    }
    unir_arrays(siguiente, primero, 1, 1);
}


void json_value(int sincro[], int N){
    int primero[7] = {L_LLAVE, L_CORCHETE, LITERAL_CADENA, LITERAL_NUM, PR_TRUE, PR_FALSE, PR_NULL};
    int siguiente[2] = {COMA, R_LLAVE};
    int i = 0, is = 0;
    unir_arrays(primero, sincro, 7, N);
    while (i < N) {
        if (t.compLex == sincro[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLex == L_LLAVE || t.compLex == L_CORCHETE) {
            json_elemento(siguiente, 2);
        }
        else if (t.compLex == LITERAL_CADENA) {
            machear(LITERAL_CADENA);
        }
        else if (t.compLex == LITERAL_NUM) {
            machear(LITERAL_NUM);
        }
        else if (t.compLex == PR_TRUE) {
            machear(PR_TRUE);
        }
        else if (t.compLex == PR_FALSE) {
            machear(PR_FALSE);
        }
        else if (t.compLex == PR_NULL) {
            machear(PR_NULL);
        }
}
    else {
        imprimir_error();
    }
    unir_arrays(siguiente, primero, 2, 7);
}


void json_sub_objeto(int sincro[], int N){
    int primero[2] = {R_LLAVE, LITERAL_CADENA};
    int siguiente[4] = {COMA, R_CORCHETE, R_LLAVE, EOF};
    int i = 0, is = 0;
    unir_arrays(primero, sincro, 2, N);
    while (i < N) {
        if (t.compLex == sincro[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLex == R_LLAVE) {
            machear(R_LLAVE);
        }
        else if (t.compLex == LITERAL_CADENA) {
            json_texto(siguiente, 4);
            machear(R_LLAVE);
        }
    }
    else {
        imprimir_error();
    }
    unir_arrays(siguiente, primero, 4, 2);
}


void json_objectos(int sincro[], int N){
    int primero[1] = {L_LLAVE};
    int siguiente[4] = {COMA, R_CORCHETE, R_LLAVE, EOF};
    int i = 0, is = 0;
    unir_arrays(primero, sincro, 1, N);
    while (i < N) {
        if (t.compLex == sincro[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLex == L_LLAVE) {
            machear(L_LLAVE);
            json_sub_objeto(siguiente, 4);
        }
    }
    else {
        imprimir_error();
    }
    unir_arrays(siguiente, primero, 4, 1);
}


void json_array(int sincro[], int N){
    int primero[1] = {L_CORCHETE};
    int siguiente[4] = {COMA, R_CORCHETE, R_LLAVE, EOF};
    int i = 0, is = 0;
    unir_arrays(primero, sincro, 1, N);
    while (i < N) {
        if (t.compLex == sincro[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLex == L_CORCHETE) {
            machear(L_CORCHETE);
            json_arr(siguiente, 4);
        }
    }
    else {
        imprimir_error();
    }
    unir_arrays(siguiente, primero, 4, 1);
}


void json_arr(int sincro[], int N){
    int primero[3] = {R_CORCHETE, L_CORCHETE, L_LLAVE};
    int siguiente[4] = {COMA, R_CORCHETE, R_LLAVE, EOF};
    unir_arrays(primero, sincro, 3, N);
    if (t.compLex == R_CORCHETE) {
        machear(R_CORCHETE);
    }
    else if (t.compLex == L_LLAVE || t.compLex == L_CORCHETE) {
        json_tipo_element(siguiente, 4);
        machear(R_CORCHETE);
    }
    unir_arrays(siguiente, primero, 4, 3);
}


void json_texto(int sincro[], int N){
    int primero[1] = {LITERAL_CADENA};
    int siguiente[1] = {R_LLAVE};
    int i = 0, is = 0;
    unir_arrays(primero, sincro, 1, N);
    while (i < N) {
        if (t.compLex == sincro[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLex == LITERAL_CADENA) {
            json_tipo_attribute(siguiente, 1);
                    json_atributo(siguiente, 1);
        }
    }
    else {
        imprimir_error();
    }
    unir_arrays(siguiente, primero, 1, 1);
}


void json_atributo(int sincro[], int N){
    if (t.compLex == R_LLAVE){
        return;
    }

    int primero[1] = {COMA};
    int siguiente[1] = {R_LLAVE};
    int i = 0, is = 0;
    unir_arrays(primero, sincro, 1, N);
    while (i < N) {
        if (t.compLex == sincro[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLex == COMA) {
            machear(COMA);
            json_tipo_attribute(siguiente, 1);
            json_atributo(siguiente, 1);
        }
    }
    unir_arrays(siguiente, primero, 1, 1);
}

void json_tipo_element(int sincro[], int N){
    int primero[2] = {L_LLAVE, L_CORCHETE};
    int siguiente[1] = {R_CORCHETE};
    int i = 0, is = 0;
    unir_arrays(primero, sincro, 2, N);
    while (i < N) {
        if (t.compLex == sincro[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLex == L_CORCHETE || t.compLex == L_LLAVE) {
            json_elemento(siguiente, 1);
            json_lista_elem(siguiente, 1);
        }
    }
    else {
        imprimir_error();
    }
    unir_arrays(siguiente, primero, 1, 2);
}

void json_lista_elem(int sincro[], int N){
    if(t.compLex == R_CORCHETE){ 
       return;
    }
    int primero[1] = {COMA};
    int siguiente[1] = {R_CORCHETE};
    unir_arrays(primero, sincro, 1, N);
    if (t.compLex == COMA) {
        machear(COMA);
        json_elemento(siguiente, 1);
        json_lista_elem(siguiente, 1);
    }
    unir_arrays(siguiente, primero, 1, 1);
}


void json_tipo_attribute(int sincro[], int N){
    int primero[1] = {LITERAL_CADENA};
    int siguiente[2] = {COMA, R_LLAVE};
    int i = 0, is = 0;
    unir_arrays(primero, sincro, 1, N);
    while (i < N) {
        if (t.compLex == sincro[i]) {
            is = 1;
        }
        i++;
    }
    if (is == 0) {
        if (t.compLex == LITERAL_CADENA) {
            json_name(siguiente, 2);
            machear(DOS_PUNTOS);
            json_value(siguiente, 2);
        }
    }
    else {
        imprimir_error();
    }
    unir_arrays(siguiente, primero, 2, 1);
}


/*IMPRIMIR DATOS*/
typedef struct {
  int codigo;
  const char* descripcion;
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
		if(nombreString[i].codigo==valor){//Encontró el valor del index
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
	fichero = fopen("output.txt" , "w"); 

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
		
		sigLex();
		initJson();

		if (valido == 1) {
            printf("\n\nEl fuente es sintacticamente valido\n");
        }
		
		fclose(archivo);

		
	} else {

		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}

	return 0;
}
