#include<iostream>
#include<fstream>
#include<cstring>
#include<cstdlib>
#include<clocale>
#include<limits>
using namespace std;

const int MEMORIA_TOTAL = 8192;  
const int MAX_NOMBRE = 30;

struct Proceso
{
    int id;
    char nombre[MAX_NOMBRE];
    int prioridad;
    char estado[12];   
    int memoria;       
};

struct NodoLista { Proceso dato; NodoLista* sig; };
struct NodoCola  { Proceso dato; NodoCola* sig; };
struct NodoPila  { int idProceso; int kb; NodoPila* sig; };


int leerEntero(const char* mensaje, int minimo, int maximo)
{
    int valor;
    bool correcto;
    do
    {
        cout<<mensaje; cin>>valor;
        if(cin.eof()) exit(0);
        if(cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            correcto = false;
            cout<<"Error, debe ingresar un numero entero \n";
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            correcto = (valor >= minimo && valor <= maximo);
            if(!correcto)
            {
                cout<<"Error, el valor debe estar entre "<<minimo<<" y "<<maximo<<" \n";
            }
        }
    } while(!correcto);
    return valor;
}

void leerNombre(const char* mensaje, char destino[])
{
    bool correcto;
    do
    {
        cout<<mensaje;
        cin.getline(destino, MAX_NOMBRE);
        if(cin.eof()) exit(0);
        if(cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            correcto = false;
        }
        else
        {
            correcto = (strlen(destino) > 0);
        }
        if(!correcto)
        {
            cout<<"Error, el nombre no puede estar vacio ni superar 29 caracteres \n";
        }
    } while(!correcto);
}

bool igualesSinCaso(const char a[], const char b[])
{
    int i;
    if(strlen(a) != strlen(b)) return false;
    for(i=0; a[i]!='\0'; i++)
    {
        char x = a[i], y = b[i];
        if(x>='A' && x<='Z') x = x - 'A' + 'a';
        if(y>='A' && y<='Z') y = y - 'A' + 'a';
        if(x != y) return false;
    }
    return true;
}

NodoLista* buscarPorId(NodoLista* cabeza, int id)
{
    NodoLista* aux = cabeza;
    while(aux != NULL)
    {
        if(aux->dato.id == id) return aux;
        aux = aux->sig;
    }
    return NULL;
}

NodoLista* buscarPorNombre(NodoLista* cabeza, const char nombre[])
{
    NodoLista* aux = cabeza;
    while(aux != NULL)
    {
        if(igualesSinCaso(aux->dato.nombre, nombre)) return aux;
        aux = aux->sig;
    }
    return NULL;
}

void insertarLista(NodoLista*& cabeza, Proceso p)
{
    NodoLista* nuevo = new NodoLista;
    NodoLista* aux;
    nuevo->dato = p;
    nuevo->sig = NULL;
    if(cabeza == NULL)
    {
        cabeza = nuevo;
    }
    else
    {
        aux = cabeza;
        while(aux->sig != NULL) aux = aux->sig;
        aux->sig = nuevo;
    }
}

bool eliminarLista(NodoLista*& cabeza, int id)
{
    NodoLista* aux;
    NodoLista* borrar;
    if(cabeza == NULL) return false;
    if(cabeza->dato.id == id)          
    {
        borrar = cabeza;
        cabeza = cabeza->sig;
        delete borrar;
        return true;
    }
    aux = cabeza;
    while(aux->sig != NULL && aux->sig->dato.id != id) aux = aux->sig;
    if(aux->sig == NULL) return false; 
    borrar = aux->sig;
    aux->sig = borrar->sig;
    delete borrar;
    return true;
}

void mostrarProceso(Proceso p)
{
    cout<<"ID: "<<p.id<<" | Nombre: "<<p.nombre<<" | Prioridad: "<<p.prioridad
        <<" | Memoria: "<<p.memoria<<" KB | Estado: "<<p.estado<<"\n";
}

void listarProcesos(NodoLista* cabeza)
{
    int total = 0;
    NodoLista* aux = cabeza;
    if(cabeza == NULL)
    {
        cout<<"No hay procesos registrados. \n";
        return;
    }
    while(aux != NULL)
    {
        mostrarProceso(aux->dato);
        total++;
        aux = aux->sig;
    }
    cout<<"Total: "<<total<<" proceso(s) \n";
}

void liberarLista(NodoLista*& cabeza)
{
    NodoLista* borrar;
    while(cabeza != NULL)
    {
        borrar = cabeza;
        cabeza = cabeza->sig;
        delete borrar;
    }
}

