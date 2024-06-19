
# Ecualización de Histogramas en Imágenes ByN

## Descripción
Este proyecto toma un archivo de una imagen en blanco y negro y genera una nueva imagen con el histograma ecualizado. Además, genera un archivo CSV con el histograma original y ecualizado y una gráfica de matplotlib que ilustra ambos histogramas. El procesamiento se realiza de dos formas: secuencialmente y en paralelo utilizando OpenMP. También se muestra en la consola información básica de la imagen y los tiempos de procesamiento.

## Instrucciones de Instalación
1. Asegúrese de que la biblioteca STB esté en una carpeta en el mismo directorio que `Proyecto.c`.
2. Compile el archivo `Proyecto.c` utilizando un compilador compatible con OpenMP:
   ```sh
   gcc -fopenmp -lm -o proyecto Proyecto.c
   ```

## Uso
Ejecute el programa pasando la ruta de la imagen como argumento. Por ejemplo:
```sh
./proyecto images/image-1.jpg
```

## Salida del Programa
El programa muestra en la consola la siguiente información básica sobre la imagen:
- Ancho
- Alto
- Canales
- Tamaño
- Tiempo de carga

Además, se generan los siguientes archivos:
- Una nueva imagen con el histograma ecualizado.
- Un archivo CSV con el histograma original y el ecualizado.
- Una gráfica generada con matplotlib que ilustra ambos histogramas.

## Funciones Principales
- `ecualizacion`: Realiza la ecualización del histograma de forma secuencial.
- `ecualizacionParalela`: Realiza la ecualización del histograma de forma paralela utilizando OpenMP.
- `imagenGris`: Convierte la imagen a escala de grises.

#### En `images` se pueden ver algunos ejemplos de la ejecución.
