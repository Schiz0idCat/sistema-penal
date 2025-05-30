#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Pruebas
#define maxCategoria 4       // declaraciones, informes, grabaciones, documentos, evidencias
#define maxStrData 100       // la información de una prueba almacenada en un string

// Personas
#define maxStrRut 12         // "12.345.678-9", 12 caracteres en total  
#define maxStrRol 9          // "implicado" es el str más largo dentro de los roles
#define maxStrNombre 20      // máxima longitud de un nombre

// Casos
#define maxStrRuc 20         // máxima longitud de un ruc

// Fiscales
#define maxCasos 100         // casos en los que puede llegar a trabajar un fiscal

// Jueces
#define maxJueces 100        // jueces que pueden existir en el sistema 

struct Prueba {
    int id;
    int categoria; // 0: declaraciones; 1: informes; 2: grabaciones; 3: documentos; 4: evidencias
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
 * Jueces
 */
struct Persona {
    char *rol;
    char *rut;
    char *nombre;

    // ROL FISCAL:
    struct Caso **casos;
};

/* Lista que gestiona personas
 * Consideraciones:
 * - la lista de implicados es simplemente enlazada
 * - lista de fiscales es circular simplemente enlazada
 */
struct NodoPersona {
    struct Persona *persona;
    struct NodoPersona *sig;
};

struct Caso {
    char *ruc;
    int estado;                              // 0: activo; 1: archivado
    int medidaCautelar;                      // 0: sin medida cautelar; 1: con medida cautela
    struct NodoPrueba **categoriasPruebas;   // array de NodoPrueba (array de listas doblemente enlazadas)
    struct NodoPersona **implicados;         // array de implicados al caso (array de listas simplemente enlazadas)
    struct Persona *fiscal;
};

struct NodoSIAU {
    struct Caso *caso;
    struct NodoSIAU *izq, *der;
};

struct MinPublico {
    struct Persona **jueces;        // Array de jueces
    struct NodoPersona *fiscales;   // circular simple
    struct NodoSIAU *siau;          // árbol
};

//==========>   PRUEBAS   <==========//
void mostrarCategoriasPruebas(struct NodoPrueba **pruebas) {
    int i;

    for (i = 0; i < maxCategoria; i++) {
        printf("Categoria %d: %d\n", i + 1, pruebas[i]->prueba->categoria);
    }
}

void mostrarPrueba(struct Prueba *prueba){
    printf("id: %d\n", prueba->id);
    printf("categoria: %d\n", prueba->categoria);
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
    
    prueba->id = -1;
    prueba->categoria = -1;
    prueba->data = (char *)malloc(sizeof(char) * maxStrData);

    return prueba;
}

void inputCrearPrueba(struct Prueba *prueba) {
    printf("Ingrese el id de la prueba: ");
    scanf("%d", &prueba->id);
    
    printf("Ingrese la categoria de la prueba: ");
    scanf(" %d", &prueba->categoria);

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

struct NodoPrueba *buscarNodoPrueba(struct NodoPrueba *pruebas, int id) {
    while (pruebas != NULL) {
        if (pruebas->prueba->id == id) {
            return pruebas;
        }

        pruebas = pruebas->sig;
    }

    return NULL;
}

int eliminarPrueba(struct NodoPrueba **pruebas, struct Prueba *prueba) {
    struct NodoPrueba *nodoEliminar;

    nodoEliminar = buscarNodoPrueba(*pruebas, prueba->id);

    if (nodoEliminar != NULL) {
        if (nodoEliminar->ant == NULL) {
            *pruebas = (*pruebas)->sig;

            if (*pruebas != NULL) {
                (*pruebas)->ant = NULL;
            }
        }
        else {
            nodoEliminar->sig->ant = nodoEliminar->sig;
        }

        nodoEliminar->sig = NULL;
        nodoEliminar->ant = NULL;

        return 0;
    }

    return 1;
}

void modificarPrueba(struct Prueba *prueba);

//==========>   CASOS   <==========//
void mostrarCaso(struct Caso *caso) {
    printf("ruc: %s\n", caso->ruc);
    printf("estado: %d\n", caso->estado);
    printf("medida cautelar: %d\n\n", caso->medidaCautelar);
}

void mostrarArbolCasos(struct NodoSIAU *siau) {
    if (siau != NULL) {
        if (siau->izq != NULL) {
            mostrarArbolCasos(siau->izq);
        }

        mostrarCaso(siau->caso);

        if (siau->der != NULL) {
            mostrarArbolCasos(siau->der);
        }
    }
}

struct Caso *buscarCaso(struct NodoSIAU *siau, char *ruc) {
    if (siau != NULL) {
        if (strcmp(siau->caso->ruc, ruc) == 0) {
            return siau->caso;
        }
        if (strcmp(siau->caso->ruc, ruc) > 0) {
            return buscarCaso(siau->izq, ruc);
        }
        return buscarCaso(siau->der, ruc);
    }

    return NULL;
}

struct Caso *crearCaso() {
    struct Caso *caso;

    caso = (struct Caso *)malloc(sizeof(struct Caso));

    caso->estado = -1;
    caso->fiscal = NULL;
    caso->implicados = NULL;
    caso->ruc = (char *)malloc(sizeof(char) * maxStrRuc);
    caso->categoriasPruebas = NULL;
    caso->medidaCautelar = -1;

    return caso;
}

void inputCrearCaso(struct Caso *caso, struct Persona *fiscal) {
    printf("Ingresar estado del caso: ");
    scanf("%d", &caso->estado);

    printf("Asignando fiscal");
    caso->fiscal = fiscal;
}

struct NodoSIAU *crearNodoCaso(struct Caso *caso) {
    struct NodoSIAU *nuevo = (struct NodoSIAU *)malloc(sizeof(struct NodoSIAU));
    nuevo->caso = caso;
    nuevo->izq = NULL;
    nuevo->der = NULL;
    return nuevo;
}

void agregarCaso(struct NodoSIAU **siau, struct Caso *caso) {
    if (*siau == NULL) {
        *siau = crearNodoCaso(caso);
        return;
    }

    if (strcmp(caso->ruc, (*siau)->caso->ruc) < 0) {
        agregarCaso(&(*siau)->izq, caso);
    } else {
        agregarCaso(&(*siau)->der, caso);
    }
}

void modificarCaso(struct Caso *caso) {
    int opcion;
    printf("Modificar caso con RUC: %s\n", caso->ruc);
    printf("1. Cambiar estado\n");
    printf("2. Cambiar medida cautelar\n");
    printf("3. Salir\n");
    printf("Opción: ");
    scanf("%d", &opcion);

    switch (opcion) {
        case 1:
            printf("Nuevo estado: ");
            scanf("%d", &caso->estado);
            break;
        case 2:
            printf("Nueva medida cautelar: ");
            scanf("%d", &caso->medidaCautelar);
            break;
        case 3:
            break;
        default:
            printf("No se realizaron cambios.\n");
    }
}

//==========>   IMPLICADOS   <==========//
void mostrarImplicado(struct Persona *implicado) {
    printf("rol: %s\n", implicado->rol);
    printf("nombre: %s\n", implicado->nombre);
    printf("rut: %s\n", implicado->rut);
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
}

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

    fiscal->casos = (struct Caso **)malloc(sizeof(struct Caso *) * maxCasos);

    return fiscal;
}

void inputCrearImplicado(struct Persona *fiscal);

struct NodoPersona *crearNodoFiscal(struct Persona *fiscal) {
    struct NodoPersona *nuevoNodo;

