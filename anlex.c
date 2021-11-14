// Librerias
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

// Constantes
#define LITERAL_CADENA		1
#define LITERAL_NUM			2
#define L_CORCHETE 			3
#define R_CORCHETE			4
#define L_LLAVE				5
#define R_LLAVE				6
#define COMA				7
#define DOS_PUNTOS			8
#define PR_TRUE				9	
#define PR_FALSE			10
#define PR_NULL				11
#define EOF					12
#define TAMBUFF 	        5
#define TAMLEX 		        50
#define TAMHASH 	        101


// Estructuras
typedef struct entrada{
	int compLex;
	char lexema[TAMLEX];	
	struct entrada *tipoDato; 
	
} entrada;

typedef struct {
	int compLex;
	char *comp;
	char *tipo_lexema ;
	entrada *pe;
} token;

// Prototipos
void insertar(entrada e);
entrada* buscar(const char *clave);
void initTabla();
void initTablaSimbolos();
void sigLex();

void tradArray();
void tradArrays();
void tradAtributo();
void tradAtriNombre();
void tradAtriValor();
void tradAtriLista();
void tradAtriList();
void tradElemento();
void tradObjetos();
void tradObj();
void tradElemLista();
void tradListaElem();
void initTrad();

void json_tipo_attribute(int sincro[], int tam);
void json_sub_objeto(int sincro[], int tam);
void json_array(int sincro[], int tam);
void json_texto(int sincro[], int tam);
void json_atributo(int sincro[], int tam);
void json_tipo_element(int sincro[], int tam);
void json_lista_elem(int sincro[], int tam);
void json_name(int sincro[], int tam);
void json_objectos(int sincro[], int tam);
void json_elemento(int sincro[], int tam);
void json_arr(int sincro[], int tam);
void json_value(int sincro[], int tam);


// Variables globales

int consumir;

char cad[5*TAMLEX];
token t;

FILE *archivo;			
FILE * fichero;  		//Archivo de Salida

char buff[2*TAMBUFF];
char id[TAMLEX];
int delantero=-1;
int fin=0;
int nroLinea=1;
int x;
char addEspacio[500];

int valido = 1;

FILE *salida;
int tabulacion=0;


// Funciones

void error(const char* mensaje)
{
	fprintf(fichero,"\nLINEA %d: ERROR LEXICO. %s.\n",nroLinea,mensaje);	
}

