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
    char *rut;
    char *nombre;
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

//==========>   GENERAL   <==========//
struct Persona *crearPersona() {
    struct Persona *persona;

    persona = (struct Persona *)malloc(sizeof(struct Persona));

    persona->rut = (char *)malloc(sizeof(char) * maxStrRut);
    persona->nombre = (char *)malloc(sizeof(char) * maxStrNombre);

    return persona;
}

void inputCrearPersona(struct Persona *persona) {
    printf("\nIngrese nombre: ");
    scanf(" %[^\n]", persona->nombre);

    printf("Ingrese rut: ");
    scanf(" %[^\n]", persona->rut);
}

void mostrarPersona(struct Persona *persona) {
    printf("\nNombre: %s\n", persona->nombre);
    printf("Rut: %s\n", persona->rut);
}

void modificarPersona(struct Persona *persona) {
    int opcion;

    do {
        printf("\nPersona a modificar:\n");
        mostrarPersona(persona);
        printf("\n1.- Modificar nombre\n");
        printf("2.- Modificar rut\n");
        printf("3.- Salir\n");
        printf("Elija una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                printf("\nIngrese el nuevo nombre: ");
                scanf(" %[^\n]", persona->nombre);
                printf("Nombre actualizado correctamente.\n");
                break;
            case 2:
                printf("\nIngrese el nuevo RUT: ");
                scanf(" %[^\n]", persona->rut);
                printf("RUT actualizado correctamente.\n");
                break;
            case 3:
                printf("\nSaliendo del menú de modificación...\n");
                break;
            default:
                printf("\nOpción inválida. Intente de nuevo.\n");
        }
    } while (opcion != 3);
}

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

void modificarPrueba(struct Prueba *prueba) {
    int opcion;

    printf("Modificar prueba con ID: %d\n", prueba->id);
    printf("1. Cambiar categoría\n");
    printf("2. Cambiar data\n");
    printf("3. Salir\n");
    printf("Opción: ");
    scanf("%d", &opcion);

    switch (opcion) {
        case 1:
            printf("Nueva categoría: ");
            scanf("%d", &prueba->categoria);
            break;
        case 2:
            printf("Nueva data: ");
            scanf(" %[^\n]", prueba->data);
            break;
        case 3:
            break;
        default:
            printf("Opción inválida.\n");
    }
}

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
void mostrarListaImplicados(struct NodoPersona *implicados) {
    while (implicados != NULL) {
        mostrarPersona(implicados->persona);

        implicados = implicados->sig;
    }
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

int eliminarImplicado(struct NodoPersona **implicados, char *rut) {
    struct NodoPersona *act;
    struct NodoPersona *ant;

    act = *implicados;
    ant = NULL;

    if (strcmp((*implicados)->persona->rut, rut) == 0) {
        *implicados = (*implicados)->sig;

        return 1;
    }

    while (act != NULL && strcmp((act)->persona->rut, rut) != 0) {
        ant = act;
        act = act->sig;
    }

    if (act != NULL) {
        ant->sig = act->sig;

        return 1;
    }

    return 0;
}

void interaccionCasos() {
    printf("\ninteractuando con los casos...\n\n");
}

//==========>   Fiscal   <==========//
void mostrarListaFiscales(struct NodoPersona *fiscales) { 
    struct NodoPersona *actual;
    
    actual = fiscales;

    do {
        mostrarPersona(actual->persona);
        actual = actual->sig;
    } while (actual != fiscales);
}

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

            while (ultimo->sig != *implicados) {
                ultimo = ultimo->sig;
            }

            *implicados = (*implicados)->sig;
            ultimo->sig = *implicados;
        }

        return 1; // eliminado correctamente
    }
    
    act = (*implicados)->sig;
    ant = *implicados;

    while (act != *implicados) {
        if (strcmp(act->persona->rut, rut) == 0) {
            ant->sig = act->sig;
            return 1; // eliminado correctamente
        }

        ant = ant->sig;
        act = act->sig;
    }

    return 0; // eliminado incorrectamente
}

