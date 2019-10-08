#include "Headers/TimeManager.h"
using namespace std;
using namespace chrono;

/**
 * Metodo que calcula el frame rate.
 * @param writeToConsole bandera que indica si se requiere logear a la consola el valor que se obtiene
 * @return FPS
 */
double TimeManager::CalculateFrameRate(bool writeToConsole = false) {
	// Variables estaticas que almacenan los incrementos del tiempo
	static double framesPerSecond = 0.0f; // Se almacenan los frames por segundo
	static double startTime = GetTime(); // Se almacena el tiempo de inicio
	static double lastTime = GetTime(); //  Se almacena el tiempo del ulimo frame
	static char strFrameRate[50] = { 0 }; // Almacenamos la cadena para el titulo de la ventana
	static double currentFPS = 0.0f; // Se almacena el valor actual de los frames por segundos

	// Obtiene el tiempo actual en segundos, depende de nuestro ambiente
	CurrentTime = GetTime();

	// Se calcula el tiempo delta, este es el tiempo que ha pasadp desde la ultima que revisamos el tiempo actual
	DeltaTime = CurrentTime - lastTime;

	// Se asigna al lastTime como el tiempo actual para entonces obtener el tiempo pasado desde el ultimo frame
	lastTime = CurrentTime;

	// Se incrementa el countador de frames
	++framesPerSecond;

	// if a second has passed we can get the current frame rate
	if (CurrentTime - startTime > 1.0f) {
		// Here we set the startTime to the currentTime.  This will be used as the starting point for the next second.
		// This is because GetTime() counts up, so we need to create a delta that subtract the current time from.
		/*
		 *
		 */
		startTime = CurrentTime;

		// Si la bandera esta habilitada muestra los frames por segundo acutales.
		if (writeToConsole)
			fprintf(stderr, "Current Frames Per Second: %d\n",
					int(framesPerSecond));

		// Se almacenan los FPS actuales, reiniciamos este en la siguiente linea
		currentFPS = framesPerSecond;

		// Se reinicia los frames por segundos
		framesPerSecond = 0;
	}

	// Regresa el mas reciente calculo de los FPS
	return currentFPS;
}

/**
 * Metodo que obtien el tiempo actual en milisegundos
 * @return tiempo
 */
double TimeManager::GetTime() {
	// Guarda el tiempo del sistema que ha pasado desde 1/1/1970
	auto beginningOfTime = system_clock::now().time_since_epoch();

	// Se convierte el tiempo del sistema en milisegundos
	auto ms = duration_cast < milliseconds > (beginningOfTime).count();

	// Regresa el tiempo en segundos y usamos un valor fraccional para escalar el tiempo.
	return ms * 0.001;
}

/**
 * Metodo que pausa un cierto tiempo en milisegundo el hilo que se esta ejecutando
 * @param milliseconds tiempo de pausa
 */
void TimeManager::Sleep(int ms) {
	// Se pausa el hilo que se esta ejecutando actualmente.
	this_thread::sleep_for(milliseconds(ms));
}
