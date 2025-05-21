#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// pruebas
#define maxCategoria 4       // declaraciones, informes, grabaciones, documentos, evidencias
#define maxStrCategoria 13   //
#define maxStrData 100       //

// personas
#define maxStrRut 12         // "12.345.678-9", 12 caracteres en total  
#define maxStrRol 9          // "implicado" es el str más largo dentro de los roles
#define maxStrNombre 20      //

// casos
#define maxStrEstado
#define maxStrRuc

struct Prueba {
    int id;
    char *categoria;
    char *data;
};

struct NodoPrueba { // doble enlazada
    struct Prueba *prueba;
    struct NodoPrueba *sig, *ant;
};

/* con este struct se gestionan:
 * Implicados en el caso (víctima, imputado, testigo, terceros)
 * Ficales
 */
struct Persona {
    char *rol;
    char *rut;
    char *nombre;

    // ROL IMPLICADO - JUEZ:
    // IMPLICADO: apunta al juez que lleva su caso
    // JUEZ: apunta al fiscal con quien se comunica
    struct Persona *fiscal; // implicado apunta al juez que lleva su caso

    // ROL IMPLICADO:
    int medidaCautelar; // 0: no tiene medida cautelar; 1: sí tiene medida cautelar

    // ROL FISCAL:
    struct Persona *juez;
    struct Caso *casosActivos;
    int maxCasosActivos;
};

struct Caso {
    char *ruc;
    char *estado;
    char *nombreClave;
    struct NodoPrueba **categoriasPruebas;   // array de NodoPrueba (array de listas doblemente enlazadas)
    struct NodoImplicado *implicados;        // simple enlazada
    struct Persona *fiscal;
};

/* Lista que gestiona personas
 * Consideraciones:
 * - la lista de implicados es simplemente enlazada
 * - lista de fiscales y jueces es circular simplemente enlazada
 */
struct NodoPersona {
    struct Persona *persona;
    struct NodoPersona *sig;
};

struct NodoSIAU {
    struct Caso *caso;
    struct NodoSIAU *izq, *der;
};

struct MinPublico {
    struct NodoJuez *jueces;       // circular simple
    struct NodoFiscal *fiscales;   // circular simple
    struct NodoSIAU *siau;         // árbol
};

//==========>   PRUEBAS   <==========//
void mostrarCategoriasPruebas(struct NodoPrueba **pruebas) {
    for (int i = 0; i < maxCategoria; i++)
        printf("Categoria %d: %s\n\n", i + 1, pruebas[i]->prueba->categoria);
}

void mostrarPrueba(struct Prueba *prueba){
    printf("id: %d\n", prueba->id);
    printf("categoria: %s\n", prueba->categoria);
    printf("data: %s\n\n", prueba->data);
}

void mostrarListaPruebas(struct NodoPrueba *pruebas) {
    while (pruebas != NULL) {
        mostrarPrueba(pruebas->prueba);

        pruebas = pruebas->sig;
    }
}

struct Prueba *crearPrueba() {
    struct Prueba *prueba = malloc(sizeof(struct Prueba));
    
    prueba->id = 0;
    prueba->categoria = (char *)malloc(sizeof(char) * maxStrRol);
    prueba->data = (char *)malloc(sizeof(char) * maxStrData);

    return prueba;
}

void inputCrearPrueba(struct Prueba *prueba) {
    printf("Ingrese el id de la prueba: ");
    scanf("%d", &prueba->id);
    
    printf("Ingrese la categoria de la prueba: ");
    scanf(" %[^\n]", prueba->categoria);

    printf("Ingrese los datos de la prueba: ");
    scanf(" %[^\n]", prueba->data);
}

struct NodoPrueba *crearNodoPrueba(struct Prueba *prueba) {
    struct NodoPrueba *nuevoNodo;

    nuevoNodo = (struct NodoPrueba *)malloc(sizeof(struct NodoPrueba));
    nuevoNodo->prueba = prueba;
    nuevoNodo->sig = NULL;
    nuevoNodo->ant = NULL;

    return nuevoNodo;
}

