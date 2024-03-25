import glfw # Importa a biblioteca GLFW (Graphics Library Framework) para criar janelas e gerenciar eventos.
from OpenGL.GL import * # Importa todas as funções e constantes da biblioteca OpenGL.
import numpy as np # Importa a biblioteca NumPy para operações numéricas e manipulação de arrays.
import glm # Importa a biblioteca GLM (OpenGL Mathematics) para operações matemáticas.

# Variáveis globais para o programa
VertexArrayID = None # Identificador do Vertex Array Object (VAO) inicializado como None.
programID = None # Identificador do programa de shader inicializado como None.
vertexbuffer = None # Identificador do buffer de vértices inicializado como None.
colorbuffer = None # Identificador do buffer de cores inicializado como None.

# Código fonte dos shaders vertex e fragment (são programas executados na GPU)
vertex_shader_code = """
#version 330 core // Especifica a versão do GLSL (OpenGL Shading Language) como 3.30 core.
layout(location = 0) in vec3 vertexPosition; // Declara um atributo de entrada para as posições dos vértices (vec3 = vetor de 3 elementos) com layout location 0.
layout(location = 1) in vec3 vertexColor; // Declara um atributo de entrada para as cores dos vértices com layout location 1.

uniform mat4 mvp; // Declara uma variável uniforme (constante durante a execução do shader) para a matriz de projeção.
uniform mat4 trans; // Declara uma variável uniforme para a matriz de transformação.

out vec3 fragmentColor; // Declara uma variável de saída para a cor do fragmento.

void main() { // Função principal do vertex shader.
    gl_Position = mvp * trans * vec4(vertexPosition, 1.0); // Calcula a posição final do vértice.
    fragmentColor = vertexColor; // Passa a cor do vértice para o fragment shader.
}
"""

fragment_shader_code = """
#version 330 core // Especifica a versão do GLSL como 3.30 core.
in vec3 fragmentColor; // Declara uma variável de entrada para a cor do fragmento.
out vec3 color; // Declara uma variável de saída para a cor final do fragmento.

void main() { // Função principal do fragment shader.
    color = vec3(fragmentColor); // Define a cor final do fragmento.
}
"""

# protótipos de função
def compileShader(source, shader_type): # Função para compilar um shader.
    shader = glCreateShader(shader_type) # Cria um shader do tipo especificado.
    glShaderSource(shader, source) # Associa o código fonte ao shader.
    glCompileShader(shader) # Compila o shader.
    if glGetShaderiv(shader, GL_COMPILE_STATUS) != GL_TRUE: # Se a compilação falhou.
        print(glGetShaderInfoLog(shader)) # Imprime o log de erros do shader.
        return None # Retorna None para indicar falha.
    return shader # Retorna o identificador do shader compilado.

def transferDataToGPUMemory(): # Função para transferir dados para a memória da GPU.
    global VertexArrayID, programID, vertexbuffer, colorbuffer
    # VAO (Vertex Array Object)
    VertexArrayID = glGenVertexArrays(1) # Gera um novo Vertex Array Object (VAO).
    glBindVertexArray(VertexArrayID) # Vincula o VAO para uso.
    # Compilar e vincular shaders
    programID = glCreateProgram() # Cria um novo programa de shader.
    vertex_shader = compileShader(vertex_shader_code, GL_VERTEX_SHADER) # Compila o vertex shader.
    fragment_shader = compileShader(fragment_shader_code, GL_FRAGMENT_SHADER) # Compila o fragment shader.
    glAttachShader(programID, vertex_shader) # Anexa o vertex shader ao programa.
    glAttachShader(programID, fragment_shader) # Anexa o fragment shader ao programa.
    glLinkProgram(programID) # Vincula os shaders ao programa.
    # Vértices e cores dos componentes da casa
    g_vertex_buffer_data = np.array([
        # Telhado (triângulo)
        -10.0, 20.0, 0.0, # Vértice 1 do telhado
        10.0, 20.0, 0.0, # Vértice 2 do telhado
        0.0, 30.0, 0.0, # Vértice 3 do telhado
        # Paredes (retângulo)
        -10.0, 10.0, 0.0, # Vértice 1 da parede esquerda
        -10.0, 20.0, 0.0, # Vértice 2 da parede esquerda
        10.0, 20.0, 0.0, # Vértice 3 da parede direita
        -10.0, 10.0, 0.0, # Vértice 1 da parede esquerda (novamente)
        10.0, 20.0, 0.0, # Vértice 2 da parede direita (novamente)
        10.0, 10.0, 0.0 # Vértice 3 da parede direita
    ], dtype=np.float32) # Cria um array NumPy com os dados dos vértices.
    vertexbuffer = glGenBuffers(1) # Gera um novo buffer de vértices.
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer) # Vincula o buffer de vértices para uso.
    glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.nbytes, g_vertex_buffer_data, GL_STATIC_DRAW) # Envia os dados dos vértices para o buffer.
    g_color_buffer_data = np.array([
         # Telhado (cor vermelha)
        1.0, 0.0, 0.0, # Cor vermelha para o vértice 1 do telhado
        1.0, 0.0, 0.0, # Cor vermelha para o vértice 2 do telhado
        1.0, 0.0, 0.0, # Cor vermelha para o vértice 3 do telhado
        # Paredes (cor azul)
        0.0, 0.0, 1.0, # Cor azul para o vértice 1 da parede esquerda
        0.0, 0.0, 1.0, # Cor azul para o vértice 2 da parede esquerda
        0.0, 0.0, 1.0, # Cor azul para o vértice 3 da parede direita
        0.0, 0.0, 1.0, # Cor azul para o vértice 1 da parede esquerda (novamente)
        0.0, 0.0, 1.0, # Cor azul para o vértice 2 da parede direita (novamente)
        0.0, 0.0, 1.0 # Cor azul para o vértice 3 da parede direita
    ], dtype=np.float32) # Cria um array NumPy com os dados das cores.
    colorbuffer = glGenBuffers(1) # Gera um novo buffer de cores.
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer) # Vincula o buffer de cores para uso.
    glBufferData(GL_ARRAY_BUFFER, g_color_buffer_data.nbytes, g_color_buffer_data, GL_STATIC_DRAW) # Envia os dados das cores para o buffer.