void interaccionFiscales(struct NodoPersona **fiscales) {
    int opcion;
    struct Persona *fiscal;
    char *rut;
    
    fiscal = NULL;
    rut = (char *)malloc(sizeof(char) * maxStrRut);

    do {
        printf("\nGESTIÓN DE FISCALES\n");
        printf("1.- Mostrar Fiscales\n");
        printf("2.- Mostrar Fiscal\n");
        printf("3.- Agregar Fiscal\n");
        printf("4.- Eliminar Fiscal\n");
        printf("5.- Modificar Fiscal\n");
        printf("6.- Salir\n");
        printf("Elija una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: // mostrar todos
                if (*fiscales == NULL) {
                    printf("\nNo hay fiscales registrados\n");
                }
                else {
                    mostrarListaFiscales(*fiscales);
                }
                break;
            case 2: // mostrar uno solo
                if (*fiscales == NULL) {
                    printf("\nNo hay fiscales registrados\n");
                }
                else {
                    printf("\nIngrese el rut del fiscal a mostrar: ");
                    scanf(" %[^\n]", rut);

                    fiscal = buscarFiscal(*fiscales, rut);

                    if (fiscal == NULL) {
                        printf("\nNo hay ningún fiscal con rut: %s\n", rut);
                    }
                    else {
                        mostrarPersona(fiscal);
                    }
                }
                break;
            case 3: // Agregar
                fiscal = crearPersona();
                inputCrearPersona(fiscal);
                agregarFiscal(fiscales, fiscal);
                break;
            case 4: // Eliminar eliminar
                if (*fiscales == NULL) {
                    printf("\nNo hay fiscales regsitrados\n");
                }
                else {
                    printf("\nIngrese el rut del fiscal a eliminar: ");
                    scanf(" %[^\n]", rut);

                    if (eliminarFiscal(fiscales, rut) == 1) {
                        printf("\nFiscal eliminado correctamente\n");
                    } else {
                        printf("\nNo hay ningún fiscal con rut: %s\n", rut);
                    }
                }
                break;
            case 5: // modificar
                if (*fiscales == NULL) {
                    printf("\nNo hay fiscales registrados\n");
                }
                else {
                    printf("\nIngrese el rut del fiscal a mostrar: ");
                    scanf(" %[^\n]", rut);

                    fiscal = buscarFiscal(*fiscales, rut);

                    if (fiscal == NULL) {
                        printf("\nNo hay ningún fiscal con rut: %s\n", rut);
                    }
                    else {
                        modificarPersona(fiscal);
                    }
                }
                break;
            case 6: // salir de la interfaz
                printf("\nSaliendo de la interfaz...\n\n");
                break;
            default:
                printf("\nOpción inválida. Intente de nuevo.\n"); 
        }
    } while (opcion != 6);
}

//==========>   Juez   <==========//
int mostrarArregloJueces(struct Persona **jueces) { 
    int i;
    int flag;

    flag = 0; // 0: NO hay jueces; 1: SÍ hay jueces

    for (i = 0; i < maxJueces; i++) {
        if( jueces[i] != NULL) {
            mostrarPersona(jueces[i]);
            flag = 1;
        }
    }   

    return flag;
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

void interaccionJueces(struct Persona **jueces) {
    int opcion;
    struct Persona *juez;
    char *rut;
    
    juez = NULL;
    rut = (char *)malloc(sizeof(char) * maxStrRut);

    do {
        printf("\nGESTIÓN DE JUECES\n");
        printf("1.- Mostrar Jueces\n");
        printf("2.- Mostrar Juez\n");
        printf("3.- Agregar Juez\n");
        printf("4.- Eliminar Juez\n");
        printf("5.- Modificar Juez\n");
        printf("6.- Salir\n");
        printf("Elija una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: // mostrar todos
                if (*jueces == NULL) {
                    printf("\nNo hay jueces registrados\n");
                }
                else {
                    if (mostrarArregloJueces(jueces) == 0) {
                        printf("\nNo hay jueces registrados\n");
                    }
                }
                break;
            case 2: // mostrar uno solo
                if (*jueces == NULL) {
                    printf("\nNo hay jueces registrados\n");
                }
                else {
                    printf("\nIngrese el rut del juez a mostrar: ");
                    scanf(" %[^\n]", rut);

                    juez = buscarJuez(jueces, rut);

                    if (juez == NULL) {
                        printf("\nNo hay ningún juez con rut: %s\n", rut);
                    }
                    else {
                        mostrarPersona(juez);
                    }
                }
                break;
            case 3: // Agregar
                juez = crearPersona();
                inputCrearPersona(juez);
                agregarJuez(jueces, juez);
                break;
            case 4: // Eliminar eliminar
                if (*jueces == NULL) {
                    printf("\nNo hay jueces regsitrados\n");
                }
                else {
                    printf("\nIngrese el rut del juez a eliminar: ");
                    scanf(" %[^\n]", rut);

                    if (eliminarJuez(jueces, rut) == 1) {
                        printf("\nJuez eliminado correctamente\n");
                    } else {
                        printf("\nNo hay ningún juez con rut: %s\n", rut);
                    }
                }
                break;
            case 5: // modificar
                if (*jueces == NULL) {
                    printf("\nNo hay jueces registrados\n");
                }
                else {
                    printf("\nIngrese el rut del juez a mostrar: ");
                    scanf(" %[^\n]", rut);

                    juez = buscarJuez(jueces, rut);

                    if (juez == NULL) {
                        printf("\nNo hay ningún juez con rut: %s\n", rut);
                    }
                    else {
                        modificarPersona(juez);
                    }
                }
                break;
            case 6: // salir de la interfaz
                printf("\nSaliendo de la interfaz...\n\n");
                break;
            default:
                printf("\nOpción inválida. Intente de nuevo.\n"); 
        }
    } while (opcion != 6);
}

int main() {
    struct NodoPersona *fiscales;
    struct Persona **jueces;
    int opcion;

    fiscales = NULL;
    jueces = (struct Persona **)malloc(sizeof(struct Persona *) * maxJueces);

    do {
        printf("SISTEMA PENAL\n");
        printf("1.- Jueces.\n");
        printf("2.- Fiscales.\n");
        printf("3.- Casos.\n");
        printf("4.- salir.\n");
        printf("Elija una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                interaccionJueces(jueces);
                break;
            case 2:
                interaccionFiscales(&fiscales);
                break;
            case 3:
                interaccionCasos();
                break;
            case 4:
                printf("\nSaliendo del programa...");
                break;
            default:
                printf("\nPor favor escoja una opción válida.\n\n");
        }
    } while (opcion != 4);

    return 0;
}