bool estaEnCola(NodoCola* frente, int id)
{
    NodoCola* aux = frente;
    while(aux != NULL)
    {
        if(aux->dato.id == id) return true;
        aux = aux->sig;
    }
    return false;
}

void encolar(NodoCola*& frente, NodoCola*& fin, Proceso p)
{
    NodoCola* nuevo = new NodoCola;
    NodoCola* aux;
    nuevo->dato = p;
    nuevo->sig = NULL;

    if(frente == NULL)                           
    {
        frente = nuevo;
        fin = nuevo;
    }
    else if(p.prioridad < frente->dato.prioridad) 
    {
        nuevo->sig = frente;
        frente = nuevo;
    }
    else                                        
    {
        aux = frente;
        while(aux->sig != NULL && aux->sig->dato.prioridad <= p.prioridad)
        {
            aux = aux->sig;
        }
        nuevo->sig = aux->sig;
        aux->sig = nuevo;
        if(nuevo->sig == NULL) fin = nuevo;
    }
}

bool desencolar(NodoCola*& frente, NodoCola*& fin, Proceso& p)
{
    NodoCola* borrar;
    if(frente == NULL) return false;
    borrar = frente;
    p = borrar->dato;
    frente = frente->sig;
    if(frente == NULL) fin = NULL;              
    delete borrar;
    return true;
}

bool quitarDeCola(NodoCola*& frente, NodoCola*& fin, int id)
{
    NodoCola* aux;
    NodoCola* borrar;
    if(frente == NULL) return false;
    if(frente->dato.id == id)
    {
        borrar = frente;
        frente = frente->sig;
        if(frente == NULL) fin = NULL;
    }
    else
    {
        aux = frente;
        while(aux->sig != NULL && aux->sig->dato.id != id) aux = aux->sig;
        if(aux->sig == NULL) return false;
        borrar = aux->sig;
        aux->sig = borrar->sig;
        if(borrar == fin) fin = aux;
    }
    delete borrar;
    return true;
}

void mostrarCola(NodoCola* frente)
{
    int pos = 1;
    NodoCola* aux = frente;
    if(frente == NULL)
    {
        cout<<"La cola esta vacia. \n";
        return;
    }
    while(aux != NULL)
    {
        cout<<pos<<". ID "<<aux->dato.id<<" | "<<aux->dato.nombre
            <<" | Prioridad "<<aux->dato.prioridad<<"\n";
        pos++;
        aux = aux->sig;
    }
}

void liberarCola(NodoCola*& frente, NodoCola*& fin)
{
    NodoCola* borrar;
    while(frente != NULL)
    {
        borrar = frente;
        frente = frente->sig;
        delete borrar;
    }
    fin = NULL;
}

void push(NodoPila*& tope, int idProceso, int kb)
{
    NodoPila* nuevo = new NodoPila;
    nuevo->idProceso = idProceso;
    nuevo->kb = kb;
    nuevo->sig = tope;
    tope = nuevo;
}

bool pop(NodoPila*& tope, int& idProceso, int& kb)
{
    NodoPila* borrar;
    if(tope == NULL) return false;
    borrar = tope;
    idProceso = borrar->idProceso;
    kb = borrar->kb;
    tope = tope->sig;
    delete borrar;
    return true;
}

bool estaEnPila(NodoPila* tope, int id)
{
    NodoPila* aux = tope;
    while(aux != NULL)
    {
        if(aux->idProceso == id) return true;
        aux = aux->sig;
    }
    return false;
}

int memoriaUsada(NodoPila* tope)
{
    int total = 0;
    NodoPila* aux = tope;
    while(aux != NULL)
    {
        total = total + aux->kb;
        aux = aux->sig;
    }
    return total;
}

void mostrarMemoria(NodoPila* tope)
{
    int usada = memoriaUsada(tope);
    NodoPila* aux = tope;
    cout<<"Memoria total: "<<MEMORIA_TOTAL<<" KB \n";
    cout<<"Memoria usada: "<<usada<<" KB \n";
    cout<<"Memoria libre: "<<MEMORIA_TOTAL - usada<<" KB \n";
    if(tope == NULL)
    {
        cout<<"No hay bloques asignados. \n";
        return;
    }
    cout<<"Bloques (del tope hacia la base): \n";
    while(aux != NULL)
    {
        if(aux == tope) cout<<"[TOPE] ";
        else cout<<"       ";
        cout<<"Proceso ID "<<aux->idProceso<<" -> "<<aux->kb<<" KB \n";
        aux = aux->sig;
    }
}