def cleanupDataFromGPU(): # Função para limpar os dados da GPU.
    global VertexArrayID, programID, vertexbuffer, colorbuffer
    glDeleteBuffers(1, [vertexbuffer]) # Exclui o buffer de vértices.
    glDeleteBuffers(1, [colorbuffer]) # Exclui o buffer de cores.
    glDeleteVertexArrays(1, [VertexArrayID]) # Exclui o Vertex Array Object (VAO).
    glDeleteProgram(programID) # Exclui o programa de shader.

def draw(): # Função para desenhar a cena.
    global delta, VertexArrayID, programID, vertexbuffer, colorbuffer
    # Limpar a tela
    glClear(GL_COLOR_BUFFER_BIT) # Limpa o buffer de cor.
    # Use nosso shader
    glUseProgram(programID) # Ativa o programa de shader.
    # Criar o domínio da cena
    mvp = glm.ortho(-40.0, 40.0, -40.0, 40.0) # Cria a matriz de projeção ortogonal.
    # Recupere os locais uniformes da matriz
    matrix = glGetUniformLocation(programID, "mvp") # Obtém a localização da variável uniforme "mvp".
    glUniformMatrix4fv(matrix, 1, GL_FALSE, glm.value_ptr(mvp)) # Envia a matriz de projeção para o shader.
    trans = glm.mat4(1.0) # Cria uma matriz de transformação identidade.
    trans = glm.translate(trans, glm.vec3(delta, delta, 0.0)) # Aplica uma translação à matriz de transformação.
    m = glGetUniformLocation(programID, "trans") # Obtém a localização da variável uniforme "trans".
    glUniformMatrix4fv(m, 1, GL_FALSE, glm.value_ptr(trans)) # Envia a matriz de transformação para o shader.
    # Primeiro buffer de atributos: vértices
    glEnableVertexAttribArray(0) # Habilita o atributo de vértices.
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer) # Vincula o buffer de vértices.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, None) # Configura o ponteiro de atributo para os vértices.
    # 2º buffer de atributos: cores
    glEnableVertexAttribArray(1) # Habilita o atributo de cores.
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer) # Vincula o buffer de cores.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, None) # Configura o ponteiro de atributo para as cores.
    # Desenhe os componentes da casa
    glDrawArrays(GL_TRIANGLES, 0, 3) # Desenha o telhado (triângulo).
    glDrawArrays(GL_TRIANGLES, 3, 6) # Desenha as paredes (retângulo).
    # Desativar matrizes de atributos para vértices
    glDisableVertexAttribArray(0) # Desabilita o atributo de vértices.
    glDisableVertexAttribArray(1) # Desabilita o atributo de cores.

def main(): # Função principal.
    global window, delta, VertexArrayID, programID, vertexbuffer, colorbuffer
    # Inicialização do GLFW
    if not glfw.init(): # Se a inicialização do GLFW falhar.
        return # Retorna sem fazer nada.
    # Configuração da versão OpenGL e similares
    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3) # Define a versão principal do contexto OpenGL para 3.
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3) # Define a versão secundária do contexto OpenGL para 3.
    glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, GL_TRUE) # Habilita a compatibilidade futura do OpenGL.
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE) # Define o perfil do OpenGL para core.
    # Abrir uma janela
    window = glfw.create_window(800, 800, "Movimentação de casa em 2D", None, None) # Cria uma janela de 800x800 pixels com o título "Movimentação de casa em 2D".
    if not window: # Se a criação da janela falhar.
        glfw.terminate() # Termina o GLFW.
        return # Retorna sem fazer nada.
    # Criar contexto de janela
    glfw.make_context_current(window) # Define o contexto OpenGL atual para a janela criada.
    # Certifique-se de que podemos capturar a tecla de escape sendo pressionada abaixo
    glfw.set_input_mode(window, glfw.STICKY_KEYS, GL_TRUE) # Define o modo de entrada para capturar teclas pressionadas mesmo após serem soltas.
    # Fundo branco
    glClearColor(1.0, 1.0, 1.0, 0.0) # Define a cor de fundo como branca.
    # Transferir meus dados (vértices, cores e shaders) para o lado da GPU
    transferDataToGPUMemory()
    # Renderizar a cena para cada frame
    delta = 0.0 # Inicializa o deslocamento como zero.
    while not glfw.window_should_close(window): # Loop enquanto a janela não for fechada.
        draw() # Chama a função draw() para desenhar a cena.
        glfw.swap_buffers(window) # Troca os buffers de renderização (double buffering).
        glfw.poll_events() # Processa eventos de entrada (teclado, mouse, etc.).
        if delta < 10: # Se o deslocamento for menor que 10.
            delta += 0.05 # Incrementa o deslocamento.
    # Limpeza de VAO (Vertex Array Object), VBO (Vertex Buffer Object): e shaders da GPU
    cleanupDataFromGPU()
    # Fechar a janela OpenGL e encerrar o GLFW

if __name__ == "__main__": # Verifica se este é o script principal (não um módulo importado).
    main() # Chama a função principal.