void agregarPrueba(struct NodoPrueba **pruebas, struct Prueba *prueba) {
    struct NodoPrueba *nuevoNodo = crearNodoPrueba(prueba);

    if (*pruebas == NULL) {
        *pruebas = nuevoNodo;

        return;
    }

    struct NodoPrueba *ultimo = *pruebas;

    while (ultimo->sig != NULL)
        ultimo = ultimo->sig;

    ultimo->sig = nuevoNodo;
    nuevoNodo->ant = ultimo;
}

struct Prueba *buscarPrueba(struct NodoPrueba *pruebas, int id) {
    while (pruebas != NULL) {
        if (pruebas->prueba->id == id) 
            return pruebas->prueba;

        pruebas = pruebas->sig;
    }

    return NULL;
}

int eliminarPrueba(struct NodoPrueba **pruebas, struct Prueba *prueba) {}

//==========>   CASOS   <==========//
void mostrarCaso(struct Caso *caso);
void mostrarArbolCasos(struct NodoSIAU *siau);
struct Caso *buscarCaso(char *ruc);
struct Caso *crearCaso();
void inputCrearCaso(struct Caso *caso);
void agregarCaso(struct NodoSIAU *siau, struct Caso *caso);
void modificarCaso(struct Caso *caso);
int eliminarCaso(struct Caso *caso);

//==========>   IMPLICADOS   <==========//
void mostrarImplicado(struct Persona *implicado) {
    printf("rol: %s\n", implicado->rol);
    printf("nombre: %s\n", implicado->nombre);
    printf("rut: %s\n", implicado->rut);
    printf("Fiscal a cargo: %s\n", implicado->fiscal->nombre);
    printf("Rut fiscal a cargo: %s\n\n", implicado->fiscal->rut);
}   

void mostrarListaImplicados(struct NodoPersona *implicados) {
    while (implicados != NULL) {
        mostrarImplicado(implicados->persona);

        implicados = implicados->sig;
    }
}

struct Persona *crearImplicado(struct Persona *fiscal) {
    struct Persona *implicado = malloc(sizeof(struct Persona));

    implicado->rol = (char *)malloc(sizeof(char) * maxStrRol);
    implicado->rut = (char *)malloc(sizeof(char) * maxStrRut);
    implicado->nombre = (char *)malloc(sizeof(char) * maxStrNombre);

    implicado->medidaCautelar = 0;

    implicado->fiscal = fiscal;

    return implicado;
}

void inputCrearImplicado(struct Persona *persona) {
    printf("Ingrese el rol de la persona: ");
    scanf(" %[^\n]", persona->rol);

    printf("Ingrese el nombre de la persona: ");
    scanf(" %[^\n]", persona->nombre);

    printf("Ingrese el rut de la persona: ");
    scanf(" %[^\n]", persona->rut);

}

struct NodoPersona *crearNodoImplicado(struct Persona *persona) {
    struct NodoPersona *implicado;

    implicado = (struct NodoPersona *)malloc(sizeof(struct NodoPersona));
    implicado->persona = persona; 
    implicado->sig = NULL;
    
    return implicado;
}

void agregarImplicado(struct NodoPersona *implicados, struct NodoPersona *implicado) {
    while (implicados->sig != NULL)
        implicados = implicados->sig;

    implicados->sig = implicado;
}

struct Persona *buscarImplicado(struct NodoPersona *implicados, char *rut) {
    while (implicados != NULL) {
        if (strcmp(implicados->persona->rut, rut) == 0)
            return implicados->persona;

        implicados = implicados->sig;
    }

    return NULL;
}

int eliminarImplicado(struct NodoPersona **implicados, char *rut);

//==========>   Fiscal   <==========//
void mostrarFiscal(struct Persona *fiscal) {
    printf("nombre: %s\n", fiscal->nombre);
    printf("rut: %s\n", fiscal->rut);
    printf("Juez a cargo: %s\n", fiscal->juez->nombre);
    printf("Rut juez a cargo: %s\n\n", fiscal->juez->rut);
}

//==========>   Juez   <==========//
void mostrarJuez(struct Persona *juez) {
    printf("nombre: %s\n", juez->nombre);
    printf("rut: %s\n", juez->rut);
}

int main() {

    return 0;
}