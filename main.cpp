#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/types.h>

/** Nodo para lista enlazada simple (genérico) */
template<typename T>
struct Nodo {
    T value;
    Nodo<T>* next;
    Nodo(const T& v): value(v), next(nullptr) {}
};

/** Lista enlazada simple genérica para lecturas */
template<typename T>
class ListaSensor {
    Nodo<T>* head;
public:
    ListaSensor(): head(nullptr) {}
    ~ListaSensor(){ clear(); }
    ListaSensor(const ListaSensor& other): head(nullptr) {
        for(Nodo<T>* p = other.head; p; p = p->next) push_back(p->value);
    }
    ListaSensor& operator=(const ListaSensor& other){
        if(this == &other) return *this;
        clear();
        for(Nodo<T>* p = other.head; p; p = p->next) push_back(p->value);
        return *this;
    }
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
    /** Eliminar el nodo con el valor mínimo */
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
    /** Promedio de valores (0 si vacía) */
    double average() const{
        if(!head) return 0.0;
        int cnt = 0;
        double sum = 0.0;
        for(Nodo<T>* p = head; p; p = p->next){ sum += p->value; cnt++; }
        return cnt ? (sum / cnt) : 0.0;
    }
};

/** Clase base para sensores (interfaz) */
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

/** Sensor de temperatura (lecturas float) */
class SensorTemperatura : public SensorBase {
    ListaSensor<float> historial;
public:
    SensorTemperatura(const char* s): SensorBase(s) {}
    void addLectura(const char* val){ float v = std::atof(val); historial.push_back(v); }
    /** Procesar: eliminar mínimo */
    void procesarLectura(){
        if(historial.empty()){ std::printf("%s: sin lecturas\n", id); return; }
        historial.remove_min();
        std::printf("%s: min eliminado\n", id);
    }
    void imprimirInfo() const { std::printf("%s: temp\n", id); }
    ~SensorTemperatura(){}
};

/** Sensor de presión (lecturas int) */
class SensorPresion : public SensorBase {
    ListaSensor<int> historial;
public:
    SensorPresion(const char* s): SensorBase(s) {}
    void addLectura(const char* val){ int v = std::atoi(val); historial.push_back(v); }
    /** Procesar: calcular promedio */
    void procesarLectura(){
        if(historial.empty()){ std::printf("%s: sin lecturas\n", id); return; }
        double avg = historial.average();
        std::printf("%s: promedio %.2f\n", id, avg);
    }
    void imprimirInfo() const { std::printf("%s: pres\n", id); }
    ~SensorPresion(){}
};

class SensorVibracion : public SensorBase {
    ListaSensor<int> historial;
public:
    SensorVibracion(const char* s): SensorBase(s) {}
    void addLectura(const char* val){ int v = std::atoi(val); historial.push_back(v); }
    void procesarLectura(){
        if(historial.empty()){ std::printf("%s: sin lecturas\n", id); return; }
        double avg = historial.average();
        std::printf("%s: promedio %.2f\n", id, avg);
    }
    void imprimirInfo() const { std::printf("%s: vibr\n", id); }
    ~SensorVibracion(){}
};

struct SensorNode { SensorBase* s; SensorNode* next; SensorNode(SensorBase* ss): s(ss), next(nullptr) {} };

/** Manager que guarda sensores y los procesa */
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
    void imprimirTodos() const{ for(SensorNode* p = head; p; p = p->next) p->s->imprimirInfo(); }
    void registrarPorTipo(const char* tipo, const char* id, const char* val){
        SensorBase* s = find(id);
        if(!s){
            if(std::strcmp(tipo, "TEMP") == 0) { s = new SensorTemperatura(id); addSensor(s); }
            else if(std::strcmp(tipo, "PRES") == 0) { s = new SensorPresion(id); addSensor(s); }
            else if(std::strcmp(tipo, "VIBR") == 0) { s = new SensorVibracion(id); addSensor(s); }
            else { return; }
        }
        s->addLectura(val);
    }
    void procesarTodos(){ for(SensorNode* p = head; p; p = p->next) p->s->procesarLectura(); }
};

/**
 * @brief Programa principal
 * Lee comandos por stdin y actúa sobre los sensores
 */
static FILE* open_input_path(const char* path){
    struct stat st;
    if(stat(path, &st) == 0 && S_ISCHR(st.st_mode)){
        int fd = open(path, O_RDWR | O_NOCTTY | O_SYNC);
        if(fd < 0) return nullptr;
        struct termios tty;
        if(tcgetattr(fd, &tty) == 0){
            cfsetospeed(&tty, B9600);
            cfsetispeed(&tty, B9600);
            tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; 
            tty.c_iflag &= ~IGNBRK;
            tty.c_lflag = 0;
            tty.c_oflag = 0;
            tty.c_cc[VMIN]  = 1;
            tty.c_cc[VTIME] = 1;
            tty.c_iflag &= ~(IXON | IXOFF | IXANY);
            tty.c_cflag |= (CLOCAL | CREAD);
            tty.c_cflag &= ~(PARENB | PARODD);
            tty.c_cflag &= ~CSTOPB;
            tcsetattr(fd, TCSANOW, &tty);
        }
        return fdopen(fd, "r+");
    } else {
        return fopen(path, "r");
    }
}

