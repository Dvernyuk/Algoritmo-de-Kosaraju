// Dmytro Vernyuk
// Grafos | Practica 5: Kosajaru

#include <iostream>
#include <fstream>
#include <queue> 
#include <vector>
#include <time.h>
#include <chrono>
#include <stack>
using namespace std;

int N_VERTICES = 5; // Numero de vertices del grafo
const int MAX_VERTICES = 5000; // Maximo de vertices que puede tener el grafo

typedef struct arista {
    int origen;
    int destino;
} t_arista;

ostream& operator<<(ostream& os, const t_arista& a) {
    os << "arista " << a.origen << " -> " << a.destino;
    return os;
}

// Añadir arista
void addArista(vector<t_arista> grafo[], int o, int d) {
    t_arista arista = { o,d };
    grafo[o].push_back(arista);
    if (o != d) // Si el origen y el destino son distintos la añadimos tanto en la lista del origen como la del destino
        grafo[d].push_back(arista);
}

int crearGrafo(vector<t_arista> grafo[]) {
    int destino, cara_cruz, nAristas;
    bool end; // Variable que permite o no la creacion de aristas
    bool asignados[MAX_VERTICES];
    queue<int> aux;

    for (int i = 0; i < N_VERTICES; i++)
        asignados[i] = false;

    srand((int)time(NULL));

    nAristas = 0;
    for (int i = 0; i < N_VERTICES; i++) {
        end = false;

        while (aux.size() < N_VERTICES && !end) { // Cada vertice no puede tener mas aristas que todos los vertices juntos
            cara_cruz = rand() % 2;
            if (cara_cruz == 1 || grafo[i].empty()) { // La primera arista siempre se pone
                nAristas++;
                
                if (grafo[i].empty()) { // Como minimo habra una arista no recursiva en todos los nodos, sea el origen o el destino
                    do { destino = rand() % N_VERTICES; } while (destino == i);
                }
                else { // Solo habra una arista con el mismo origen y direccion
                    do { destino = rand() % N_VERTICES; } while (asignados[destino] == true);
                }
                asignados[destino] = true;
                aux.push(destino);

                addArista(grafo, i, destino);
            }
            else {
                end = true;
            }
        }

        while (!aux.empty()) {
            asignados[aux.front()] = false;
            aux.pop();
        }
    }
    return nAristas;
}

void crearGrafoPersonalizado(vector<t_arista> grafo[]) {
    bool end = false;
    int origen, destino;

    cout << "Introduce el numero del vertice origen y, separado por un espacio, el vertice destino para crear aristas" << endl;
    cout << "-1 para terminar" << endl;
    while (!end) {
        cin >> origen;
        if (origen != -1) {
            cin >> destino;
            addArista(grafo, origen, destino);
        }
        else {
            end = true;
        }
    }
}

void eliminarGrafo(vector<t_arista> grafo[]) {
    for (int i = 0; i < N_VERTICES; i++) {
        grafo[i].clear();
    }
}

void printGrafo(const vector<t_arista> grafo[]) {
    for (int i = 0; i < N_VERTICES; ++i) {
        cout << "Vertice " << i << ":" << endl;
        for (const t_arista& arista : grafo[i])
            if (arista.origen == i)
                cout << "\t" << arista << endl;
        cout << endl;
    }
}

void printSCC(const vector<t_arista> grafo[], const vector<int> SCC[], int k) {
    int j;
    
    cout << "*Grafo aciclico resultante:" << endl;
    // k indica el numero total de vertices SCC y SCC[] contiene los vertices que forman cada SCC
    for (int i = 0; i < k; ++i) {
        cout << "Vertice SCC";
        for (int vertice : SCC[i])
            cout << " " << vertice;
        cout << ":" << endl;

        // Acumulamos todas las aristas en aux
        vector<t_arista> aux;
        for (int vertice : SCC[i]) {
            for (const t_arista& arista : grafo[vertice]) {
                if (arista.origen == vertice) {
                    aux.push_back(arista);
                }
            }
        }

        // Eliminamos las aristas sobrantes, las que se convertirian en recursivas al formar el SCC
        for (int vertice : SCC[i]) {
            j = 0;
            while (j < aux.size()) {
                if (aux[j].destino == vertice)
                    aux.erase(aux.begin() + j);
                else
                    j++;
            }
        }

        for (t_arista arista: aux)
            cout << "\t" << arista << endl;

        aux.clear();
        cout << endl;
    }
}

// Version normal del DFS
void DFS1(const vector<t_arista> grafo[], int i, vector<bool>& visitados, stack<int>& pila) {
    visitados[i] = true;
    for (t_arista a : grafo[i])
        if (!visitados[a.destino])
            DFS1(grafo, a.destino, visitados, pila);

    pila.push(i);
}

// Version del DFS que actua sobre el grafo como si ese fuese traspuesto
void DFS2(const vector<t_arista> grafo[], int i, vector<bool>& visitados, vector<int> SCC[], int k, bool fTiempos) {
    visitados[i] = true;
    if (!fTiempos)
        SCC[k].push_back(i);
    for (t_arista a : grafo[i])
        if (!visitados[a.origen])
            DFS2(grafo, a.origen, visitados, SCC, k, fTiempos);
}

