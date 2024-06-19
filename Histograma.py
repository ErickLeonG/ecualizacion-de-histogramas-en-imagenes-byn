import matplotlib.pyplot as plt
import os
import sys
def Graficar(ruta):
    try:
        CSV = open(os.path.join(os.path.dirname(__file__), ruta), 'r', encoding='utf-8')
        linea = True
        hisograma = []
        positions = []
        while linea:
            linea = CSV.readline()
            if linea:
                datos = linea.split(',')
                positions.append(datos[0])
                hisograma.append([datos[1], datos[2]])
        plt.bar(positions, [int(hisograma[i][0]) for i in range(len(hisograma))], label='Histograma original')
        plt.bar(positions, [int(hisograma[i][1]) for i in range(len(hisograma))], label='Histograma ecualizado')
        plt.title('Gráfica de histogramas')
        plt.suptitle('Histograma de ' + ruta)
        plt.xlabel('Valor')
        plt.ylabel('Frecuencia')
        plt.xticks([i for i in range(0, 256, 15)])
        fig = plt.gcf()
        fig.set_size_inches(12, 6)
        plt.legend()
        plt.savefig(os.path.join(os.path.dirname(__file__),ruta.split('.')[0] + '.png'))
        plt.clf()
    except FileNotFoundError:
        print('Error al abrir el archivo ' + ruta)
        exit()
if __name__ == '__main__':
    if(len(sys.argv) != 3):
        print('Uso: python3 Histograma.py <archivo.csv> <archivo.csv>')
        exit()
    Graficar(sys.argv[1])
    Graficar(sys.argv[2])