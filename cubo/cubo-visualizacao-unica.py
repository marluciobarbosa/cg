import os
import sys
import glfw
from OpenGL.GL import *
import numpy as np
import glm

# Variáveis globais para a janela
largura_janela = 1024
altura_janela = 768
janela = None

# Variáveis globais para os IDs dos objetos na GPU
id_objeto_array_vertices = None
id_buffer_vertices = None
id_buffer_cores = None
id_buffer_vertices_eixos = None
id_buffer_cores_eixos = None
id_programa = None
id_matriz_mvp = None
MVP = None

def limpar_dados_da_gpu():
    glDeleteBuffers(1, [id_buffer_vertices])
    glDeleteBuffers(1, [id_buffer_cores])
    glDeleteVertexArrays(1, [id_objeto_array_vertices])
    glDeleteProgram(id_programa)

def configurar_mvp():
    global MVP, id_programa, id_matriz_mvp
    # Obtém um identificador para nossa uniforme "MVP"
    id_matriz_mvp = glGetUniformLocation(id_programa, "MVP")
    
    # Matriz de projeção: campo de visão de 45 graus, proporção 4:3, 
    # alcance de exibição: 0.1 unidade <-> 100 unidades
    Projecao = glm.perspective(glm.radians(45.0), 4.0 / 3.0, 0.1, 100.0)
    # Matriz da câmera
    Visualizacao = glm.lookAt(
        glm.vec3(4,3,-3), # A câmera está em (4,3,-3), no Espaço do Mundo
        glm.vec3(0,0,0),  # e olha para a origem
        glm.vec3(0,1,0)   # A cabeça está para cima 
                          # (configure para 0,-1,0 para olhar de cabeça para baixo)
    )
    # Matriz do modelo: uma matriz identidade (o modelo estará na origem)
    Modelo = glm.mat4(1.0)
    # Nossa ModelViewProjection: multiplicação de nossas 3 matrizes
    MVP = Projecao * Visualizacao * Modelo 

def transferir_dados_para_memoria_gpu():
    global id_objeto_array_vertices, id_buffer_vertices, id_buffer_cores, id_buffer_vertices_eixos, id_buffer_cores_eixos, id_programa
    # No início do programa, após inicializar o cubo
    inicializar_dados_eixos()

    # VAO
    id_objeto_array_vertices = glGenVertexArrays(1)
    glBindVertexArray(id_objeto_array_vertices)
    
    # Cria e compila nosso programa GLSL a partir dos shaders
    id_programa = carregar_shaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader")
    
    # Nossos vértices. Três floats consecutivos formam um vértice 3D; Três vértices consecutivos formam um triângulo.
    # Um cubo tem 6 faces com 2 triângulos cada, o que resulta em 6*2=12 triângulos, e 12*3 vértices
    dados_buffer_vertices = np.array([
        -1.0,-1.0,-1.0,  -1.0,-1.0, 1.0,  -1.0, 1.0, 1.0, # Face 1
        -1.0,-1.0,-1.0,  -1.0, 1.0,-1.0,  -1.0, 1.0, 1.0,
        1.0,-1.0, 1.0,   -1.0,-1.0, 1.0,  1.0,-1.0,-1.0, # Face 2
        1.0,-1.0,-1.0,   -1.0,-1.0,-1.0,  -1.0,-1.0, 1.0,
        -1.0, 1.0, 1.0,  1.0, 1.0,-1.0,   1.0, 1.0, 1.0, # Face 3
        -1.0, 1.0, 1.0,  -1.0, 1.0,-1.0,  1.0, 1.0,-1.0,
        -1.0,-1.0,-1.0,  1.0,-1.0,-1.0,   1.0, 1.0,-1.0, # Face 4
        -1.0,-1.0,-1.0,  -1.0, 1.0,-1.0,  1.0, 1.0,-1.0,
        -1.0,-1.0, 1.0,  1.0,-1.0, 1.0,   1.0, 1.0, 1.0, # Face 5
        -1.0,-1.0, 1.0,  -1.0, 1.0, 1.0,  1.0, 1.0, 1.0,
        1.0,-1.0,-1.0,   1.0, 1.0,-1.0,   1.0, 1.0, 1.0, # Face 6
        1.0,-1.0,-1.0,   1.0,-1.0, 1.0,   1.0, 1.0, 1.0
    ], dtype=np.float32)

    # Cores para cada face, repetidas para cada vértice
    dados_buffer_cores = np.array([
        1.0, 0.0, 0.0,  1.0, 0.0, 0.0,  1.0, 0.0, 0.0, # Face 1 Vermelho
        1.0, 0.0, 0.0,  1.0, 0.0, 0.0,  1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,  0.0, 1.0, 0.0,  0.0, 1.0, 0.0, # Face 2 Verde
        0.0, 1.0, 0.0,  0.0, 1.0, 0.0,  0.0, 1.0, 0.0,
        0.0, 0.0, 1.0,  0.0, 0.0, 1.0,  0.0, 0.0, 1.0, # Face 3 Azul
        0.0, 0.0, 1.0,  0.0, 0.0, 1.0,  0.0, 0.0, 1.0,
        1.0, 1.0, 0.0,  1.0, 1.0, 0.0,  1.0, 1.0, 0.0, # Face 4 Amarelo
        1.0, 1.0, 0.0,  1.0, 1.0, 0.0,  1.0, 1.0, 0.0,
        1.0, 0.0, 1.0,  1.0, 0.0, 1.0,  1.0, 0.0, 1.0, # Face 5 Magenta
        1.0, 0.0, 1.0,  1.0, 0.0, 1.0,  1.0, 0.0, 1.0,
        0.0, 1.0, 1.0,  0.0, 1.0, 1.0,  0.0, 1.0, 1.0, # Face 6 Ciano
        0.0, 1.0, 1.0,  0.0, 1.0, 1.0,  0.0, 1.0, 1.0
    ], dtype=np.float32)

    # Move os dados dos vértices para a memória de vídeo; especificamente para o VBO chamado idBufferVertices
    id_buffer_vertices = glGenBuffers(1)
    glBindBuffer(GL_ARRAY_BUFFER, id_buffer_vertices)
    glBufferData(GL_ARRAY_BUFFER, dados_buffer_vertices.nbytes, dados_buffer_vertices, GL_STATIC_DRAW)
    
    # Move os dados das cores para a memória de vídeo; especificamente para o CBO chamado idBufferCores
    id_buffer_cores = glGenBuffers(1)
    glBindBuffer(GL_ARRAY_BUFFER, id_buffer_cores)
    glBufferData(GL_ARRAY_BUFFER, dados_buffer_cores.nbytes, dados_buffer_cores, GL_STATIC_DRAW)

