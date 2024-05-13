import glfw  # Importa a biblioteca GLFW usada para criar e manipular janelas e receber entradas
from OpenGL.GL import *  # Importa as funções básicas do OpenGL
from OpenGL.GLU import *  # Importa as funções de utilidades do OpenGL
import numpy as np  # Importa a biblioteca numpy para operações matemáticas avançadas
import os  # Importa a biblioteca os para interagir com o sistema operacional
import sys  # Importa a biblioteca sys para acessar funções e variáveis do sistema

def carregar_obj(arquivo):
    vertices = []  # Lista para armazenar os vértices do modelo
    indices = []  # Lista para armazenar os índices dos vértices que formam as faces
    normais = []  # Lista para armazenar as normais dos vértices
    with open(arquivo, 'r') as arquivo:  # Abre o arquivo do modelo OBJ para leitura
        for linha in arquivo:  # Itera sobre cada linha do arquivo
            partes = linha.strip().split()  # Divide a linha em partes
            if not partes:
                continue
            if partes[0] == 'v':  # Se a linha define um vértice
                vertices.append(list(map(float, partes[1:4])))  # Adiciona o vértice à lista
            elif partes[0] == 'f':  # Se a linha define uma face
                indices_face = [int(part.split('/')[0]) - 1 for part in partes[1:]]  # Extrai os índices dos vértices
                indices.extend(indices_face)  # Adiciona os índices à lista
            elif partes[0] == 'vn':  # Se a linha define uma normal de vértice
                normais.append(list(map(float, partes[1:4])))  # Adiciona a normal à lista
    return np.array(vertices, dtype=np.float32), np.array(indices, dtype=np.uint32), np.array(normais, dtype=np.float32)

def configurar_visao_e_projecao(visao, largura, altura, centro, raio, cores):
    x = (visao % 2) * largura // 2
    y = (visao // 2) * altura // 2
    glViewport(x, y, largura // 2, altura // 2)  # Define a região da janela para a visualização atual
    glScissor(x, y, largura // 2, altura // 2)  # Define a região para limitar a área de desenho
    glEnable(GL_SCISSOR_TEST)  # Habilita o teste de tesoura
    glClearColor(*cores[visao])  # Define a cor de fundo para a vista
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)  # Limpa os buffers de cor e profundidade
    glDisable(GL_SCISSOR_TEST)  # Desabilita o teste de tesoura

    glMatrixMode(GL_PROJECTION)  # Alterna para a matriz de projeção
    glLoadIdentity()  # Carrega a matriz identidade
    if visao == 0:  # Se a visão for a perspectiva
        gluPerspective(45, float(largura) / float(altura), 0.1, 100.0)  # Configura a projeção perspectiva
        olho = centro + np.array([0, 0, 3 * raio])
    else:  # Se a visão for ortográfica
        glOrtho(-raio, raio, -raio, raio, 0.1, 10 * raio)  # Configura a projeção ortográfica
        if visao == 1:
            olho = centro + np.array([0, 3 * raio, 0])
        elif visao == 2:
            olho = centro + np.array([-3 * raio, 0, 0])
        elif visao == 3:
            olho = centro + np.array([0, 0, 3 * raio])

    glMatrixMode(GL_MODELVIEW)  # Alterna para a matriz de modelo/visão
    glLoadIdentity()  # Carrega a matriz identidade
    cima = np.array([0, 1, 0])  # Define a direção "para cima" padrão
    if visao == 1:  # Ajusta a direção "para cima" para a vista de topo
        cima = np.array([0, 0, -1])
    gluLookAt(olho[0], olho[1], olho[2], centro[0], centro[1], centro[2], cima[0], cima[1], cima[2])  # Configura a câmera

def main():
    if not glfw.init():  # Inicializa a biblioteca GLFW
        return
    
    janela = glfw.create_window(800, 800, "Projeções do Bule com Fundo Colorido", None, None)  # Cria uma janela
    if not janela:
        glfw.terminate()  # Termina o GLFW se a janela não puder ser criada
        return
    
    glfw.make_context_current(janela)  # Torna o contexto da janela atual
    
    diretorio_atual = os.path.dirname(sys.argv[0])  # Obtém o diretório atual
    caminho_bule = os.path.join(diretorio_atual, 'teapot.obj')  # Monta o caminho para o arquivo do modelo
    
    vertices, indices, normais = carregar_obj(caminho_bule)  # Carrega o modelo OBJ
    centro, raio = np.mean(vertices, axis=0), np.ptp(vertices) / 2  # Calcula o centro e o raio do modelo
    
    glEnable(GL_DEPTH_TEST)  # Habilita o teste de profundidade

    cores = [
        (1.0, 0.0, 0.0, 1.0),  # Vermelho
        (0.0, 1.0, 0.0, 1.0),  # Verde
        (0.0, 0.0, 1.0, 1.0),  # Azul
        (1.0, 1.0, 0.0, 1.0)   # Amarelo
    ]
    
    while not glfw.window_should_close(janela):  # Loop principal enquanto a janela estiver aberta
        largura, altura = glfw.get_framebuffer_size(janela)  # Obtém as dimensões da janela
        
        for visao in range(4):  # Para cada uma das 4 vistas
            configurar_visao_e_projecao(visao, largura, altura, centro, raio, cores)  # Configura a visão e projeção
            glEnableClientState(GL_VERTEX_ARRAY)  # Habilita o array de vértices
            glVertexPointer(3, GL_FLOAT, 0, vertices)  # Define os vértices
            if normais.size > 0:
                glEnableClientState(GL_NORMAL_ARRAY)  # Habilita o array de normais
                glNormalPointer(GL_FLOAT, 0, normais)  # Define as normais
            glDrawElements(GL_TRIANGLES, len(indices), GL_UNSIGNED_INT, indices)  # Desenha o modelo
            glDisableClientState(GL_VERTEX_ARRAY)  # Desabilita o array de vértices
            glDisableClientState(GL_NORMAL_ARRAY)  # Desabilita o array de normais
        
        glfw.swap_buffers(janela)  # Troca os buffers de renderização
        glfw.poll_events()  # Processa eventos de entrada
    
    glfw.terminate()  # Termina o GLFW ao fechar a janela

if __name__ == "__main__":
    main()  # Executa a função principal
