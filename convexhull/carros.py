import cv2
import os
import sys

# Obter o diretório atual
diretorio_atual = os.path.dirname(sys.argv[0])

# Nome do arquivo de imagem
nome_imagem = "carros.webp"

# Concatenar o diretório e o nome da imagem
caminho_completo = os.path.join(diretorio_atual, nome_imagem)

# Carregar a imagem
imagem = cv2.imread(caminho_completo)
# Verificar se a imagem foi carregada com sucesso
if imagem is None:
    print(f'Erro ao carregar a imagem {caminho_completo}.')
    exit()

# Converter a imagem para escala de cinza
imagem_cinza = cv2.cvtColor(imagem, cv2.COLOR_BGR2GRAY)

# Binarizar a imagem
limiar, imagem_binaria = cv2.threshold(imagem_cinza,50,255,0) #cv2.threshold(imagem_cinza, 127, 255, cv2.THRESH_BINARY)

# Encontrar os contornos dos carros
contornos, hierarquia = cv2.findContours(imagem_binaria, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

# Desenhar os contornos na imagem original
cv2.drawContours(imagem, contornos, -1, (0, 255, 0), 2)

# Encontrar o fecho convexo de cada carro
fechos_convexos = []
# for contorno in contornos:
for contorno in contornos:    
    fecho_convexo = cv2.convexHull(contorno) #, clockwise=True)
    fechos_convexos.append(fecho_convexo)

# Desenhar os fechos convexos na imagem original
for fecho_convexo in fechos_convexos:
    cv2.drawContours(imagem, [fecho_convexo], -1, (0, 0, 255), 2)

# Salvar a imagem com a imagem final
nome_sem_extensao, extensao = os.path.splitext(nome_imagem)
novo_nome_imagem = f'hull_{nome_sem_extensao}.png'
caminho_completo_png = os.path.join(diretorio_atual, novo_nome_imagem)

cv2.imwrite(caminho_completo_png, imagem)

# Mostrar a imagem final
cv2.imshow('Fecho Convexo dos Carros', imagem)
cv2.waitKey(0)
cv2.destroyAllWindows()