void sigLex(){
	int cierre=0; 
	x=0;
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	entrada e;
	
	char palabra_reservada[10];
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
			nroLinea++;
			fprintf(fichero,"\n");
			x=0;
			continue;
		}
		else if(c=='\"'){
			i=0;
			id[i]=c;
			//i++;
			while((isascii(c) || cierre == 0) && c!=EOF ){
				//Leemos hasta encontrar el \" de cierre 
				c=fgetc(archivo);
				if (c=='"'){ //forzamos la salida
					

					id[i]='\0';
					t.compLex = LITERAL_CADENA;
					t.comp = "LITERAL_CADENA";
					t.tipo_lexema = id;
					cierre=1;
					break;
					
				}else if(c==':' || c=='\n' || c==EOF ){
					sprintf(msg,"NO SE ESPERABA '%c'",c);
                    fprintf(salida," %s","SE ESPERABA QUE FINALIZE EL LITERAL");
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
		
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;
				
				while(!acepto)
				{
					switch(estado){
					case 0:
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
					
					case 1:
						c=fgetc(archivo);						
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}

						else{
							sprintf(msg,"NO SE ESPERABA '%c'",c);
							estado=-1;
						}
						break;
					case 2:
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
					case 3:
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
							sprintf(msg,"NO SE ESPERABA '%c'",c);
							estado=-1;
						}
						break;
					case 4:
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"NO SE ESPERABA '%c'",c);
							estado=-1;
						}
						break;
					case 5:
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6:
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
						t.comp = "LITERAL_NUM";
						t.tipo_lexema = id;
						t.compLex=LITERAL_NUM;
						break;
					case -1:
						if (c==EOF)
							error("NO SE ESPERABA EL FIN DE ARCHIVO");
						else
							error(msg);
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

			i=0;
			do {	
				id[i]=c;
				i++;
				c= fgetc(archivo);
			}while (isalpha(c));
			id[i]='\0';
			if (c!=EOF)
                ungetc(c,archivo);
			if(strcmp(id, "true") == 0 || strcmp(id, "TRUE") == 0){
				t.compLex=PR_TRUE;
				t.comp="PR_TRUE";
				t.tipo_lexema = "true";
				break;
			}else if (strcmp(id, "false") == 0 || strcmp(id, "FALSE") == 0)
			{
				t.compLex=PR_FALSE;
				t.comp="PR_FALSE";
				t.tipo_lexema = "false";
				break;
			}else if (strcmp(id, "null") == 0 || strcmp(id, "NULL") == 0)
			{
				t.compLex=PR_NULL;
				t.comp="PR_NULL";
				t.tipo_lexema = "NULL";
				break;

			}else{
				
                printf(msg,"NO SE ESPERABA '%s'",id);
                error(msg);	
				break;			
            } 
			
			
			

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

			sprintf(msg,"%c NO ESPERADO",c);
			error(msg);
			t.compLex=1;
			while(c!='\n'){
				c=fgetc(archivo);
			}
			strcpy(addEspacio, " ");
			x=0;
			ungetc(c,archivo);
		}
		 else {
            sprintf(msg,"NO SE ESPERABA '%c'",c);
            error(msg);
        }
	}
	if (c==EOF)
	{
		t.compLex=EOF;		
		t.comp="EOF";
		t.tipo_lexema = "EOF";
		sprintf(e.lexema,"EOF");
		t.pe=&e;
	}
	
}

// ANALIZADOR SINTACTICO

void imprimir_error(){
    valido = 0;
    fprintf(salida,"\nEN LA LINEA %d: SE REGISTRO UN ERROR SINTACTICO. NO SE ESPERABA %s. ", nroLinea, t.comp);
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
    unir_arrays(primero, siguiente, 2, 1);
    json_elemento(siguiente, 1);
}


void json_elemento(int sincro[], int N){
    int primero[2] = {L_CORCHETE, L_LLAVE};
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
        if (t.compLex == L_LLAVE) {
            json_objectos(siguiente, 4);
        }
        else if (t.compLex == L_CORCHETE) {
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

// PARTE DE LA TRADUCCION

void print_tab(){
    int i=0;
    for (i=1;i<=tabulacion;i++){
		fprintf(salida," "); 
    }
}


void initTrad(){
	tradElemento();
}


void tradElemento(){
	
	if(t.compLex == L_LLAVE){
		tradObjetos();
	}
	else if(t.compLex == L_CORCHETE){
		tradArray();
	}
}

void tradObjetos(){
	if(t.compLex == L_LLAVE){
		machear(L_LLAVE);
		tradObj();
	}
}

void tradObj(){
	if(t.compLex == LITERAL_CADENA){
        tradAtriLista();
        machear(R_LLAVE);

    }
    else if(t.compLex == R_LLAVE){
        machear(R_LLAVE);
    }
}

void tradArray(){
	if(t.compLex == L_CORCHETE){   
        machear(L_CORCHETE);
        tradArrays();
		print_tab();
    }	
}

void tradArrays(){
	if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
		fprintf(salida,"\n"); 
        print_tab();
		fprintf(salida,"<item>\n"); 
        tabulacion+=4;
        tradElemLista();
        print_tab();
		fprintf(salida,"</item>\n"); 
		tabulacion-=4; 
    }
    else if(t.compLex == R_CORCHETE){
        tabulacion-=4;
        machear(R_CORCHETE);
    }
}

void tradAtriLista() {
	if(t.compLex == LITERAL_CADENA){
        tradAtributo();
        tradAtriList();
    }
}

void tradAtriList(){
	if (t.compLex == R_LLAVE){
		tabulacion-=4;
    }
  
    if(t.compLex == COMA){
        machear(COMA);
        tradAtributo();
        tradAtriList();
    }
}

void tradElemLista(){
	if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
        tradElemento();
        tradListaElem();   
    }
}

