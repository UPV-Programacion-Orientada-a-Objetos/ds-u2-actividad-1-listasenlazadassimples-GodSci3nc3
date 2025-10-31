## Sistema IoT 

Este repositorio contiene una implementación de un sistema polimórfico de gestión de sensores y ejemplos para ejecutar.

Archivos relevantes:

- `main.cpp` : código fuente principal (implementación simple, sin STL para listas).
- `CMakeLists.txt` : proyecto CMake (c++17).
- `samples/lecturas.txt` : ejemplo de lecturas para probar sin Arduino.
- `mainpage(example).dox` : página principal de Doxygen (plantilla del compañero).
- `README_SPEC.md` : copia del enunciado / especificación original (no tocar).

Construir y ejecutar

1. Compilar con CMake:

```bash
mkdir -p build && cd build
cmake ..
make -j
```

2. Ejecutar en modo archivo (pruebas):

```bash
./iot_monitor ../samples/lecturas.txt
```

3. Ejecutar con Arduino (ejemplo):

Conecta el Arduino por USB. Identifica el puerto (/dev/ttyACM0 o /dev/ttyUSB0).

Dar permisos o usar sudo:

```bash
# opción recomendada (añadir al grupo dialout)
sudo usermod -aG dialout $USER
# o ejecutar con sudo
sudo ./iot_monitor /dev/ttyACM0
```

El programa configura el puerto a 9600,8N1. El Arduino debe enviar líneas con formato:

```
TEMP T-001 23.5
PRES P-105 1013
VIBR V-201 1075
```

Generar documentación (Doxygen)

Si Doxygen está instalado, en el build puedes ejecutar `make doc` (CMake ya tiene target `doc`). El `Doxyfile` está en la raíz del proyecto.

Archivos generados por este cambio

- `README_SPEC.md` : copia del README original (especificación del profesor).
- `README.md` : este README de uso rápido.