void liberarPila(NodoPila*& tope)
{
    NodoPila* borrar;
    while(tope != NULL)
    {
        borrar = tope;
        tope = tope->sig;
        delete borrar;
    }
}


void guardarPilaRec(NodoPila* nodo, ofstream& f)
{
    if(nodo == NULL) return;
    guardarPilaRec(nodo->sig, f);
    f<<nodo->idProceso<<" "<<nodo->kb<<"\n";
}

bool guardarDatos(NodoLista* cabeza, NodoCola* frente, NodoPila* tope)
{
    NodoLista* a;
    NodoCola* c;
    ofstream fp("procesos.txt");
    ofstream fc("cola.txt");
    ofstream fm("memoria.txt");
    if(!fp || !fc || !fm) return false;

    for(a=cabeza; a!=NULL; a=a->sig)
    {
        fp<<a->dato.id<<" "<<a->dato.prioridad<<" "<<a->dato.memoria<<" "
          <<a->dato.estado<<" "<<a->dato.nombre<<"\n";
    }
    for(c=frente; c!=NULL; c=c->sig)
    {
        fc<<c->dato.id<<"\n";
    }
    guardarPilaRec(tope, fm);
    return true;
}

bool cargarDatos(NodoLista*& cabeza, NodoCola*& frente, NodoCola*& fin, NodoPila*& tope)
{
    Proceso p;
    NodoLista* n;
    int id, kb;
    ifstream fp("procesos.txt");
    if(!fp) return false;           

    liberarLista(cabeza);           
    liberarCola(frente, fin);
    liberarPila(tope);

    while(fp>>p.id>>p.prioridad>>p.memoria>>p.estado)
    {
        fp.ignore(1);
        fp.getline(p.nombre, MAX_NOMBRE);
        if(p.id > 0 && buscarPorId(cabeza, p.id) == NULL) insertarLista(cabeza, p);
    }

    ifstream fc("cola.txt");
    while(fc>>id)
    {
        n = buscarPorId(cabeza, id);
        if(n != NULL && !estaEnCola(frente, id)) encolar(frente, fin, n->dato);
    }

    ifstream fm("memoria.txt");
    while(fm>>id>>kb)
    {
        push(tope, id, kb);
    }
    return true;
}

void registrarProceso(NodoLista*& cabeza)
{
    Proceso p;
    cout<<"\n=== REGISTRAR PROCESO === \n";
    p.id = leerEntero("Ingrese ID (1-9999): ", 1, 9999);
    if(buscarPorId(cabeza, p.id) != NULL)
    {
        cout<<"Error, ya existe un proceso con ese ID \n";
        return;
    }
    leerNombre("Ingrese nombre del proceso: ", p.nombre);
    p.prioridad = leerEntero("Ingrese prioridad (1 = mas urgente ... 10): ", 1, 10);
    p.memoria = leerEntero("Ingrese memoria requerida en KB (1-4096): ", 1, 4096);
    strcpy(p.estado, "Nuevo");
    insertarLista(cabeza, p);
    cout<<"Proceso registrado correctamente \n";
}

void eliminarProceso(NodoLista*& cabeza, NodoCola*& frente, NodoCola*& fin, NodoPila* tope)
{
    int id;
    cout<<"\n=== ELIMINAR PROCESO === \n";
    id = leerEntero("Ingrese ID del proceso a eliminar: ", 1, 9999);
    if(buscarPorId(cabeza, id) == NULL)
    {
        cout<<"Proceso no existe \n";
    }
    else if(estaEnPila(tope, id))
    {
        cout<<"Error, el proceso tiene memoria asignada. Libere su bloque (pop) primero \n";
    }
    else
    {
        quitarDeCola(frente, fin, id);   
        eliminarLista(cabeza, id);
        cout<<"Proceso eliminado correctamente \n";
    }
}

void buscarProceso(NodoLista* cabeza)
{
    int op, id;
    char nombre[MAX_NOMBRE];
    NodoLista* n;
    cout<<"\n=== BUSCAR PROCESO === \n";
    cout<<"1. Por ID \n";
    cout<<"2. Por nombre \n";
    op = leerEntero("Ingrese opcion: ", 1, 2);
    if(op == 1)
    {
        id = leerEntero("Ingrese ID a buscar: ", 1, 9999);
        n = buscarPorId(cabeza, id);
    }
    else
    {
        leerNombre("Ingrese nombre a buscar: ", nombre);
        n = buscarPorNombre(cabeza, nombre);
    }
    if(n == NULL)
    {
        cout<<"Proceso no existe \n";
    }
    else
    {
        cout<<"\nProceso Encontrado: \n";
        mostrarProceso(n->dato);
    }
}