void tradListaElem(){
	if(t.compLex == R_CORCHETE){ 
        machear(R_CORCHETE);
    }
	if(t.compLex == COMA){  
        machear(COMA);
        print_tab();
		fprintf(salida,"</item>\n"); 
        print_tab();
		fprintf(salida,"<item>\n"); 
        tabulacion+=4;
        tradElemento();
        tradListaElem();
    }
}

void tradAtributo(){
	if(t.compLex == LITERAL_CADENA){
        print_tab();
        char lexema[TAMLEX];
        strcpy(lexema,t.tipo_lexema);
		fprintf(salida,"<");
        tradAtriNombre();
		fprintf(salida,">"); 
        machear(DOS_PUNTOS);
        tradAtriValor();
		fprintf(salida,"</"); 
		fprintf(salida,"%s",lexema); 
		fprintf(salida,">\n");   
    }
}

void tradAtriNombre(){
	if(t.compLex == LITERAL_CADENA){    
        char lexema[TAMLEX];
        strcpy(lexema,t.tipo_lexema);
        fprintf(salida,"%s",lexema); 
        machear(LITERAL_CADENA);
    }
}

void tradAtriValor(){
	if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
        tabulacion +=4;
        tradElemento();
    }
    else if(t.compLex == LITERAL_CADENA){
		fprintf(salida,"\"%s\"",t.tipo_lexema);
        machear(LITERAL_CADENA);
    }
    else if(t.compLex == LITERAL_NUM){
		fprintf(salida,"%s",t.tipo_lexema);
        machear(LITERAL_NUM);
    }
    else if(t.compLex == PR_TRUE){
		fprintf(salida,"%s",t.tipo_lexema);
        machear(PR_TRUE);
    }
    else if(t.compLex == PR_FALSE){
		fprintf(salida,"%s",t.tipo_lexema);
        machear(PR_FALSE);
    }
    else if(t.compLex == PR_NULL){
		fprintf(salida,"%s",t.tipo_lexema);
        machear(PR_NULL);
    }
}


typedef struct {
  int codigo;
  const char* descripcion;
} nombres;

nombres nombreString[] = {
  {L_CORCHETE, "L_CORCHETE" },
  {R_CORCHETE, "R_CORCHETE"},
  {L_LLAVE, "L_LLAVE" },
  {R_LLAVE, "R_LLAVE"},
  {COMA, "COMA"},
  {DOS_PUNTOS, "DOS_PUNTOS" },
  {LITERAL_CADENA, "LITERAL_CADENA"},  
  {LITERAL_NUM, "LITERAL_NUM" },
  {PR_TRUE, "PR_TRUE"},
  {PR_FALSE, "PR_FALSE" }
};

void imprimir(int valor){
	int i;
	char encontrado='n';
	for(i=0;i<11 ; i++){
		if(nombreString[i].codigo==valor){
			encontrado='s';
			break;
		}
	}
	if (encontrado=='s'){
		fprintf(salida,"%s ",nombreString[i].descripcion);
	}else{
	
	}
}

int main(int argc,char* args[])
{

	initTabla();
	initTablaSimbolos();
	
	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("ARCHIVO NO ENCONTRADO.\n");
			exit(1);
		}
		
		salida=fopen("salida.xml","w");
		sigLex();
		initJson();

		if (valido) {
			fclose(archivo);
            archivo=fopen(args[1],"rt");
			sigLex();
			if (valido==1){
                 initTrad();
                 printf("\nEL FUENTE ES SINTACTICAMENTE CORRECTO\n");
			} else {
                  initTrad();
                  printf("\nEL FUENTE TIENE ERRORES SINTACTICOS.\n");
			}
		} else {	
			initTrad();
			printf("\nEL FUENTE TIENE ERRORES SINTACTICOS.\n");
		}
		fclose(salida);

		
	} else {

		printf("DEBE PASAR COMO PARAMETRO EL PATH AL ARCHIVO FUENTE.\n");
		exit(1);
	}

	return 0;
}