def inicializar_dados_eixos():
    # Cria o VBO para os eixos (parte positiva dos eixos)
    global id_buffer_vertices_eixos, id_buffer_cores_eixos
    dados_buffer_vertices_eixos = np.array([
        0.0, 0.0, 0.0,  4.0, 0.0, 0.0,  # Eixo X (Vermelho)
        0.0, 0.0, 0.0,  0.0, 4.0, 0.0,  # Eixo Y (Verde)
        0.0, 0.0, 0.0,  0.0, 0.0, 4.0   # Eixo Z (Azul)
    ], dtype=np.float32)

    dados_buffer_cores_eixos = np.array([
        1.0, 0.0, 0.0,  1.0, 0.0, 0.0,   # Vermelho
        0.0, 1.0, 0.0,  0.0, 1.0, 0.0,   # Verde
        0.0, 0.0, 1.0,  0.0, 0.0, 1.0    # Azul
    ], dtype=np.float32)

    id_buffer_vertices_eixos = glGenBuffers(1)
    glBindBuffer(GL_ARRAY_BUFFER, id_buffer_vertices_eixos)
    glBufferData(GL_ARRAY_BUFFER, dados_buffer_vertices_eixos.nbytes, dados_buffer_vertices_eixos, GL_STATIC_DRAW)

    id_buffer_cores_eixos = glGenBuffers(1)
    glBindBuffer(GL_ARRAY_BUFFER, id_buffer_cores_eixos)
    glBufferData(GL_ARRAY_BUFFER, dados_buffer_cores_eixos.nbytes, dados_buffer_cores_eixos, GL_STATIC_DRAW)

