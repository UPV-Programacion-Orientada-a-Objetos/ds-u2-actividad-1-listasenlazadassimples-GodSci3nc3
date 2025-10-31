# INFORME: Sistema de Gestión Polimórfica de Sensores (Entrega Estructura de datos)

Fecha: 30 de octubre de 2025

Arturo Rosales V

Resumen
-------
Se entrega un programa en C++ (C++17) que implementa una jerarquía polimórfica de sensores y listas enlazadas genéricas (`ListaSensor<T>`). El sistema acepta lecturas desde stdin, desde un archivo de texto (`samples/lecturas.txt`) o desde un dispositivo serial (Arduino) y procesa las lecturas polimórficamente.

Objetivos
---------
- Implementar clases derivadas de `SensorBase` (Temperatura, Presión, Vibración).
- Implementar `ListaSensor<T>` como lista enlazada simple con Regla de los Tres (copia profunda).
- Permitir lectura por serial configurando el puerto a 9600 8N1.
- Documentar el código con Doxygen y entregar un informe técnico.

Diseño y decisión de implementación
-----------------------------------
- Polimorfismo: `SensorBase` define la interfaz; cada sensor implementa `addLectura`, `procesarLectura` e `imprimirInfo`.
- Almacenamiento: `ListaSensor<T>` implementa inserción al final, cálculo de promedio y eliminación del mínimo; implementa constructor copia y operador= para copia profunda.
- Manager: lista simple de `SensorBase*` que busca sensores por ID y delega la adición de lecturas.
- Serial: `open_input_path` detecta si la ruta es character device y configura termios a 9600/8N1; lee línea a línea con `fgets`.

Formato de entradas
-------------------
- Formato preferido (3 tokens): `TIPO ID VALOR` (ej.: `TEMP T-001 23.5`).
- Formato alternativo (2 tokens): `ID VALOR`. Si el sensor no existe se crea: si `VALOR` contiene `.` se considera `TEMP`, en caso contrario `PRES`.

Ejecución y pruebas
-------------------
1) Compilar:

```bash
mkdir -p build && cd build
cmake ..
make -j
```

2) Probar con archivo de ejemplo:

```bash
./iot_monitor ../samples/lecturas.txt
```

3) Probar con Arduino (conectar y usar `/dev/ttyACM0` o `/dev/ttyUSB0`):

```bash
sudo ./iot_monitor /dev/ttyACM0
```

Se recomienda añadir el usuario al grupo `dialout` para evitar usar `sudo`.

Resultados observados
--------------------
- El programa crea sensores cuando se reciben lecturas y muestra líneas de confirmación (`ID: ok` o `ID: creado y agregado`).
- El modo interactivo permite simular lecturas (opción "Simular Arduino") y ejecutar el procesamiento polimórfico.

Generación de documentación
---------------------------
1. Desde `build/` ejecutar:

```bash
make doc
```

Conclusión
----------
El sistema cumple los requisitos básicos del enunciado: polimorfismo, listas genéricas, lectura por serial y documentación inicial con Doxygen. Se incluyen instrucciones para generar la documentación y ejecutar pruebas sin hardware.
