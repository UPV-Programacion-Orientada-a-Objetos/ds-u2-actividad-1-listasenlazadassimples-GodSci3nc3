#include <cstdio>
#include <cstdlib>
#include <cstring>

template<typename T>
struct Nodo {
    T value;
    Nodo<T>* next;
    Nodo(const T& v): value(v), next(nullptr) {}
};

template<typename T>
class ListaSensor {
    Nodo<T>* head;
public:
    ListaSensor(): head(nullptr) {}
    ~ListaSensor(){ clear(); }
    void push_back(T v){
        Nodo<T>* n = new Nodo<T>(v);
        if(!head){ head = n; return; }
        Nodo<T>* p = head;
        while(p->next) p = p->next;
        p->next = n;
    }
    bool empty() const { return head == nullptr; }
    void clear(){
        while(head){ Nodo<T>* t = head; head = head->next; delete t; }
    }
    bool remove_min(){
        if(!head) return false;
        Nodo<T>* cur = head;
        Nodo<T>* prev = nullptr;
        Nodo<T>* min = head;
        Nodo<T>* prev_min = nullptr;
        while(cur){
            if(cur->value < min->value){ min = cur; prev_min = prev; }
            prev = cur;
            cur = cur->next;
        }
        if(prev_min == nullptr) head = min->next;
        else prev_min->next = min->next;
        delete min;
        return true;
    }
    double average() const{
        if(!head) return 0.0;
        int cnt = 0;
        double sum = 0.0;
        for(Nodo<T>* p = head; p; p = p->next){ sum += p->value; cnt++; }
        return cnt ? (sum / cnt) : 0.0;
    }
};

class SensorBase {
protected:
    char id[50];
public:
    SensorBase(const char* s){ std::strncpy(id, s, 49); id[49]=0; }
    virtual ~SensorBase() {}
    virtual void procesarLectura() = 0;
    virtual void addLectura(const char* val) = 0;
    virtual void imprimirInfo() const = 0;
    const char* getId() const { return id; }
};

class SensorTemperatura : public SensorBase {
    ListaSensor<float> historial;
public:
    SensorTemperatura(const char* s): SensorBase(s) {}
    void addLectura(const char* val){ float v = std::atof(val); historial.push_back(v); }
    void procesarLectura(){
        if(historial.empty()){ std::printf("%s: sin lecturas\n", id); return; }
        historial.remove_min();
        std::printf("%s: min eliminado\n", id);
    }
    void imprimirInfo() const { std::printf("%s: temp\n", id); }
    ~SensorTemperatura(){}
};

class SensorPresion : public SensorBase {
    ListaSensor<int> historial;
public:
    SensorPresion(const char* s): SensorBase(s) {}
    void addLectura(const char* val){ int v = std::atoi(val); historial.push_back(v); }
    void procesarLectura(){
        if(historial.empty()){ std::printf("%s: sin lecturas\n", id); return; }
        double avg = historial.average();
        std::printf("%s: promedio %.2f\n", id, avg);
    }
    void imprimirInfo() const { std::printf("%s: pres\n", id); }
    ~SensorPresion(){}
};

struct SensorNode { SensorBase* s; SensorNode* next; SensorNode(SensorBase* ss): s(ss), next(nullptr) {} };

class Manager {
    SensorNode* head;
public:
    Manager(): head(nullptr) {}
    ~Manager(){
        while(head){ SensorNode* t = head; head = head->next; delete t->s; delete t; }
    }
    void addSensor(SensorBase* s){
        SensorNode* n = new SensorNode(s);
        n->next = head; head = n;
    }
    SensorBase* find(const char* id){
        for(SensorNode* p = head; p; p = p->next) if(std::strcmp(p->s->getId(), id) == 0) return p->s;
        return nullptr;
    }
    void procesarTodos(){ for(SensorNode* p = head; p; p = p->next) p->s->procesarLectura(); }
};

int main(){
    Manager mgr;
    char cmd[20];
    std::printf("monitor sencillo\n");
    std::printf("comandos: agregartemp agregarpres leer procesar salir\n");
    while(std::scanf("%19s", cmd) == 1){
        if(std::strcmp(cmd, "agregartemp") == 0){
            char id[50]; if(std::scanf("%49s", id) != 1) continue;
            if(mgr.find(id)){ std::printf("%s: ya existe\n", id); continue; }
            mgr.addSensor(new SensorTemperatura(id));
            std::printf("%s: creado\n", id);
    } else if(std::strcmp(cmd, "agregarpres") == 0){
            char id[50]; if(std::scanf("%49s", id) != 1) continue;
            if(mgr.find(id)){ std::printf("%s: ya existe\n", id); continue; }
            mgr.addSensor(new SensorPresion(id));
            std::printf("%s: creado\n", id);
    } else if(std::strcmp(cmd, "leer") == 0){
            char id[50]; char val[50]; if(std::scanf("%49s %49s", id, val) != 2) continue;
            SensorBase* s = mgr.find(id);
            if(!s){ std::printf("%s: no encontrado\n", id); continue; }
            s->addLectura(val);
            std::printf("%s: agregado\n", id);
        } else if(std::strcmp(cmd, "procesar") == 0){
            std::printf("procesando\n"); mgr.procesarTodos();
        } else if(std::strcmp(cmd, "salir") == 0){ break; }
        else { std::printf("desconocido\n"); }
    }
    std::printf("adios\n");
    return 0;
}