def carregar_shaders(arquivo_vertice, arquivo_fragmento):
    # Cria os shaders
    shader_vertice = glCreateShader(GL_VERTEX_SHADER)
    shader_fragmento = glCreateShader(GL_FRAGMENT_SHADER)

    # Carrega os shaders
    diretorio_atual = os.path.dirname(sys.argv[0])  # Obtém o diretório atual
    
    with open(os.path.join(diretorio_atual, arquivo_vertice), 'r') as f:
        codigo_shader_vertice = f.read()
    with open(os.path.join(diretorio_atual, arquivo_fragmento), 'r') as f:
        codigo_shader_fragmento = f.read()

    # Compila o shader de vértice
    glShaderSource(shader_vertice, codigo_shader_vertice)
    glCompileShader(shader_vertice)
    # Verifica o shader de vértice
    if not glGetShaderiv(shader_vertice, GL_COMPILE_STATUS):
        error = glGetShaderInfoLog(shader_vertice).decode()
        print(f"Erro na compilação do shader de vértice: {error}")

    # Compila o shader de fragmento
    glShaderSource(shader_fragmento, codigo_shader_fragmento)
    glCompileShader(shader_fragmento)
    # Verifica o shader de fragmento
    if not glGetShaderiv(shader_fragmento, GL_COMPILE_STATUS):
        error = glGetShaderInfoLog(shader_fragmento).decode()
        print(f"Erro na compilação do shader de fragmento: {error}")

    # Linka o programa
    programa = glCreateProgram()
    glAttachShader(programa, shader_vertice)
    glAttachShader(programa, shader_fragmento)
    glLinkProgram(programa)
    # Verifica o programa
    if not glGetProgramiv(programa, GL_LINK_STATUS):
        error = glGetProgramInfoLog(programa).decode()
        print(f"Erro ao linkar programa: {error}")

    glDeleteShader(shader_vertice)
    glDeleteShader(shader_fragmento)

    return programa

def desenhar():
    global id_buffer_vertices, id_buffer_cores, id_programa
    # Limpa a tela
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    
    # Usa nosso shader
    glUseProgram(id_programa)
    
    # Envie nossa transformação para o shader atualmente vinculado, na uniforme "MVP"
    glUniformMatrix4fv(id_matriz_mvp, 1, GL_FALSE, glm.value_ptr(MVP))
    
    # Primeiro buffer de atributo: vértices
    glEnableVertexAttribArray(0)
    glBindBuffer(GL_ARRAY_BUFFER, id_buffer_vertices)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, None)
    
    # Segundo buffer de atributo: cores
    glEnableVertexAttribArray(1)
    glBindBuffer(GL_ARRAY_BUFFER, id_buffer_cores)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, None)

    # Desenha o cubo
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3) # 12*3 é o número total de vértices a serem desenhados

    # Desenha os eixos
    desenhar_eixos()

    glDisableVertexAttribArray(0)
    glDisableVertexAttribArray(1)

def desenhar_eixos():
    global id_buffer_vertices_eixos, id_buffer_cores_eixos
    glEnableVertexAttribArray(0)
    glBindBuffer(GL_ARRAY_BUFFER, id_buffer_vertices_eixos)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, None)

    glEnableVertexAttribArray(1)
    glBindBuffer(GL_ARRAY_BUFFER, id_buffer_cores_eixos)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, None)

    glDrawArrays(GL_LINES, 0, 6)

    glDisableVertexAttribArray(0)
    glDisableVertexAttribArray(1)

def main():
    global janela
    # Inicializa GLFW
    if not glfw.init():
        return

    glfw.window_hint(glfw.SAMPLES, 4)
    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3)
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3)
    glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, GL_TRUE) # Para alegria do MacOS; não deveria ser necessário
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)

    # Abre uma janela
    janela = glfw.create_window(largura_janela, altura_janela, "Um cubo com 12 triângulos", None, None)
    if not janela:
        glfw.terminate()
        return

    # Cria contexto da janela
    glfw.make_context_current(janela)

    # Garante que podemos capturar a tecla ESC sendo pressionada abaixo
    glfw.set_input_mode(janela, glfw.STICKY_KEYS, GL_TRUE)

    # Fundo branco
    glClearColor(1.0, 1.0, 1.0, 0.0)

    # Habilita teste de profundidade
    glEnable(GL_DEPTH_TEST)
    # Aceita o fragmento se ele estiver mais próximo da câmera que o anterior
    glDepthFunc(GL_LESS)

    # Transfere dados (vértices, cores e shaders) para o lado da GPU
    transferir_dados_para_memoria_gpu()

    # Configura a matriz de projeção-visualização-modelo
    configurar_mvp()

    # Renderiza cena para cada frame
    while not glfw.window_should_close(janela):
        # Desenha o cubo
        desenhar()
        # Troca os buffers
        glfw.swap_buffers(janela)
        # Procura por eventos
        glfw.poll_events()

    # Limpa VAO, VBOs e shaders da GPU
    limpar_dados_da_gpu()

    # Fecha a janela OpenGL e termina GLFW
    glfw.terminate()

if __name__ == "__main__":
    main()
