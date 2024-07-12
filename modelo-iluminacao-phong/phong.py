# Importa as bibliotecas necessárias
import glfw  # Biblioteca para criar janelas e contextos OpenGL
from OpenGL.GL import *  # Importa todas as funções OpenGL
from OpenGL.GLU import *  # Importa funções de utilidade OpenGL
import glm  # Importa a biblioteca GLM
import numpy as np  # Biblioteca para cálculos numéricos

# Função para inicializar o GLFW e criar uma janela
def inicializar_glfw():
    if not glfw.init():  # Tenta inicializar o GLFW
        return None  # Se falhar, retorna None
    window = glfw.create_window(800, 800, "Modelo de iluminação Phong", None, None)
    if not window:  # Se falhar ao criar a janela
        glfw.terminate()  # Finaliza o GLFW
        return None  # Retorna None
    glfw.make_context_current(window)  # Torna a janela o contexto OpenGL atual
    return window  # Retorna a janela criada

# Função para configurar as propriedades de iluminação
def configurar_iluminacao():
    glEnable(GL_LIGHTING)  # Habilita a iluminação
    glEnable(GL_LIGHT0)  # Habilita a luz 0 (ambiente)
    glEnable(GL_LIGHT1)  # Habilita a luz 1 (difusa)
    glEnable(GL_LIGHT2)  # Habilita a luz 2 (especular)

    # Configura a luz ambiente
    luz_ambiente = [0.2, 0.2, 0.2, 1.0]  # Cor da luz ambiente (cinza claro)
    glLightfv(GL_LIGHT0, GL_AMBIENT, luz_ambiente)  # Define a cor da luz ambiente
    glLightfv(GL_LIGHT0, GL_DIFFUSE, [0, 0, 0, 1.0])  # Sem componente difusa
    glLightfv(GL_LIGHT0, GL_SPECULAR, [0, 0, 0, 1.0])  # Sem componente especular

    # Configura a luz difusa
    luz_difusa = [0.8, 0.8, 0.8, 1.0]  # Cor da luz difusa (branco)
    glLightfv(GL_LIGHT1, GL_DIFFUSE, luz_difusa)  # Define a cor da luz difusa
    glLightfv(GL_LIGHT1, GL_AMBIENT, [0, 0, 0, 1.0])  # Sem componente ambiente
    glLightfv(GL_LIGHT1, GL_SPECULAR, [0, 0, 0, 1.0])  # Sem componente especular
    glLightfv(GL_LIGHT1, GL_POSITION, [1.0, 0.0, 0.0, 0.0])  # Posição da luz difusa

    # Configura a luz especular
    luz_especular = [1.0, 1.0, 1.0, 1.0]  # Cor da luz especular (branco brilhante)
    glLightfv(GL_LIGHT2, GL_SPECULAR, luz_especular)  # Define a cor da luz especular
    glLightfv(GL_LIGHT2, GL_AMBIENT, [0, 0, 0, 1.0])  # Sem componente ambiente
    glLightfv(GL_LIGHT2, GL_DIFFUSE, [0, 0, 0, 1.0])  # Sem componente difusa
    glLightfv(GL_LIGHT2, GL_POSITION, [1.0, 0.0, 0.0, 0.0])  # Posição da luz especular

# Função para desenhar uma esfera
def desenhar_esfera(raio, slices, stacks):
    # Este loop cria as fatias verticais da esfera
    for i in range(slices):
        # Calcula as coordenadas para duas fatias adjacentes
        lat0 = np.pi * (-0.5 + float(i) / slices)
        z0 = raio * np.sin(lat0)
        zr0 = raio * np.cos(lat0)

        lat1 = np.pi * (-0.5 + float(i + 1) / slices)
        z1 = raio * np.sin(lat1)
        zr1 = raio * np.cos(lat1)

        # Começa a desenhar uma faixa de quadriláteros
        glBegin(GL_QUAD_STRIP)
        # Este loop cria os segmentos horizontais da esfera
        for j in range(stacks + 1):
            lng = 2 * np.pi * float(j) / stacks
            x = np.cos(lng)
            y = np.sin(lng)
            # Define a normal e o vértice para dois pontos adjacentes
            glNormal3f(x * zr0, y * zr0, z0)
            glVertex3f(x * zr0, y * zr0, z0)
            glNormal3f(x * zr1, y * zr1, z1)
            glVertex3f(x * zr1, y * zr1, z1)
        glEnd()  # Termina de desenhar a faixa de quadriláteros

