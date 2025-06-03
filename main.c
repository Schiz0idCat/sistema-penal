#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Pruebas
#define maxCategoria 5       // declaraciones, informes, grabaciones, documentos, evidencias
#define maxStrData 100       // la información de una prueba almacenada en un string

// Personas
#define maxStrRut 12         // "12.345.678-9", 12 caracteres en total  
#define maxStrNombre 20      // máxima longitud de un nombre

// Casos
#define maxStrRuc 20         // máxima longitud de un ruc
#define maxImplicados 4      // 0: imputado; 1: victima; 2: testigo; 3: tercero
#define maxStrRol 9          // "imputado" es el str más largo dentro de los roles

// Jueces
#define maxJueces 100        // jueces que pueden existir en el sistema 

struct Prueba {
    int id;
    int visible;         // 0: no visible por implicados; 1: sí visible por implicados
    char *responsable;
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
    int estado;                              // 0: archivado; 1: activo
    int medidaCautelar;                      // 0: sin medida cautelar; 1: con medida cautela
    struct NodoPrueba **categoriasPruebas;   // array de NodoPrueba (array de listas doblemente enlazadas)
    struct NodoPersona **implicados;         // array de implicados al caso (array de listas simplemente enlazadas)
    struct Persona *fiscal;                  // fiscal a cargo del caso
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

void agregarImplicado(struct NodoPersona **implicados, struct Persona *implicado) {
    struct NodoPersona *nuevo;
    struct NodoPersona *ultimo;

    nuevo = crearNodoImplicado(implicado);
    ultimo = *implicados;
    
    if (*implicados == NULL) {
        *implicados = nuevo;

        return;
    }
    
    while (ultimo->sig != NULL) {
        ultimo = ultimo->sig;
    }

    ultimo->sig = nuevo;
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

void interaccionListaImplicados(struct NodoPersona **implicados) {
    struct Persona *implicado;
    char *rut;
    int opcion;

    implicado = NULL;
    rut = (char *)malloc(sizeof(char) * maxStrRut);

    do {
        printf("\nGESTIÓN DE IMPLICADOS\n");
        printf("1.- Mostrar Implicados.\n");
        printf("2.- Mostrar Implicado.\n");
        printf("3.- Agregar Implicado.\n");
        printf("4.- Salir.\n");
        printf("Elija una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: // mostrar todos
                if (*implicados == NULL) {
                    printf("\nNo hay implicados registrados\n");
                }
                else {
                    mostrarListaImplicados(*implicados);
                }
                break;
            case 2: // mostrar uno solo
                if (*implicados == NULL) {
                    printf("\nNo hay implicados registrados\n");
                }
                else {
                    printf("\nIngrese el rut del implicado a mostrar: ");
                    scanf(" %[^\n]", rut);

                    implicado = buscarImplicado(*implicados, rut);

                    if (implicado == NULL) {
                        printf("\nNo hay ningún implicado con rut: %s\n", rut);
                    }
                    else {
                        mostrarPersona(implicado);
                    }
                }
                break;
            case 3: // Agregar
                implicado = crearPersona();
                inputCrearPersona(implicado);
                agregarImplicado(implicados, implicado);
                break;
            case 4: // salir de la interfaz
                printf("\nSaliendo de la interfaz...\n");
                break;
            default:
                printf("\nOpción inválida. Intente de nuevo.\n"); 
        }
    } while (opcion != 4);
}

void interaccionCategoriasImplicados(struct NodoPersona **implicados) {
    int opcion;

    do {
        printf("\nTIPOS DE IMPLICADOS:\n");
        printf("1.- Imputados.\n");
        printf("2.- Victimas.\n");
        printf("3.- Testigos.\n");
        printf("4.- Terceros.\n");
        printf("5.- Salir.\n");
        printf("Seleccione el tipo de implicado con el que desea interactuar: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 5:
                printf("\nSaliendo de la interfaz...\n");
                break;
            default:
                interaccionListaImplicados(&implicados[opcion - 1]);
        }
    } while (opcion != 5);
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
    struct Persona *fiscal;
    char *rut;
    int opcion;
    
    fiscal = NULL;
    rut = (char *)malloc(sizeof(char) * maxStrRut);

    do {
        printf("\nGESTIÓN DE FISCALES\n");
        printf("1.- Mostrar Fiscales.\n");
        printf("2.- Mostrar Fiscal.\n");
        printf("3.- Agregar Fiscal.\n");
        printf("4.- Eliminar Fiscal.\n");
        printf("5.- Modificar Fiscal.\n");
        printf("6.- Salir.\n");
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
                    printf("\nNo hay fiscales registrados\n");
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
    struct Persona *juez;
    char *rut;
    int opcion;
    
    juez = NULL;
    rut = (char *)malloc(sizeof(char) * maxStrRut);

    do {
        printf("\nGESTIÓN DE JUECES\n");
        printf("1.- Mostrar Jueces.\n");
        printf("2.- Mostrar Juez.\n");
        printf("3.- Agregar Juez.\n");
        printf("4.- Eliminar Juez.\n");
        printf("5.- Modificar Juez.\n");
        printf("6.- Salir.\n");
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
                    printf("\nNo hay jueces registrados\n");
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

//==========>   PRUEBAS   <==========//
void mostrarPrueba(struct Prueba *prueba){
    printf("\nid: %d\n", prueba->id);
    printf("data: %s\n", prueba->data);
    printf("responsable: %s\n", prueba->responsable);
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
    
    prueba->id = -1;       // id inválido por defecto, hasta que se le asigne uno
    prueba->visible = 0;   // invisible por defecto
    prueba->data = (char *)malloc(sizeof(char) * maxStrData);
    prueba->responsable = (char *)malloc(sizeof(char) * maxStrNombre);

    return prueba;
}

void inputCrearPrueba(struct Prueba *prueba) {
    printf("\nIngrese el id de la prueba: ");
    scanf("%d", &prueba->id);

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

int interaccionInputPrueba(struct Prueba *prueba, struct Persona **jueces) {
    int opcion;
    char *responsable;

    responsable = (char *)malloc(sizeof(char) * maxStrRut);

    do {
        printf("\nGESTION RELLENAR PRUEBA\n");
        printf("¿Quién gestiona la recopilación de la prueba?\n");
        printf("1.- Juez de garantía.\n");
        printf("2.- Entidad externa.\n");
        printf("3.- Salir.\n");
        printf("Elija una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                if (*jueces == NULL) {
                    printf("\nNo hay jueces registrados\n");
                    return 1; // no se logra asignar juez
                }
                else {
                    printf("\nIngrese el rut del juez: ");
                    scanf(" %[^\n]", responsable);

                    if (buscarJuez(jueces, responsable) == NULL) {
                        printf("\nNo hay ningún juez con rut: %s\n", responsable);
                        return 1; // no se logra asignar juez
                    }
                    else {
                        prueba->responsable = responsable;
                    }
                }
                break;
            case 2:
                printf("Ingrese la entidad responsable: ");
                scanf(" %[^\n]", prueba->responsable);
                break;
            case 3:
                printf("\nSaliendo de la interfaz...\n");
                return 1; // se sale sin asignar a nadie
                break;
            default:
                printf("\nOpción inválida. Intente de nuevo.\n");
        }
    } while (opcion != 1 && opcion != 2 && opcion != 3);

    return 0; // se logró agregar un responsable
}

void interaccionListaPruebas(struct NodoPrueba **pruebas, struct Persona **jueces) {
    struct Prueba *prueba;
    int id;
    int opcion;

    prueba = NULL;

    do {
        printf("\nGESTIÓN DE PRUEBAS\n");
        printf("1.- Mostrar Pruebas.\n");
        printf("2.- Mostrar Prueba.\n");
        printf("3.- Agregar Prueba.\n");
        printf("4.- Salir.\n");
        printf("Elija una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: // mostrar todos
                if (*pruebas == NULL) {
                    printf("\nNo hay pruebas registrados\n");
                }
                else {
                    mostrarListaPruebas(*pruebas);
                }
                break;
            case 2: // mostrar uno solo
                if (*pruebas == NULL) {
                    printf("\nNo hay pruebas registrados\n");
                }
                else {
                    printf("\nIngrese el id de la prueba a mostrar: ");
                    scanf(" %d", &id);

                    prueba = buscarPrueba(*pruebas, id);

                    if (prueba == NULL) {
                        printf("\nNo hay ninguna prueba con id: %d\n", id);
                    }
                    else {
                        mostrarPrueba(prueba);
                    }
                }
                break;
            case 3: // Agregar
                prueba = crearPrueba();
                if (interaccionInputPrueba(prueba, jueces) == 0){
                    inputCrearPrueba(prueba);
                    agregarPrueba(pruebas, prueba);
                }
                break;
            case 4: // salir de la interfaz
                printf("\nSaliendo de la interfaz...\n");
                break;
            default:
                printf("\nOpción inválida. Intente de nuevo.\n"); 
        }
    } while (opcion != 4);
}

void interaccionCategoriasPruebas(struct NodoPrueba **pruebas, struct Persona **jueces) {
    int opcion;

    do {
        printf("\nTIPOS DE PRUEBAS:\n");
        printf("1.- Declaraciones.\n");
        printf("2.- Informes.\n");
        printf("3.- Grabaciones.\n");
        printf("4.- Documentos.\n");
        printf("4.- Evidencias.\n");
        printf("5.- Salir\n");
        printf("Seleccione el tipo de prueba con el que desea interactuar: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 5:
                printf("Volviendo a la interfaz de casos...\n");
                break;
            default:
                interaccionListaPruebas(&pruebas[opcion - 1], jueces);
        }
    } while (opcion != 5);
}

//==========>   CASOS   <==========//
void mostrarCaso(struct Caso *caso) {
    printf("\nruc: %s\n", caso->ruc);
    printf("estado: %d\n", caso->estado);
    printf("medida cautelar: %d\n", caso->medidaCautelar);
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

    caso->estado = -1; // estado inválido por defecto
    caso->fiscal = NULL;
    caso->implicados = (struct NodoPersona **)malloc(sizeof(struct NodoPersona *) * maxImplicados);
    caso->ruc = (char *)malloc(sizeof(char) * maxStrRuc);
    caso->categoriasPruebas = (struct NodoPrueba **)malloc(sizeof(struct NodoPrueba *) * maxCategoria);
    caso->medidaCautelar = -1; // medida cautelar inválida por defecto

    return caso;
}

void inputCrearCaso(struct Caso *caso, struct Persona *fiscal) {
    printf("\nIngrese Ruc: ");
    scanf(" %[^\n]", caso->ruc);

    printf("Ingresar estado del caso: ");
    scanf("%d", &caso->estado);

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

void modificarCaso(struct Caso *caso, struct Persona **jueces) {
    int opcion;
    printf("Modificar caso con RUC: %s\n", caso->ruc);
    printf("1.- Cambiar estado.\n");
    printf("2.- Cambiar medida cautelar.\n");
    printf("3.- Interactuar implicados.\n");
    printf("4.- Interactuar pruebas.\n");
    printf("5.- Salir\n");
    printf("Opción: ");
    scanf("%d", &opcion);

    do {
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
                interaccionCategoriasImplicados(caso->implicados);
                break;
            case 4:
                interaccionCategoriasPruebas(caso->categoriasPruebas, jueces);
                break;
            case 5:
                printf("Saliendo de la interfaz\n");
                break;
            default:
                printf("\nOpción inválida. Intente de nuevo.\n");
        }
    } while (opcion != 5);
}

void interaccionCasos(struct NodoSIAU **siau, struct NodoPersona *fiscales, struct Persona **jueces) {
    struct Caso *caso;
    struct Persona *fiscal;
    char *ruc;
    char *rut;
    int opcion;

    caso = NULL;
    fiscal = NULL;
    ruc = (char *)malloc(sizeof(char) * maxStrRuc);
    rut = (char *)malloc(sizeof(char) * maxStrRut);

    do {
        printf("\nGESTIÓN DE CASOS\n");
        printf("1.- Mostrar los casos.\n");
        printf("2.- Mostrar un caso.\n");
        printf("3.- Agregar caso.\n");
        printf("4.- Modificar caso.\n");
        printf("5.- Salir.\n");
        printf("Elija una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: // Mostrar todos
                if (*siau == NULL) {
                    printf("\nNo hay casos registrados\n");
                } else {
                    printf("\nÁrbol de casos:\n");
                    mostrarArbolCasos(*siau);
                }
                break;
            case 2: // Mostrar uno solo
                if (*siau == NULL) {
                    printf("\nNo hay casos registrados\n");
                } else {
                    printf("\nIngrese el RUC del caso a buscar: ");
                    scanf(" %[^\n]", ruc);

                    caso = buscarCaso(*siau, ruc);

                    if (caso == NULL) {
                        printf("\nNo se encontró ningún caso con RUC: %s\n", ruc);
                    } else {
                        mostrarCaso(caso);

                        printf("\nListando imputados.");
                        mostrarListaImplicados(caso->implicados[0]);

                        printf("\nListando víctimas.");
                        mostrarListaImplicados(caso->implicados[1]);

                        printf("\nListando testigos.");
                        mostrarListaImplicados(caso->implicados[2]);

                        printf("\nListando terceros.");
                        mostrarListaImplicados(caso->implicados[3]);

                        printf("\nListando declaraciones.");
                        mostrarListaPruebas(caso->categoriasPruebas[0]);

                        printf("\nListando informes.");
                        mostrarListaPruebas(caso->categoriasPruebas[1]);

                        printf("\nListando grabaciones.");
                        mostrarListaPruebas(caso->categoriasPruebas[2]);

                        printf("\nListando documentos.");
                        mostrarListaPruebas(caso->categoriasPruebas[3]);

                        printf("\nListando evidencias.");
                        mostrarListaPruebas(caso->categoriasPruebas[4]);
                    }
                }
                break;
            case 3: // Agregar nuevo caso
                if (fiscales == NULL) {
                    printf("\nNo hay fiscales a los cuales asignarle un caso.\n");
                }
                else {
                    printf("\nIngrese el RUT del fiscal a cargo: ");
                    scanf(" %[^\n]", rut);

                    fiscal = buscarFiscal(fiscales, rut);

                    if (fiscal == NULL) {
                        printf("\nNo hay ningún fiscal con rut: %s\n", rut);
                    }
                    else {
                        caso = crearCaso();

                        inputCrearCaso(caso, fiscal);
                        agregarCaso(siau, caso);
                        interaccionCategoriasImplicados(caso->implicados);
                        interaccionCategoriasPruebas(caso->categoriasPruebas, jueces);
                    }
                }
                break;
            case 4: // modificar
                if (*siau == NULL) {
                    printf("\nNo hay casos registrados\n");
                }
                else {
                    printf("\nIngrese el RUC del caso a modificar: ");
                    scanf(" %[^\n]", ruc);

                    caso = buscarCaso(*siau, ruc);

                    if (caso == NULL) {
                        printf("\nNo hay ningún caso con RUC: %s\n", ruc);
                    }
                    else {
                        modificarCaso(caso, jueces);
                    }
                }
                break;
            case 5: // Salir
                printf("\nSaliendo de la interfaz...\n\n");
                break;
            default:
                printf("\nOpción inválida. Intente de nuevo.\n");
        }
    } while (opcion != 5);
}

int main() {
    struct MinPublico *minPublico;
    int opcion;

    minPublico = (struct MinPublico *)malloc(sizeof(struct MinPublico));
    minPublico->fiscales = NULL;
    minPublico->siau = NULL;
    minPublico->jueces = (struct Persona **)malloc(sizeof(struct Persona *) * maxJueces);

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
                interaccionJueces(minPublico->jueces);
                break;
            case 2:
                interaccionFiscales(&minPublico->fiscales);
                break;
            case 3:
                interaccionCasos(&minPublico->siau, minPublico->fiscales, minPublico->jueces);
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
