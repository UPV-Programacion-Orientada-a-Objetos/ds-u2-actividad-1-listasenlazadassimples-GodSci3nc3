````markdown
[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/Cqloh_dN)
[![Open in Codespaces](https://classroom.github.com/assets/launch-codespace-2972f46106e565e64193e422d61a12cf1da4916b45550586e14ef0a7c637dd04.svg)](https://classroom.github.com/open-in-codespaces?assignment_repo_id=21370374)
# Caso de Estudio: Sistema de Gestión Polimórfica de Sensores para IoT (Genérico)

## Definición del Problema a Resolver (El Caso de Estudio)

Una empresa de monitoreo de Infraestructura Crítica (IC) necesita un sistema de bajo nivel para registrar, almacenar y procesar lecturas de múltiples tipos de sensores (temperatura, presión, vibración) de manera unificada.

El sistema debe superar dos grandes limitaciones de diseño:

* **Rigidez del Tipo de Dato:** Las lecturas pueden ser enteras (ej. conteo de vibraciones) o de punto flotante (ej. temperatura), por lo que el contenedor debe ser genérico.
* **Rigidez de la Estructura:** Se necesita una estructura de datos que se adapte al número variable de lecturas.

El objetivo es crear una Jerarquía de Clases Polimórfica que gestione Listas Enlazadas Simples Genéricas (`ListaSensor<T>`). El diseño debe permitir que cualquier tipo de sensor (subclase) pueda ser agregado a una única lista de gestión, forzando la implementación de métodos esenciales a través de Clases Abstractas.

---

## Temas Relacionados y Necesarios

Para resolver este caso de estudio, los alumnos deberán dominar la integración de los siguientes conceptos:

| Tema Principal         | Concepto a Aplicar                                                                                                |
| :------------------------------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| POO Avanzada: Jerarquía Polimórfica  | Clase Base Abstracta (`SensorBase`) con métodos virtuales puros. Uso de Herencia para crear sensores específicos (`SensorTemperatura`, `SensorPresion`).                                                  |
| Listas Enlazadas Simples      | Implementación de una Lista Enlazada Genérica (`ListaSensor<T>`) para almacenar las lecturas (`T`**`) de forma dinámica. Gestión de nodos (`Nodo<T>`) y punteros (*) a mano.                                           |
| Plantillas (Templates)       | Uso de `template <typename T>` para que la lista enlazada y el manejo de lecturas sean independientes del tipo de dato (`int`, `float`, `double`).                                                    |
| Punteros y Gestión de Memoria    | Uso de punteros a la clase base (`SensorBase*`) para lograr polimorfismo en la lista de gestión. Implementación de la Regla de los Tres/Cinco para evitar fugas de memoria al trabajar con punteros y nodos.                                |
| Obtención de señales desde el puerto serial (Arduino)  | Se deberá realizar la captura de señales simuladas desde un dispositivo Arduino desde el puerto serial |

---

## Definición y Detalles del Proceso a Desarrollar

### Diseño de Clases Obligatorias

* **Clase Base Abstracta (`SensorBase`):**
  * Propósito: Define la interfaz común para todos los sensores.
  * Método Virtual Puro: `virtual void procesarLectura() = 0;` y `virtual void imprimirInfo() const = 0;`.
  * Atributo: `protected char nombre[50];` (Identificador del sensor).

* **Clases Derivadas (Concretas):**
  * `SensorTemperatura : public SensorBase`: Representa un sensor que maneja lecturas de tipo `float`.
  * `SensorPresion : public SensorBase`: Representa un sensor que maneja lecturas de tipo `int`.

* **Clase de Nodo y Lista Genérica:**
  * `template <typename T> struct Nodo`: Estructura o clase que contiene un dato  `T` y un puntero `Nodo<T>* siguiente`.
  * `template <typename T> class ListaSensor`: Implementa las funciones de inserción, búsqueda y liberación de la Lista Enlazada Simple para el tipo de dato  `T`.

### Proceso de Solución Detallado

1. **Lista de Gestión Polimórfica:** El programa principal debe utilizar una Lista Enlazada Simple **NO** Genérica (ej., `ListaGeneral`) que almacene punteros a la clase base (`SensorBase*`). Esto permite guardar `SensorTemperatura*` y `SensorPresion*` juntos.

2. **Integración de la Lista Genérica:** Cada clase concreta (`SensorTemperatura`, `SensorPresion`) debe contener como atributo una instancia de su respectiva Lista Genérica.

  * `SensorTemperatura` contendrá `ListaSensor<float> historial;`
  * `SensorPresion` contendrá `ListaSensor<int> historial;`

3. **Registro de Lecturas:** Implementar un método para cada clase derivada que permita añadir una nueva lectura a su lista interna.

4. **Procesamiento Polimórfico:** La función principal del sistema debe iterar sobre la Lista de Gestión Polimórfica (que almacena `SensorBase*`) y llamar al método `procesarLectura()`.

  * Para `SensorTemperatura`, el método `procesarLectura()` podría calcular y eliminar el valor más bajo de su lista interna.
  * Para `SensorPresion`, podría calcular el promedio de las lecturas.

5. **Liberación de Memoria:** El destructor del sistema debe:

  * Liberar los nodos de la Lista de Gestión Polimórfica (la lista principal).
  * Para cada `SensorBase*` liberado, debe invocar al destructor de la clase derivada.
  * Los destructores de las clases derivadas (`SensorTemperatura`, `SensorPresion`) son responsables de liberar la memoria de todos los nodos de su `ListaSensor<T>` interna.

---

## Requerimientos Funcionales y No Funcionales

### Requisitos Funcionales

* **Simulación mediante Ardunino**: Simular el envío de señales mediante un dispositivo Arduino, el cual deberá enviar los tipos de señales desde el puerto serial.
* **Registro Polimórfico:** Permitir registrar cualquier tipo de sensor en una única lista de gestión.
* **Inserción Genérica:** Permitir la inserción de lecturas (`float` o `int`) en las listas internas de los sensores.
* **Procesamiento Polimórfico:** Iterar sobre la lista de gestión y llamar a la función `procesarLectura()`, la cual ejecuta lógica distinta según el tipo de sensor.
* **Operación de Lista:** Implementar las operaciones básicas de la Lista Enlazada Simple: Inserción al final y Búsqueda.
* **Uso de CMake**: Se deberá generar una sistema redistribuible mediante el uso de CMake.


### Requisitos No Funcionales

* **Exclusividad de Punteros:** Prohibido el uso de la STL (`std::list`, `std::vector`, `std::string`). El manejo de las listas debe ser completamente manual con punteros.
* **POO Avanzado:** Uso estricto de Clases Abstractas y Métodos Virtuales Puros.
* **Regla de los Tres/Cinco:** La clase que gestione la Lista Enlazada (y potencialmente las clases que contienen punteros) deben implementar el destructor, constructor de copia y operador de asignación (o al menos el destructor).

---

## Ejemplo de Entradas y Salidas del Problema en ConsolA

```Bash
Simulación de Interacción
--- Sistema IoT de Monitoreo Polimórfico ---

Opción 1: Crear Sensor (Tipo Temp - FLOAT)
Sensor 'T-001' creado e insertado en la lista de gestión.

Opción 2: Crear Sensor (Tipo Presion - INT)
Sensor 'P-105' creado e insertado en la lista de gestión.

Opción 3: Registrar Lectura
ID: T-001. Valor: 45.3 (float)
ID: T-001. Valor: 42.1 (float)

Opción 3: Registrar Lectura
ID: P-105. Valor: 80 (int)
ID: P-105. Valor: 85 (int)

Opción 4: Ejecutar Procesamiento Polimórfico

--- Procesando Sensores ---
[T-001] (Temperatura): Lectura más baja (42.1) eliminada. Promedio restante: 45.3.
[P-105] (Presion): Promedio de lecturas: 82.5.

Opción 5: Cerrar Sistema (Liberar Memoria)
```

---

## Entregables

1. Código fuente debidamente documentado mediante Doxygen.
2. Documentación generada mediante Doxygen.
3. Reporte que deberá contener:
   * Introducción
   * Manual técnico (Diseño, desarrllo, componentes)
4. Pantallasos de la implementación generada. 
````
