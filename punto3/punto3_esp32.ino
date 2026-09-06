/*
  ============================================================================
  TP1 - Rendimiento de las computadoras
  Efecto de la frecuencia de reloj sobre el tiempo de ejecucion
  ============================================================================

  Que hace:
    1) Calibra automaticamente cuantas iteraciones (N) hacen falta para que
       cada bucle (enteros, floats, doubles) tarde ~10 segundos corriendo a
       240 MHz (la frecuencia maxima del ESP32).
    2) Con ese N ya FIJO, repite los tres bucles a distintas frecuencias de
       CPU (240, 160, 80, 40 MHz) y mide el tiempo real de cada uno.
    3) Imprime todo por el Monitor Serie en formato CSV para poder pegarlo
       en una hoja de calculo y graficarlo.

  Por que interesa:
    El ESP32 (Xtensa LX6) tiene una FPU por hardware, pero SOLO para
    precision simple (float). Las operaciones con "double" (precision
    doble) se emulan por software, por lo que son mucho mas lentas que
    float aunque el codigo se vea igual. Comparando int / float / double
    a distintas frecuencias se puede ver:
      - Como escala el tiempo con la frecuencia (deberia ser ~inversamente
        proporcional: al duplicar la frecuencia, el tiempo se reduce
        aproximadamente a la mitad, porque estos bucles son "CPU-bound").
      - Cuanto mas rapido es float respecto de double gracias a la
        aceleracion por hardware.

  NOTA IMPORTANTE (comparacion justa):
    Los tres bucles hacen EXACTAMENTE el mismo trabajo: una suma y una
    multiplicacion por iteracion. Esto es clave: si un bucle hiciera menos
    operaciones que otro, la diferencia de tiempo podria deberse a eso y no
    al tipo de dato. Al igualar las operaciones, cualquier diferencia entre
    int, float y double se atribuye limpiamente al tipo de dato y a si la
    operacion se resuelve por hardware (float) o por software (double).

  Placa: cualquier ESP32 clasico (ESP32-WROOM-32 / DevKit / NodeMCU-32S).
  No hace falta ningun componente externo, solo el cable USB.

  IDE: Arduino IDE 2.x + paquete de placas "esp32" de Espressif.
  ============================================================================
*/

#include <Arduino.h>

// ---------------------------------------------------------------------------
// Configuracion
// ---------------------------------------------------------------------------

// Frecuencias a probar, en MHz. 240/160/80 son validas en cualquier ESP32.
// 40 MHz depende del cristal (la mayoria de las placas dev usan cristal de
// 40 MHz, asi que tambien deberia funcionar). Si alguna falla en tu placa,
// setCpuFrequencyMhz() devuelve false y el sketch la salta sola.
const uint32_t FRECUENCIAS[] = {240, 160, 80, 40};
const int N_FRECUENCIAS = sizeof(FRECUENCIAS) / sizeof(FRECUENCIAS[0]);

// Tiempo objetivo de calibracion (en microsegundos): 10 segundos.
const double TIEMPO_OBJETIVO_US = 10.0e6;

// N de iteraciones de cada bucle, calculado una sola vez en setup().
uint32_t N_int, N_float, N_double;

// Variables "volatile" para que el compilador no optimice el bucle y lo
// borre (porque el resultado de la suma no se usa para nada mas).
volatile int32_t  sink_int;
volatile float    sink_float;
volatile double   sink_double;

// ---------------------------------------------------------------------------
// Los tres bucles de trabajo
// Los tres hacen lo mismo: acc += i * 2  (una multiplicacion y una suma),
// cada uno con su tipo de dato. Asi la comparacion es justa.
// ---------------------------------------------------------------------------

unsigned long sumaEnteros(uint32_t n) {
  int32_t acc = 0;
  unsigned long t0 = micros();
  for (uint32_t i = 0; i < n; i++) {
    acc += (int32_t)i * 2;
  }
  unsigned long dt = micros() - t0;
  sink_int = acc;          // evita que el compilador elimine el bucle
  return dt;
}

unsigned long sumaFloats(uint32_t n) {
  float acc = 0.0f;
  unsigned long t0 = micros();
  for (uint32_t i = 0; i < n; i++) {
    acc += (float)i * 2.0f;
  }
  unsigned long dt = micros() - t0;
  sink_float = acc;
  return dt;
}

unsigned long sumaDoubles(uint32_t n) {
  double acc = 0.0;
  unsigned long t0 = micros();
  for (uint32_t i = 0; i < n; i++) {
    acc += (double)i * 2.0;
  }
  unsigned long dt = micros() - t0;
  sink_double = acc;
  return dt;
}

// ---------------------------------------------------------------------------
// Calibracion: corre una prueba chica y extrapola cuantas iteraciones
// hacen falta para llegar a ~10 s, siempre a 240 MHz.
// ---------------------------------------------------------------------------
uint32_t calibrar(unsigned long (*func)(uint32_t), uint32_t n_prueba) {
  unsigned long dt = func(n_prueba);              // tiempo de la prueba chica
  double tasa = (double)n_prueba / (double)dt;    // iteraciones por us
  double n_objetivo = tasa * TIEMPO_OBJETIVO_US;
  return (uint32_t)n_objetivo;
}

// ---------------------------------------------------------------------------
// setup(): todo el experimento corre una sola vez aca
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(2000); // tiempo para abrir el Monitor Serie antes de que empiece

  setCpuFrequencyMhz(240); // calibramos siempre a la frecuencia maxima
  delay(100);

  Serial.println();
  Serial.println("Calibrando N para que cada bucle tarde ~10 s a 240 MHz...");

  N_int    = calibrar(sumaEnteros, 20000000UL);
  N_float  = calibrar(sumaFloats,  20000000UL);
  N_double = calibrar(sumaDoubles,  5000000UL); // double es mas lento, arranca con menos

  Serial.printf("N_int    = %lu\n", (unsigned long)N_int);
  Serial.printf("N_float  = %lu\n", (unsigned long)N_float);
  Serial.printf("N_double = %lu\n", (unsigned long)N_double);
  Serial.println();

  Serial.println("=== RESULTADOS (formato CSV) ===");
  Serial.println("freq_MHz,tipo,N,tiempo_ms");

  for (int i = 0; i < N_FRECUENCIAS; i++) {
    uint32_t f = FRECUENCIAS[i];
    bool ok = setCpuFrequencyMhz(f);
    delay(200); // deja que el reloj se estabilice antes de medir

    if (!ok) {
      Serial.printf("# %lu MHz no soportado en esta placa, se salta\n",
                    (unsigned long)f);
      continue;
    }

    unsigned long dt_int    = sumaEnteros(N_int);
    unsigned long dt_float  = sumaFloats(N_float);
    unsigned long dt_double = sumaDoubles(N_double);

    Serial.printf("%lu,int,%lu,%.2f\n",
                  (unsigned long)f, (unsigned long)N_int,    dt_int    / 1000.0);
    Serial.printf("%lu,float,%lu,%.2f\n",
                  (unsigned long)f, (unsigned long)N_float,  dt_float  / 1000.0);
    Serial.printf("%lu,double,%lu,%.2f\n",
                  (unsigned long)f, (unsigned long)N_double, dt_double / 1000.0);
  }

  setCpuFrequencyMhz(240); // restauramos la frecuencia por defecto
  Serial.println();
  Serial.println("Listo. Copia las lineas CSV de arriba a una planilla para graficar.");
}

void loop() {
  // No hace falta nada aca: todo el experimento corrio una vez en setup().
}