    nuevoNodo = (struct NodoPersona *)malloc(sizeof(struct NodoPersona));
    
    nuevoNodo->persona = fiscal;
    nuevoNodo->sig = NULL;

    return nuevoNodo;
}

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

int eliminarFiscal(struct NodoPersona **implicados, char *rut) {
    struct NodoPersona *ultimo;
    struct NodoPersona *act;
    struct NodoPersona *ant;
    
    if (strcmp((*implicados)->persona->rut, rut) == 0) {
        if ((*implicados)->sig == *implicados) {
            *implicados = NULL;
        }
        else {
            ultimo = *implicados;

            while (ultimo->sig != NULL) {
                ultimo = ultimo->sig;
            }

            *implicados = (*implicados)->sig;
            (*implicados)->sig = *implicados;
        }

        return 1;
    }
    
    act = (*implicados)->sig;
    ant = *implicados;

    while (act != *implicados) {
        if (strcmp((*implicados)->persona->rut, rut) == 0) {
            ant->sig = act->sig;
            return 1;
        }

        ant = act;
        act = act->sig;
    }

    return 0;
}

//==========>   Juez   <==========//
void mostrarJuez(struct Persona *juez) {
    printf("nombre: %s\n", juez->nombre);
    printf("rol: %s\n", juez->rol);
    printf("rut: %s\n", juez->rut);
}

void mostrarListaJueces(struct Persona **jueces) { 
    int i;

    for (i = 0; i < maxJueces; i++) {
        if( jueces[i] != NULL) {
            mostrarJuez(jueces[i]);
        }
    }   
}

struct Persona *crearJuez() {
    struct Persona *juez;
    
    juez = (struct Persona *)malloc(sizeof(struct Persona));

    juez->rol = (char *)malloc(sizeof(char) * maxStrRol);
    juez->rut = (char *)malloc(sizeof(char) * maxStrRut);
    juez->nombre = (char *)malloc(sizeof(char) * maxStrNombre);

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

int agregarJuez(struct Persona **jueces, struct Persona *juez) {
    int i;

    for (i = 0; i < maxJueces; i++) {
        if (jueces[i] == NULL) {
            jueces[i] = juez;

            return 1; // Agregado con éxito
        }
    }

    return 0; // No se pudo agregar, el array está lleno
}

struct Persona *buscarJuez(struct Persona **jueces, char *rut) { 
    int i;

    for (i = 0; i < maxJueces; i++) {
        if (jueces[i] != NULL && strcmp(jueces[i]->rut, rut) == 0) {
            return jueces[i];
        }
    }

    return NULL;
}

int eliminarJuez(struct Persona **jueces, char *rut){
    int i;

    for (i = 0; i < maxJueces; i++) {
        if (jueces[i] != NULL && strcmp(jueces[i]->rut, rut) == 0) {
            jueces[i] = NULL;

            return 1; // Eliminado con éxito
        }
    }

    return 0; // No encontrado
}

int main() {

    return 0;
}
