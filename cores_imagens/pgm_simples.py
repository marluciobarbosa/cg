import numpy as np
def escrever_pgm(imagem, nome_arquivo):
  """
  Função para escrever uma imagem em tons de cinza no formato PGM.

  Args:
    imagem: Uma matriz NumPy que representa a imagem em tons de cinza.
    nome_arquivo: O nome do arquivo PGM a ser criado.

  Retorna:
    Nada.
  """

  # Verificar se a imagem é uma matriz NumPy 2D
  if not imagem.ndim == 2:
    raise ValueError("A imagem deve ser uma matriz NumPy 2D.")

  # Verificar se a imagem é em tons de cinza (valores entre 0 e 255)
  if not imagem.min() >= 0 or not imagem.max() <= 255:
    raise ValueError("A imagem deve ser em tons de cinza (valores entre 0 e 255).")

  # Obter as dimensões da imagem
  altura, largura = imagem.shape

  # Criar o cabeçalho do arquivo PGM
  cabecalho = f"P5 {largura} {altura} 255\n"

  # Converter a matriz NumPy em bytes
  dados = imagem.astype(np.uint8).tobytes()

  # Abrir o arquivo PGM em modo binário e escrever o cabeçalho e os dados
  with open(nome_arquivo, "wb") as arquivo:
    arquivo.write(cabecalho.encode())
    arquivo.write(dados)

# Exemplo de uso
imagem = np.array([
  [0, 50, 100],
  [150, 200, 255],
  [220, 180, 130],
])

escrever_pgm(imagem, "imagem.pgm")