# Função para desenhar uma esfera em uma posição específica
def desenhar_esfera_na_posicao(x, y, modo_de_iluminacao):
    glPushMatrix()  # Salva o estado atual da matriz
    glTranslatef(x, y, -5)  # Move a esfera para a posição desejada

    # Define as propriedades do material da esfera
    cor_verde = [0.0, 1.0, 0.0, 1.0]  # Cor verde
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cor_verde)  # Define cor ambiente e difusa
    glMaterialfv(GL_FRONT, GL_SPECULAR, cor_verde)  # Define cor especular
    glMaterialf(GL_FRONT, GL_SHININESS, 1.0)  # Define o brilho do material

    # Ativa diferentes luzes baseado no modo de iluminação
    if modo_de_iluminacao == 'ambiente':
        glEnable(GL_LIGHT0)
        glDisable(GL_LIGHT1)
        glDisable(GL_LIGHT2)
    elif modo_de_iluminacao == 'difusa':
        glDisable(GL_LIGHT0)
        glEnable(GL_LIGHT1)
        glDisable(GL_LIGHT2)
    elif modo_de_iluminacao == 'especular':
        glDisable(GL_LIGHT0)
        glDisable(GL_LIGHT1)
        glEnable(GL_LIGHT2)
    elif modo_de_iluminacao == 'phong':
        glEnable(GL_LIGHT0)
        glEnable(GL_LIGHT1)
        glEnable(GL_LIGHT2)
    
    desenhar_esfera(0.5, 250, 250)  # Desenha a esfera
    glPopMatrix()  # Restaura o estado anterior da matriz

# Função para configurar a matriz de projeção perspectiva
def configurar_projecao_perspectiva(fovy, aspect, near, far):
    # Implementação própria
    # f = 1.0 / np.tan(fovy * 0.5 * np.pi / 180.0)
    # matrix = np.array([
    #     [f / aspect, 0, 0, 0],
    #     [0, f, 0, 0],
    #     [0, 0, (far + near) / (near - far), -1],
    #     [0, 0, (2 * far * near) / (near - far), 0]
    # ], dtype=np.float32)
    # glLoadMatrixf(matrix)
   
    # Utilizando GLM
    perspective_matrix = glm.perspective(glm.radians(fovy), aspect, near, far)
    glLoadMatrixf(np.array(perspective_matrix, dtype=np.float32))

# Função para renderizar a cena
def renderizar():
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)  # Limpa os buffers de cor e profundidade
    glMatrixMode(GL_PROJECTION)  # Muda para a matriz de projeção
    glLoadIdentity()  # Reseta a matriz de projeção
    
    # Substituindo gluPerspective(45, 1, 1, 10) com nossa função personalizada
    configurar_projecao_perspectiva(45, 1, 1, 10)    

    glMatrixMode(GL_MODELVIEW)  # Muda para a matriz de modelagem
    glLoadIdentity()  # Reseta a matriz de modelagem

    # Desenha as esferas nos quatro quadrantes com diferentes modos de iluminação
    desenhar_esfera_na_posicao(-1.5, 1.5, 'ambiente')
    desenhar_esfera_na_posicao(1.5, 1.5, 'difusa')
    desenhar_esfera_na_posicao(-1.5, -1.5, 'especular')
    desenhar_esfera_na_posicao(1.5, -1.5, 'phong')

# Função principal
def main():
    window = inicializar_glfw()  # Inicializa o GLFW e cria a janela
    if not window:  # Se falhar ao criar a janela
        return  # Encerra o programa

    glEnable(GL_DEPTH_TEST)  # Habilita o teste de profundidade
    configurar_iluminacao()  # Configura a iluminação

    # Loop principal do programa
    while not glfw.window_should_close(window):
        renderizar()  # Renderiza a cena
        glfw.swap_buffers(window)  # Troca os buffers de exibição
        glfw.poll_events()  # Processa eventos da janela

    glfw.terminate()  # Finaliza o GLFW quando o loop termina

if __name__ == "__main__":
    main()  # Chama a função principal