import numpy as np

# Dimensões da imagem
largura, altura = 256, 256

# Criar uma matriz vazia com fundo branco (valor máximo em RGB: 255)
imagem = np.full((altura, largura, 3), 255, dtype=np.uint8)

# Função para desenhar uma linha
def desenhar_linha(x0, y0, x1, y1, cor):
    íngreme = abs(y1 - y0) > abs(x1 - x0)
    if íngreme:
        x0, y0 = y0, x0
        x1, y1 = y1, x1
    if x0 > x1:
        x0, x1 = x1, x0
        y0, y1 = y1, y0
    dx = x1 - x0
    dy = abs(y1 - y0)
    erro = dx / 2
    ypasso = 1 if y0 < y1 else -1
    y = y0
    for x in range(x0, x1 + 1):
        if íngreme:
            if 0 <= x < altura and 0 <= y < largura:
                imagem[x, y] = cor
        else:
            if 0 <= x < largura and 0 <= y < altura:
                imagem[y, x] = cor
        erro -= dy
        if erro < 0:
            y += ypasso
            erro += dx

# Coordenadas do centro da flor
centro_x, centro_y = largura // 2, altura // 2
raio_pétala = 100
num_pétalas = 16
cor_amarela = [255, 255, 0]
cor_laranja = [255, 165, 0]

# Desenhar as pétalas
for i in range(num_pétalas):
    angulo = i * (360 / num_pétalas)
    fim_x = int(centro_x + raio_pétala * np.cos(np.radians(angulo)))
    fim_y = int(centro_y + raio_pétala * np.sin(np.radians(angulo)))
    desenhar_linha(centro_x, centro_y, fim_x, fim_y, cor_amarela)

# Desenhar o centro da flor
raio_centro = 50
for x in range(centro_x - raio_centro, centro_x + raio_centro):
    for y in range(centro_y - raio_centro, centro_y + raio_centro):
        if (x - centro_x) ** 2 + (y - centro_y) ** 2 <= raio_centro ** 2:
            if 0 <= x < largura and 0 <= y < altura:
                imagem[y, x] = cor_laranja

# Salvar a imagem em formato PPM (P3 para imagens coloridas em ASCII)
with open('girassol.ppm', 'w') as f:
    f.write(f'P3\n{largura} {altura}\n255\n')  # Cabeçalho com valor máximo de RGB
    # Converter dados da imagem para uma string de valores de pixel RGB separados por espaços
    for linha in imagem:
        for pixel in linha:
            f.write(f'{pixel[0]} {pixel[1]} {pixel[2]} ')  # Escrever cada valor de pixel RGB seguido por um espaço
        f.write('\n')  # Nova linha após cada linha