void modificarPrioridad(NodoLista* cabeza, NodoCola*& frente, NodoCola*& fin)
{
    int id;
    NodoLista* n;
    cout<<"\n=== MODIFICAR PRIORIDAD === \n";
    id = leerEntero("Ingrese ID del proceso: ", 1, 9999);
    n = buscarPorId(cabeza, id);
    if(n == NULL)
    {
        cout<<"Proceso no existe \n";
        return;
    }
    cout<<"Prioridad actual: "<<n->dato.prioridad<<"\n";
    n->dato.prioridad = leerEntero("Ingrese nueva prioridad (1-10): ", 1, 10);
    if(estaEnCola(frente, id))           
    {
        quitarDeCola(frente, fin, id);
        encolar(frente, fin, n->dato);
    }
    cout<<"Prioridad actualizada con exito \n";
}

void menuProcesos(NodoLista*& cabeza, NodoCola*& frente, NodoCola*& fin, NodoPila* tope)
{
    int op;
    do
    {
        cout<<"::::: MODULO PROCESOS :::::\n";
        cout<<"1. Registrar proceso \n";
        cout<<"2. Eliminar proceso \n";
        cout<<"3. Buscar proceso \n";
        cout<<"4. Modificar prioridad \n";
        cout<<"5. Listar procesos \n";
        cout<<"0. Volver \n";
        cout<<"-----------------------------------\n";
        op = leerEntero("Ingrese opcion: ", 0, 5);

        switch(op)
        {
            case 1: registrarProceso(cabeza); break;
            case 2: eliminarProceso(cabeza, frente, fin, tope); break;
            case 3: buscarProceso(cabeza); break;
            case 4: modificarPrioridad(cabeza, frente, fin); break;
            case 5:
                cout<<"\n***** Lista de Procesos ***** \n";
                listarProcesos(cabeza);
                break;
            case 0: break;
        }
        if(op != 0) system("pause");
        system("cls");
    } while(op != 0);
}

void encolarProceso(NodoLista* cabeza, NodoCola*& frente, NodoCola*& fin)
{
    int id;
    NodoLista* n;
    cout<<"\n=== ENCOLAR PROCESO === \n";
    id = leerEntero("Ingrese ID del proceso: ", 1, 9999);
    n = buscarPorId(cabeza, id);
    if(n == NULL)
    {
        cout<<"Proceso no encontrado \n";
    }
    else if(estaEnCola(frente, id))
    {
        cout<<"El proceso ya esta en la cola \n";
    }
    else
    {
        strcpy(n->dato.estado, "Listo");
        encolar(frente, fin, n->dato);
        cout<<"Proceso encolado (estado: Listo) \n";
    }
}

void ejecutarSiguiente(NodoLista* cabeza, NodoCola*& frente, NodoCola*& fin)
{
    Proceso p;
    NodoLista* n;
    cout<<"\n=== EJECUTAR SIGUIENTE === \n";
    if(!desencolar(frente, fin, p))
    {
        cout<<"No hay procesos en la cola \n";
        return;
    }
    n = buscarPorId(cabeza, p.id);
    if(n != NULL) strcpy(n->dato.estado, "Ejecutado");
    cout<<"Ejecutando proceso "<<p.id<<" - "<<p.nombre<<" (prioridad "<<p.prioridad<<") \n";
    cout<<"Estado actualizado a: Ejecutado \n";
}

void menuCPU(NodoLista* cabeza, NodoCola*& frente, NodoCola*& fin)
{
    int op;
    do
    {
        cout<<"::::: MODULO CPU (PLANIFICADOR) :::::\n";
        cout<<"1. Encolar proceso \n";
        cout<<"2. Ejecutar siguiente \n";
        cout<<"3. Mostrar cola \n";
        cout<<"0. Volver \n";
        cout<<"-----------------------------------\n";
        op = leerEntero("Ingrese opcion: ", 0, 3);

        switch(op)
        {
            case 1: encolarProceso(cabeza, frente, fin); break;
            case 2: ejecutarSiguiente(cabeza, frente, fin); break;
            case 3:
                cout<<"\n***** Cola Actual ***** \n";
                mostrarCola(frente);
                break;
            case 0: break;
        }
        if(op != 0) system("pause");
        system("cls");
    } while(op != 0);
}