double Kosaraju(const vector<t_arista> grafo[], vector<int> SCC[], int& k, bool fTiempos) {
    stack<int> pila;
    vector<bool> visitados1(N_VERTICES, false);
    vector<bool> visitados2(N_VERTICES, false);
    
    // Inicializacion para el calculo del tiempo
    using clock = std::chrono::system_clock;
    using sec = std::chrono::duration<double, std::milli>;
    const auto t_inicio = clock::now();

    // Parte 1 del algoritmo
    for (int i = 0; i < N_VERTICES; ++i)
        if (!visitados1[i]) {
            DFS1(grafo, i, visitados1, pila);
        }

    // Parte 2 del algoritmo
    while (!pila.empty()) {
        int curr = pila.top();
        pila.pop();
        if (visitados2[curr] == false) {
            // k indica el numero total de vertices SCC y SCC[] contiene los vertices que forman cada SCC
            DFS2(grafo, curr, visitados2, SCC, k++, fTiempos);
        }
    }
    
    const sec t_final = clock::now() - t_inicio; // Calculo del tiempo transcurrido
    
    visitados1.clear();
    visitados2.clear();

    return t_final.count();
}

void tiempos(vector<t_arista> grafo[]) {
    int nv = N_VERTICES;
    double aux = 0, tot;
    int j, k, condicion;
    vector<int> SCC[MAX_VERTICES];

    ofstream out("tiempos.txt");

    // Vamos aumentanto el tamaño de la estructura en cada iteracion
    for (N_VERTICES = 5; N_VERTICES <= MAX_VERTICES; N_VERTICES += 5) { 
        cout << N_VERTICES << endl;
        tot = 0;
        j = 0;
        // Buscamos las tres primeras soluciones para hacer la media
        condicion = 3;
        while (j < condicion) { 
            eliminarGrafo(grafo);
            crearGrafo(grafo);

            k = 0;
            aux = Kosaraju(grafo, SCC, k, true);
            tot += aux;
            j++;

            // Si la media de las 3 primeras da menor que 20ms, entonces aumentamos las iteraciones
            if (j == 3) {
                aux = tot / j;
                if (aux < 20.0)
                    condicion = 100;
            }
        }

        tot /= j;
        out << N_VERTICES << " " << tot << endl;
    }

    out.close();
    N_VERTICES = nv;
}

// El menu de la aplicacion
int main() {
    bool menu = true;
    int opt;
    vector<t_arista> grafo[MAX_VERTICES];
    vector<int> SCC[MAX_VERTICES];
    int k = 0;
    double tiempo;
    
    cout << "#" << "Aristas generadas " << crearGrafo(grafo)  << endl << endl;
    cout << "*Grafo aleatorio resultante:" << endl;
    printGrafo(grafo);

    while (menu) {
        cout << "1 ~> Ejecutar Kosaraju" << endl;
        cout << "2 ~> Crear otro grafo aleatorio" << endl;
        cout << "3 ~> Crear un grafo personalizado" << endl;
        cout << "4 ~> Ver el grafo actual" << endl;
        cout << "5 ~> Cambiar N_VERTICES, actual igual a " << N_VERTICES << endl;
        cout << "9 ~> Generar tiempos.txt (para el grafico)" << endl;
        cout << "0 ~> Salir" << endl;
        cin >> opt;
        cout << endl;

        switch (opt) {
        case 1:
            tiempo = Kosaraju(grafo, SCC, k, false);
            printSCC(grafo, SCC, k);
            cout << "#Tiempo total " << tiempo << " ms" << endl << endl;

            for (int i = 0; i < k; i++) {
                SCC[i].clear();
            }
            k = 0;
            break;
        case 2:
            eliminarGrafo(grafo);
            cout << "#" << "Aristas generadas " << crearGrafo(grafo) << endl << endl;
            if (N_VERTICES <= 10) {
                cout << "*Grafo aleatorio resultante:" << endl;
                printGrafo(grafo);
            }
            break;
        case 3:
            eliminarGrafo(grafo);
            crearGrafoPersonalizado(grafo);
            cout << endl << "*Grafo personalizado resultante:" << endl;
            printGrafo(grafo);
            break;
        case 4:
            cout << "*Grafo actual:" << endl;
            printGrafo(grafo);
            break;
        case 5:
            int x;
            cout << "2 <= n <= 5000" << endl << "n = ";
            cin >> x;
            cout << endl;
            if (x > 1 && x <= 5000) {
                N_VERTICES = x;
                eliminarGrafo(grafo);
                cout << "#" << "Aristas generadas " << crearGrafo(grafo) << endl << endl;
                if (N_VERTICES <= 10) {
                    cout << "*Grafo aleatorio resultante:" << endl;
                    printGrafo(grafo);
                }
                break;
            }
            
            break;
        case 9:
            tiempos(grafo);
            cout << "Terminado!" << endl << endl;
            break; 
        case 0:
            menu = false;
        }
    }

    for (int i = 0; i < N_VERTICES; i++) {
        grafo[i].clear();
    }

    return 0;
}