#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>


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

#define TAMBUFF 	5
#define TAMLEX 		50
#define TAMHASH 	101

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