void asignarMemoria(NodoLista* cabeza, NodoPila*& tope)
{
    int id, libre;
    NodoLista* n;
    cout<<"\n=== ASIGNAR MEMORIA (PUSH) === \n";
    id = leerEntero("Ingrese ID del proceso: ", 1, 9999);
    n = buscarPorId(cabeza, id);
    if(n == NULL)
    {
        cout<<"Proceso no encontrado \n";
        return;
    }
    if(estaEnPila(tope, id))
    {
        cout<<"El proceso ya tiene un bloque asignado \n";
        return;
    }
    libre = MEMORIA_TOTAL - memoriaUsada(tope);
    if(n->dato.memoria > libre)
    {
        cout<<"Memoria insuficiente: requiere "<<n->dato.memoria
            <<" KB y solo hay "<<libre<<" KB libres \n";
        return;
    }
    push(tope, id, n->dato.memoria);
    cout<<"Se asignaron "<<n->dato.memoria<<" KB al proceso "<<id<<"\n";
}

void liberarMemoria(NodoPila*& tope)
{
    int id, kb;
    cout<<"\n=== LIBERAR MEMORIA (POP) === \n";
    if(!pop(tope, id, kb))
    {
        cout<<"No hay bloques de memoria para liberar \n";
    }
    else
    {
        cout<<"Se libero el bloque de "<<kb<<" KB del proceso "<<id<<"\n";
    }
}

void menuMemoria(NodoLista* cabeza, NodoPila*& tope)
{
    int op;
    do
    {
        cout<<"::::: MODULO MEMORIA :::::\n";
        cout<<"1. Asignar memoria (push) \n";
        cout<<"2. Liberar ultimo bloque (pop) \n";
        cout<<"3. Estado de la memoria \n";
        cout<<"0. Volver \n";
        cout<<"-----------------------------------\n";
        op = leerEntero("Ingrese opcion: ", 0, 3);

        switch(op)
        {
            case 1: asignarMemoria(cabeza, tope); break;
            case 2: liberarMemoria(tope); break;
            case 3:
                cout<<"\n***** Estado de la Memoria ***** \n";
                mostrarMemoria(tope);
                break;
            case 0: break;
        }
        if(op != 0) system("pause");
        system("cls");
    } while(op != 0);
}

void menu()
{
    NodoLista* cabeza = NULL;   
    NodoCola* frente = NULL;    
    NodoCola* fin = NULL;
    NodoPila* tope = NULL;      
    int op;

    cout<<"Cargando datos desde archivos... \n";
    if(cargarDatos(cabeza, frente, fin, tope)) cout<<"Datos cargados correctamente \n";
    else cout<<"No hay datos previos, se inicia con el sistema vacio \n";
    system("pause");
    system("cls");

    do
    {
        cout<<"::::: GESTOR DE PROCESOS :::::\n";
        cout<<"1. Procesos (registrar, eliminar, buscar, modificar, listar) \n";
        cout<<"2. CPU (encolar, ejecutar, mostrar cola) \n";
        cout<<"3. Memoria (asignar, liberar, estado) \n";
        cout<<"4. Guardar datos \n";
        cout<<"5. Cargar datos \n";
        cout<<"0. Salir \n";
        cout<<"-----------------------------------\n";
        op = leerEntero("Ingrese opcion: ", 0, 5);

        switch(op)
        {
            case 1: system("cls"); menuProcesos(cabeza, frente, fin, tope); break;
            case 2: system("cls"); menuCPU(cabeza, frente, fin); break;
            case 3: system("cls"); menuMemoria(cabeza, tope); break;
            case 4:
                if(guardarDatos(cabeza, frente, tope)) cout<<"Datos guardados correctamente \n";
                else cout<<"Error, no se pudieron guardar los datos \n";
                system("pause");
                break;
            case 5:
                if(cargarDatos(cabeza, frente, fin, tope)) cout<<"Datos cargados correctamente \n";
                else cout<<"No se encontraron archivos para cargar \n";
                system("pause");
                break;
            case 0:
                cout<<"Guardando datos en los archivos... \n";
                guardarDatos(cabeza, frente, tope);
                cout<<"Liberando la memoria dinamica... \n";
                liberarLista(cabeza);
                liberarCola(frente, fin);
                liberarPila(tope);
                cout<<"Fin del programa \n";
                break;
        }
        if(op != 0) system("cls");
    } while(op != 0);
}

int main()
{
    setlocale(LC_CTYPE, "Spanish");
    menu();
    return 0;
}
