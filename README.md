# TP1 - El rendimiento de las computadoras

Trabajo práctico de la materia [nombre de la materia] - [Universidad/Facultad].

Análisis de performance y rendimiento de computadores: uso de benchmarks de
terceros para decisiones de hardware, y herramientas para medir la performance
de código propio.

## Integrantes
- [Tu nombre]
- [Compañero/a 2]
- [Compañero/a 3]

**Grupo:** [nombre del grupo]

## Contenido del repositorio

- `informe/` - Informe único del grupo (PDF)
- `resultados.txt` - Resultados de mediciones individuales de cada integrante
- `punto3_esp32/` - Código Arduino para el ESP32 y resultados
- `punto4_gprof/` - Código de prueba, salida de gprof y gráfico del call graph

## Puntos desarrollados

1. **Benchmarks y tareas diarias** - Comparación con 7-Zip entre las máquinas del equipo.
2. **Compilación del kernel de Linux** - Rendimiento, speedup y eficiencia de tres CPUs
   (i5-13600K, Ryzen 9 5900X, Ryzen 9 7950X) usando datos de OpenBenchmarking.
3. **Frecuencia de CPU (ESP32)** - Efecto de variar la frecuencia sobre el tiempo de
   ejecución, comparando tipos de dato int/float/double.
4. **Profiling con gprof** - Análisis del tiempo por función y cómo reducirlo.
5. **Mediciones de red** - Ancho de banda LAN (iperf3), velocidad por proveedor,
   latencia (cable/WiFi/celular) y perturbación de videollamada.

## Herramientas utilizadas
Phoronix Test Suite, 7-Zip, OpenBenchmarking, Arduino IDE (ESP32), gcc/gprof,
gprof2dot, iperf3, fast.com, Google Meet.
