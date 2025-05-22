#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Pruebas
#define maxCategoria 4       // declaraciones, informes, grabaciones, documentos, evidencias
#define maxStrCategoria 13   // "declaraciones", 13 caracteres en total
#define maxStrData 100       // la información de una prueba almacenada en un string

// Personas
#define maxStrRut 12         // "12.345.678-9", 12 caracteres en total  
#define maxStrRol 9          // "implicado" es el str más largo dentro de los roles
#define maxStrNombre 20      // máxima longitud de un nombre

// Casos
#define maxStrEstado         // "cerrado", "archivado"
#define maxStrRuc            // máxima longitud de un ruc

// Fiscales
#define maxCasosActivos 5    // casos en los que trabaja un fiscal al mismo tiempo

struct Prueba {
    int id;
    char *categoria;
    char *data;
};

// doble enlazada
struct NodoPrueba {
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
    struct Caso **casosActivos;
};

struct Caso {
    char *ruc;
    char *estado;
    char *nombreClave;
    struct NodoPrueba **categoriasPruebas;   // array de NodoPrueba (array de listas doblemente enlazadas)
    struct NodoPersona *implicados;          // simple enlazada
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
    struct NodoPersona *jueces;     // circular simple
    struct NodoPersona *fiscales;   // circular simple
    struct NodoSIAU *siau;          // árbol
};

//==========>   PRUEBAS   <==========//
void mostrarCategoriasPruebas(struct NodoPrueba **pruebas) {
    int i;

    for (i = 0; i < maxCategoria; i++) {
        printf("Categoria %d: %s\n", i + 1, pruebas[i]->prueba->categoria);
    }
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
    struct Prueba *prueba;

    prueba = (struct Prueba *)malloc(sizeof(struct Prueba));
    
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
    struct NodoPrueba *nuevoNodo;
    struct NodoPrueba *ultimo;
    
    nuevoNodo = crearNodoPrueba(prueba);
    ultimo = *pruebas;

    if (*pruebas == NULL) {
        *pruebas = nuevoNodo;

        return;
    }   

    while (ultimo->sig != NULL) {
        ultimo = ultimo->sig;
    }

    ultimo->sig = nuevoNodo;
    nuevoNodo->ant = ultimo;
}

struct Prueba *buscarPrueba(struct NodoPrueba *pruebas, int id) {
    while (pruebas != NULL) {
        if (pruebas->prueba->id == id) {
            return pruebas->prueba;
        }

        pruebas = pruebas->sig;
    }

    return NULL;
}

int eliminarPrueba(struct NodoPrueba **pruebas, struct Prueba *prueba);

void modificarPrueba(struct Prueba *prueba);

//==========>   CASOS   <==========//
void mostrarCaso(struct Caso *caso);
void mostrarArbolCasos(struct NodoSIAU *siau);
struct Caso *buscarCaso(char *ruc);
struct Caso *crearCaso();
void inputCrearCaso(struct Caso *caso);
void crearNodoCaso(struct Caso *caso);
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
    struct Persona *implicado;
    
    implicado = (struct Persona *)malloc(sizeof(struct Persona));

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

struct NodoPersona *crearNodoImplicado(struct Persona *implicado) {
    struct NodoPersona *NodoImplicado;

    NodoImplicado = (struct NodoPersona *)malloc(sizeof(struct NodoPersona));

    NodoImplicado->persona = implicado; 
    NodoImplicado->sig = NULL;
    
    return NodoImplicado;
}

void agregarImplicado(struct NodoPersona **implicados, struct NodoPersona *implicado) {
    struct NodoPersona *ultimo;
    
    ultimo = *implicados;
    
    if (*implicados == NULL) {
        *implicados = implicado;

        return;
    }
    
    while (ultimo->sig != NULL) {
        ultimo = ultimo->sig;
    }

    ultimo->sig = implicado;
}

