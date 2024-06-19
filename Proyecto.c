//Se importan las bibliotecas necesarias
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb-master/stb_image_write.h"
//El valor maximo de la imagen es 255
#define L 256
//Se definen los prototipos de las funciones
void ecualizacion(unsigned char *srcIma, int canales, int size, int *histograma, int *acumHisto, int *histoEcualizado, int min);
void ecualizacionParalela(unsigned char *srcIma, int canales, int size, int *histograma, int *acumHisto, int *histoEcualizado, int min);
void imagenGris(unsigned char *srcIma, int canales, int size);
//Funcion principal
int main(int argc, char *argv[])
{
    //Se verifica que se haya ingresado un argumento
    if (argc < 2)
    {
        printf("El programa necesita un argumento: nombre de la imagen a cargar.\n");
        return 0;
    }
    //Se declaran algunas variables necesarias  
    int ancho, alto, canales;
    char *srcPath = argv[1];
    float t1, t2;
    //Se carga la imagen y mide el tiempo que tarda
    t1 = omp_get_wtime();
    unsigned char *srcIma = stbi_load(srcPath, &ancho, &alto, &canales, 0);
    t2 = omp_get_wtime();
    //Se verifica que la imagen se haya cargado correctamente
    if (srcIma == NULL)
    {
        printf("No se pudo cargar la imagen %s\n", srcPath);
        return 0;
    }
    //Se imprimen los datos de la imagen en caso de que se haya cargado correctamente
    int size = ancho * alto;
    printf("Ancho: %d\n", ancho);
    printf("Alto: %d\n", alto);
    printf("Canales: %d\n", canales);
    printf("Tamaño: %d\n", size);
    printf("Tiempo de carga: %f\n", t2 - t1);
    //Se obtiene la extension del archivo
    char *extension = strchr(srcPath, '.');
    //Se verifica si la imagen es a color
    if (canales == 3)
    {
        //Se convierte la imagen a escala de grises
        imagenGris(srcIma, canales, size);
        //Se obtiene el nombre de la imagen en escala de grises
        char *dstPathGris = malloc(strlen(srcPath) + 10);
        strncpy(dstPathGris, srcPath, extension - srcPath);
        dstPathGris[extension - srcPath] = '\0';
        strcat(dstPathGris, "_gris");
        strcat(dstPathGris, ".png");
        //Se guarda la imagen en escala de grises
        stbi_write_png(dstPathGris, ancho, alto, 1, srcIma, ancho * 1);
        //Se reinician los valores de la imagen
        srcIma = stbi_load(srcPath, &ancho, &alto, &canales, 0);
        free(dstPathGris);
    }
    //Se declaran los arreglos necesarios para la ecualizacion
    int histograma[L], acumHisto[L], histoEcualizado[L];
    //Se mide el tiempo de la ecualizacion secuencial y se imprime
    t1 = omp_get_wtime();
    ecualizacion(srcIma, canales, size, histograma, acumHisto, histoEcualizado, 0);
    t2 = omp_get_wtime();
    float tiempoSecuencial = t2 - t1;
    printf("Tiempo de ecualizacion secuencial: %f\n", tiempoSecuencial);
    //Se obtiene el nombre de la imagen ecualizada
    char *dstPath = malloc(strlen(srcPath) + 16);
    strncpy(dstPath, srcPath, extension - srcPath);
    dstPath[extension - srcPath] = '\0';
    strcat(dstPath, "_eq_secuencial");
    strcat(dstPath, extension);
    //Se guarda la imagen ecualizada, mide el tiempo y se imprime
    t1 = omp_get_wtime();
    stbi_write_jpg(dstPath, ancho, alto, canales, srcIma, 100);
    t2 = omp_get_wtime();
    printf("Tiempo de escritura imagen secuencial: %f\n", t2 - t1);
    //Se obtiene el nombre del archivo del histograma
    char *dstPathHistogram = malloc(strlen(srcPath) + 19);
    strncpy(dstPathHistogram, srcPath, extension - srcPath);
    dstPathHistogram[extension - srcPath] = '\0';
    strcat(dstPathHistogram, "_histo_secuencial");
    strcat(dstPathHistogram, ".csv");
    //Se guarda el histograma, mide el tiempo y se imprime
    t1 = omp_get_wtime();
    FILE *file = fopen(dstPathHistogram, "w");
    for (int i = 0; i < L; i++)
    {
        fprintf(file, "%d,%d,%d\n", i, histograma[i], histoEcualizado[i]);
    }
    fclose(file);
    t2 = omp_get_wtime();
    printf("Tiempo de escritura histograma secuencial: %f\n", t2 - t1);
    //Se reinician los valores de la imagen
    srcIma = stbi_load(srcPath, &ancho, &alto, &canales, 0);
    //Se mide el tiempo de la ecualizacion paralela y se imprime
    t1 = omp_get_wtime();
    ecualizacionParalela(srcIma, canales, size, histograma, acumHisto, histoEcualizado, 0);
    t2 = omp_get_wtime();
    float tiempoParalelo = t2 - t1;
    printf("Tiempo de ecualizacion paralelo: %f\n", tiempoParalelo);
    //Se imprime el numero de procesadores, el speedup, la eficiencia y el tiempo de overhead
    printf("Nº de procesadores: %d\n", omp_get_num_procs());
    printf("SpeedUp: %f\n", tiempoSecuencial / tiempoParalelo);
    printf("Eficiencia: %f\n", (tiempoSecuencial / tiempoParalelo) / omp_get_num_procs());
    printf("Tiempo de overhead: %f\n", tiempoParalelo - tiempoSecuencial/omp_get_num_procs());
    //Se obtiene el nombre de la imagen ecualizada
    char *dstPathParalelo = malloc(strlen(srcPath) + 16);
    strncpy(dstPathParalelo, srcPath, extension - srcPath);
    dstPathParalelo[extension - srcPath] = '\0';
    strcat(dstPathParalelo, "_eq_paralelo");
    strcat(dstPathParalelo, extension);
    //Se guarda la imagen ecualizada, mide el tiempo y se imprime
    t1 = omp_get_wtime();
    stbi_write_jpg(dstPathParalelo, ancho, alto, canales, srcIma, 100);
    t2 = omp_get_wtime();
    printf("Tiempo de escritura imagen paralelo: %f\n", t2 - t1);
    //Se obtiene el nombre del archivo del histograma
    char *dstPathHistogramParalelo = malloc(strlen(srcPath) + 19);
    strncpy(dstPathHistogramParalelo, srcPath, extension - srcPath);
    dstPathHistogramParalelo[extension - srcPath] = '\0';
    strcat(dstPathHistogramParalelo, "_histo_paralelo");
    strcat(dstPathHistogramParalelo, ".csv");
    //Se guarda el histograma, mide el tiempo y se imprime
    t1 = omp_get_wtime();
    FILE *fileParalelo = fopen(dstPathHistogramParalelo, "w");
    for (int i = 0; i < L; i++)
    {
        fprintf(fileParalelo, "%d,%d,%d\n", i, histograma[i], histoEcualizado[i]);
    }
    fclose(fileParalelo);
    t2 = omp_get_wtime();
    printf("Tiempo de escritura histograma paralelo: %f\n", t2 - t1);
    //Se llama al script de python para graficar los histogramas
    char *command = malloc(255);
    sprintf(command, "python3 Histograma.py %s %s\n", dstPathHistogram, dstPathHistogramParalelo);
    system(command);
    //Se libera la memoria
    stbi_image_free(srcIma);
    free(dstPath);
    free(dstPathHistogram);
    free(dstPathParalelo);
    free(dstPathHistogramParalelo);
    free(command);
    return 0;
}
void ecualizacion(unsigned char *srcIma, int canales, int size, int *histograma, int *acumHisto, int *histoEcualizado, int min)
{
    //Se comprueba si la imagen esta en escala de grises o a color
    if (canales == 1)
    {
        //Se inicializa el histograma en 0
        for (int i = 0; i < L; i++)
        {
            histograma[i] = 0;
        }
        //Se calcula el histograma de frecuencias
        for (int i = 0; i < size; i++)
        {
            histograma[srcIma[i]]++;
        }
        //Se calcula el histograma acumulado
        acumHisto[0] = histograma[0];
        for (int i = 1; i < L; i++)
        {
            acumHisto[i] = acumHisto[i - 1] + histograma[i];       
        }
        //Se obtiene el valor minimo del histograma
        while (histograma[min++] == 0);
        //Se realiza la ecualizacion
        for (int i = 0; i < L; i++)
        {
            histoEcualizado[i] = round(((double)(acumHisto[i] - histograma[min]) / (double)(size - histograma[min])) * (L - 1));
        }
        //Se ecualiza la imagen
        for (int i = 0; i < size; i++)
        {
            srcIma[i] = histoEcualizado[srcIma[i]];
        }
        //Se restaura el histograma ecualizado en 0
        for (int i = 0; i < L; i++)
        {
            histoEcualizado[i] = 0;
        }
        //Se calcula el histograma de frecuencias ecualizado
        for (int i = 0; i < size; i++)
        {
            histoEcualizado[srcIma[i]]++;
        }
    }
    else if (canales == 3)
    {
        //Se inicializa el histograma en 0
        for (int i = 0; i < L; i++)
        {
            histograma[i] = 0;
        }
        //Se calcula el histograma de frecuencias
        for (int i = 0; i < size; i++)
        {
            histograma[(int)round((srcIma[i * 3] + srcIma[i * 3 + 1] + srcIma[i * 3 + 2]) / 3)]++;
        }
        //Se calcula el histograma acumulado
        acumHisto[0] = histograma[0];
        for (int i = 1; i < L; i++)
        {
            acumHisto[i] = acumHisto[i - 1] + histograma[i];        
        }
        //Se obtiene el valor minimo del histograma
        while (histograma[min++] == 0);
        //Se realiza la ecualizacion
        for (int i = 0; i < L; i++)
        {
            histoEcualizado[i] = round(((double)(acumHisto[i] - histograma[min]) / (double)(size - histograma[min])) * (L - 1));
        }
        //Se ecualiza la imagen
        int value;
        for (int i = 0; i < size; i++)
        {
            value = (int)round((srcIma[i * 3] + srcIma[i * 3 + 1] + srcIma[i * 3 + 2]) / 3);
            srcIma[i * 3] = histoEcualizado[value];
            srcIma[i * 3 + 1] = histoEcualizado[value];
            srcIma[i * 3 + 2] = histoEcualizado[value];
        }
        //Se restaura el histograma ecualizado en 0
        for (int i = 0; i < L; i++)
        {
            histoEcualizado[i] = 0;
        }
        //Se calcula el histograma de frecuencias ecualizado
        for (int i = 0; i < size; i++)
        {
            histoEcualizado[(int)round((srcIma[i * 3] + srcIma[i * 3 + 1] + srcIma[i * 3 + 2]) / 3)]++;
        }
    }
    //Si la imagen no esta en escala de grises ni a color, se muestra un mensaje de error
    else
    {
        printf("El programa solo acepta imagenes con 1 o 3 canales.\n");
    }
}
void ecualizacionParalela(unsigned char *srcIma, int canales, int size, int *histograma, int *acumHisto, int *histoEcualizado, int min){
    if (canales == 1)
    {
        //Se abre la region paralela
        #pragma omp parallel
        {
            //Se inicializa el histograma en 0
            #pragma omp for
            for (int i = 0; i < L; i++)
            {
                histograma[i] = 0;
            }
            //Se calcula el histograma de frecuencias
            #pragma omp for reduction(+:histograma[:L])
            for (int i = 0; i < size; i++)
            {
                histograma[srcIma[i]]++;
            }
            //Se calcula el histograma acumulado y se obtiene el valor minimo del histograma con un solo hilo
            #pragma omp single
            {
                acumHisto[0] = histograma[0];
                for (int i = 1; i < L; i++)
                {
                    acumHisto[i] = acumHisto[i - 1] + histograma[i];       
                }
                while (histograma[min++] == 0);
            }
            //Se realiza la ecualizacion
            #pragma omp for           
            for (int i = 0; i < L; i++)
            {
                histoEcualizado[i] = round(((double)(acumHisto[i] - histograma[min]) / (double)(size - histograma[min])) * (L - 1));
            }
            //Se ecualiza la imagen
            #pragma omp for
            for (int i = 0; i < size; i++)
            {
                srcIma[i] = histoEcualizado[srcIma[i]];
            }
            //Se restaura el histograma ecualizado en 0
            #pragma omp for
            for (int i = 0; i < L; i++)
            {
                histoEcualizado[i] = 0;
            }
            //Se calcula el histograma de frecuencias ecualizado
            #pragma omp for reduction(+:histoEcualizado[:L])
            for (int i = 0; i < size; i++)
            {
                histoEcualizado[srcIma[i]]++;
            }
        }
    } else if (canales == 3)
    {
        int value;
        //Se abre la region paralela
        #pragma omp parallel
        {
            //Se inicializa el histograma en 0
            #pragma omp for
            for (int i = 0; i < L; i++)
            {
                histograma[i] = 0;
            }
            //Se calcula el histograma de frecuencias
            #pragma omp for reduction(+:histograma[:L])
            for (int i = 0; i < size; i++)
            {
                histograma[(int)round((srcIma[i * 3] + srcIma[i * 3 + 1] + srcIma[i * 3 + 2]) / 3)]++;
            }
            //Se calcula el histograma acumulado y se obtiene el valor minimo del histograma con un solo hilo
            #pragma omp single
            {
                acumHisto[0] = histograma[0];
                for (int i = 1; i < L; i++)
                {
                    acumHisto[i] = acumHisto[i - 1] + histograma[i];        
                }
                while (histograma[min++] == 0);
            }
            //Se realiza la ecualizacion
            #pragma omp for           
            for (int i = 0; i < L; i++)
            {
                histoEcualizado[i] = round(((double)(acumHisto[i] - histograma[min]) / (double)(size - histograma[min])) * (L - 1));
            }
            //Se ecualiza la imagen
            #pragma omp for private(value)
            for (int i = 0; i < size; i++)
            {
                value = (int)round((srcIma[i * 3] + srcIma[i * 3 + 1] + srcIma[i * 3 + 2]) / 3);
                srcIma[i * 3] = histoEcualizado[value];
                srcIma[i * 3 + 1] = histoEcualizado[value];
                srcIma[i * 3 + 2] = histoEcualizado[value];
            }
            //Se restaura el histograma ecualizado en 0
            #pragma omp for
            for (int i = 0; i < L; i++)
            {
                histoEcualizado[i] = 0;
            }
            //Se calcula el histograma de frecuencias ecualizado
            #pragma omp for reduction(+:histoEcualizado[:L])
            for (int i = 0; i < size; i++)
            {
                histoEcualizado[(int)round((srcIma[i * 3] + srcIma[i * 3 + 1] + srcIma[i * 3 + 2]) / 3)]++;
            }
        }
    } else
    {
        printf("El programa solo acepta imagenes con 1 o 3 canales.\n");
    }
}
void imagenGris(unsigned char *srcIma, int canales, int size){
    if (canales == 3)
    {
        //En cada iteracion se calcula el promedio de los 3 canales y se guarda en el mismo canal
        for (int i = 0; i < size; i++)
        {
            srcIma[i] = (int)round((srcIma[i * 3] + srcIma[i * 3 + 1] + srcIma[i * 3 + 2]) / 3);
        }
    }
}