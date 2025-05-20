#include <stdio.h>
#include <stdlib.h>

#define CANTCATEGORIASPRUEBAS 4 // declaraciones, informes, grabaciones, documentos, evidencias

struct NodoJuez;
struct NodoFiscal;
struct NodoPrueba;
struct NodoImplicado;
struct NodoCaso;

struct Prueba {
    char *categoria;
    char *data;
};

struct NodoPrueba { // doble enlazada
    struct Prueba *prueba;
    struct NodoPrueba *sig, *ant;
};

struct Implicado {
    char *rol;
    char *rut;
    char *nombre;
    struct Fiscal *fical;
};

struct NodoImplicado { // simple enlazada
    struct Implicado *implicado;
    struct NodoImplicado *sig;
};

struct Caso {
    char *ruc;
    struct NodoPrueba *categoriasPruebas; // doble enlazada
    struct NodoImplicado *implicados; // simple enlazada
};

struct Fiscal {
    char *rut;
    char *nombre;
    struct NodoJuez *jueces; // circular simple
    struct Caso *casosActivos;
    int cantCasosActivos;
};

struct NodoFiscal { // circular simple
    struct Fiscal *fiscal;
    struct NodoFiscal *sig;
};

struct Juez {
    char *rut;
    char *nombre;
    struct NodoFiscal *fiscales;
};

struct NodoJuez { // circular simple
    struct Juez *juez;
    struct NodoJuez *sig;
};

struct NodoSIAU {
    struct Caso *caso;
    struct NodoSIAU *izq, *der;
};

struct MinPublico {
    struct NodoJuez *jueces;       // circular simple
    struct NodoFiscal *fiscales;   // circular simple
};

void mostrarImplicado(struct Implicado *implicado) {
    printf("rol: %s\n", implicado->rol);
    printf("nombre: %s\n", implicado->nombre);
    printf("rut: %s\n", implicado->rut);
    printf("Fiscal a cargo: %s\n", implicado->fical->nombre);
    printf("Rut fiscal a cargo: %s\n\n", implicado->fical->rut);
}

void mostrarListaImplicados(struct NodoImplicado *implicados) {
    while (implicados != NULL) {
        mostrarImplicado(implicados->implicado);

        implicados = implicados->sig;
    }
}

int main() {

    return 0;
}