struct Persona *buscarImplicado(struct NodoPersona *implicados, char *rut) {
    while (implicados != NULL) {
        if (strcmp(implicados->persona->rut, rut) == 0) {
            return implicados->persona;
        }

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

// circular simple
void mostrarListaFiscales(struct NodoPersona *fiscales) { 
    struct NodoPersona *actual;
    
    actual = fiscales;

    do {
        mostrarFiscal(actual->persona);
        actual = actual->sig;
    } while (actual != fiscales);
}

struct Persona *crearFiscal() {
    struct Persona *fiscal;
    
    fiscal = malloc(sizeof(struct Persona));

    fiscal->rol = (char *)malloc(sizeof(char) * maxStrRol);
    fiscal->rut = (char *)malloc(sizeof(char) * maxStrRut);
    fiscal->nombre = (char *)malloc(sizeof(char) * maxStrNombre);

    fiscal->juez = NULL;
    fiscal->casosActivos = (struct Caso **)malloc(sizeof(struct Caso *) * maxCasosActivos);

    return fiscal;
}

void inputCrearImplicado(struct Persona *fiscal);

// nodo simple
struct NodoPersona *crearNodoFiscal(struct Persona *fiscal) {
    struct NodoPersona *nuevoNodo;

    nuevoNodo = (struct NodoPersona *)malloc(sizeof(struct NodoPersona));
    
    nuevoNodo->persona = fiscal;
    nuevoNodo->sig = NULL;

    return nuevoNodo;
}

// circular simple
void agregarFiscal(struct NodoPersona **fiscales, struct Persona *fiscal) { 
    struct NodoPersona *nuevoNodo;
    struct NodoPersona *ultimo;
    
    nuevoNodo = crearNodoFiscal(fiscal);
    
    ultimo = *fiscales;

    if (*fiscales == NULL) {
        *fiscales = nuevoNodo;
        nuevoNodo->sig = nuevoNodo;
        return;
    }

    while (ultimo->sig != *fiscales) {
        ultimo = ultimo->sig;
    }

    ultimo->sig = nuevoNodo;
    nuevoNodo->sig = *fiscales;
}

// circular simple
struct Persona *buscarFiscal(struct NodoPersona *fiscales, char *rut) {
    struct NodoPersona *actual;
    
    actual = fiscales;

    do {
        if (strcmp(actual->persona->rut, rut) == 0) {
            return actual->persona;
        }

        actual = actual->sig;
    } while (actual != fiscales);

    return NULL;
}

int eliminarFiscal(struct NodoPersona **implicados, char *rut);

//==========>   Juez   <==========//
void mostrarJuez(struct Persona *juez) {
    printf("nombre: %s\n", juez->nombre);
    printf("rut: %s\n", juez->rut);
}

// circular simple
void mostrarListaJueces(struct NodoPersona *jueces) { 
    struct NodoPersona *actual;
    
    actual = jueces;

    do {
        mostrarJuez(actual->persona);
        actual = actual->sig;
    } while (actual != jueces);
}

struct Persona *crearJuez(struct Persona *fiscal) {
    struct Persona *juez;
    
    juez = (struct Persona *)malloc(sizeof(struct Persona));

    juez->rol = (char *)malloc(sizeof(char) * maxStrRol);
    juez->rut = (char *)malloc(sizeof(char) * maxStrRut);
    juez->nombre = (char *)malloc(sizeof(char) * maxStrNombre);
    juez->fiscal = fiscal;

    return juez;
}

void inputCrearJuez(struct Persona *juez) {
    printf("Ingrese el rol de la persona: ");
    scanf(" %[^\n]", juez->rol);

    printf("Ingrese el nombre de la persona: ");
    scanf(" %[^\n]", juez->nombre);

    printf("Ingrese el rut de la persona: ");
    scanf(" %[^\n]", juez->rut);
}

// nodo simple
struct NodoPersona *crearNodoJuez(struct Persona *juez) { 
    struct NodoPersona *nuevoNodo;

    nuevoNodo = (struct NodoPersona *)malloc(sizeof(struct NodoPersona));
    
    nuevoNodo->persona = juez;
    nuevoNodo->sig = NULL;

    return nuevoNodo;
}

// circular simple
void agregarJuez(struct NodoPersona **jueces, struct Persona *juez) {
    struct NodoPersona *nuevoNodo;
    struct NodoPersona *ultimo;

    nuevoNodo = crearNodoJuez(juez);
    ultimo = *jueces;

    if (*jueces == NULL) {
        *jueces = nuevoNodo;
        nuevoNodo->sig = nuevoNodo;
        return;
    }

    while (ultimo->sig != *jueces) {
        ultimo = ultimo->sig;
    }

    ultimo->sig = nuevoNodo;
    nuevoNodo->sig = *jueces;
}

struct Persona *buscarJuez(struct NodoPersona *jueces, char *rut) { // circular simple
    struct NodoPersona *actual;

    actual = jueces;

    do {
        if (strcmp(actual->persona->rut, rut) == 0) {
            return actual->persona;
        }

        actual = actual->sig;
    } while (actual != jueces);

    return NULL;
}

// circular simple
int eliminarJuez(struct NodoPersona **jueces, char *rut);

int main() {

    return 0;
}