static void process_line(const char* line, Manager& mgr){
    if(!line) return;
    char buf[200]; std::strncpy(buf, line, sizeof(buf)-1); buf[sizeof(buf)-1]=0;
    for(int i=(int)std::strlen(buf)-1;i>=0;i--){ if(buf[i]=='\n' || buf[i]=='\r') buf[i]=0; else break; }
    char* toks[4]; int t=0;
    char* p = std::strtok(buf, " :\t");
    while(p && t<4){ toks[t++]=p; p = std::strtok(NULL, " :\t"); }
    if(t==3){ 
        mgr.registrarPorTipo(toks[0], toks[1], toks[2]);
        std::printf("%s: ok\n", toks[1]);
    } else if(t==2){ 
        SensorBase* s = mgr.find(toks[0]);
        if(s){ s->addLectura(toks[1]); std::printf("%s: agregado\n", toks[0]); }
        else {
            if(std::strchr(toks[1], '.')){ mgr.addSensor(new SensorTemperatura(toks[0])); mgr.find(toks[0])->addLectura(toks[1]); std::printf("%s: creado y agregado\n", toks[0]); }
            else { mgr.addSensor(new SensorPresion(toks[0])); mgr.find(toks[0])->addLectura(toks[1]); std::printf("%s: creado y agregado\n", toks[0]); }
        }
    }
}

int main(int argc, char** argv){
    Manager mgr;
    char cmd[20];
    std::srand(std::time(nullptr));
    bool interactive = isatty(fileno(stdin));
    if(argc > 1){
        FILE* f = open_input_path(argv[1]);
        if(!f){ std::printf("no puedo abrir %s\n", argv[1]); return 1; }
        char line[256];
        while(fgets(line, sizeof(line), f)){
            process_line(line, mgr);
        }
        if(f != stdin) fclose(f);
        return 0;
    }
    if(interactive){
        while(true){
            int opt=0;
            std::printf("=== Sistema IoT - POO ===\n");
            std::printf("1. Crear Sensor Temperatura\n");
            std::printf("2. Crear Sensor Presion\n");
            std::printf("3. Crear Sensor Vibracion\n");
            std::printf("4. Agregar Lectura Manual\n");
            std::printf("5. Simular Arduino (5 lecturas)\n");
            std::printf("6. Procesar Sensores\n");
            std::printf("7. Mostrar Sensores\n");
            std::printf("8. Salir\n");
            std::printf("Opcion: ");
            if(std::scanf("%d", &opt) != 1) break;
            if(opt == 1){ char id[50]; std::printf("ID: "); if(std::scanf("%49s", id) != 1) continue; if(mgr.find(id)){ std::printf("%s: ya existe\n", id); continue; } mgr.addSensor(new SensorTemperatura(id)); std::printf("%s: creado\n", id); }
            else if(opt == 2){ char id[50]; std::printf("ID: "); if(std::scanf("%49s", id) != 1) continue; if(mgr.find(id)){ std::printf("%s: ya existe\n", id); continue; } mgr.addSensor(new SensorPresion(id)); std::printf("%s: creado\n", id); }
            else if(opt == 3){ char id[50]; std::printf("ID: "); if(std::scanf("%49s", id) != 1) continue; if(mgr.find(id)){ std::printf("%s: ya existe\n", id); continue; } mgr.addSensor(new SensorVibracion(id)); std::printf("%s: creado\n", id); }
            else if(opt == 4){ char id[50], val[50]; std::printf("ID VALOR: "); if(std::scanf("%49s %49s", id, val) != 2) continue; SensorBase* s = mgr.find(id); if(!s){ std::printf("%s: no encontrado\n", id); continue; } s->addLectura(val); std::printf("%s: agregado\n", id); }
            else if(opt == 5){ std::printf("Simulando 5 lecturas del Arduino...\n"); const char* tipos[3] = {"TEMP","PRES","VIBR"}; const char* ids[3] = {"T-001","P-105","V-201"}; for(int i=0;i<5;i++){ int t = std::rand()%3; int idn = t; char val[20]; if(t==0) std::sprintf(val, "%.1f", 20.0 + (std::rand()%150)/10.0); else std::sprintf(val, "%d", 1000 + (std::rand()%100)); std::printf("Dato Arduino: %s | %s | %s\n", tipos[t], ids[idn], val); mgr.registrarPorTipo(tipos[t], ids[idn], val); } std::printf("Simulacion completada!\n"); }
            else if(opt == 6){ std::printf("procesando\n"); mgr.procesarTodos(); }
            else if(opt == 7){ mgr.imprimirTodos(); }
            else if(opt == 8) break; else { std::printf("opcion invalida\n"); }
        }
        std::printf("adios\n");
        return 0;
    } else {
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
}
