# Inclui a biblioteca GLFW para criar janelas e contextos OpenGL
import glfw
# Inclui a biblioteca OpenGL para renderização 3D
from OpenGL.GL import *
# Inclui a biblioteca NumPy para manipulação eficiente de arrays
import numpy as np
# Inclui a biblioteca de shaders OpenGL para compilar shaders de vértices e fragmentos
from OpenGL.GL import shaders

# Define o shader de vértices como uma string contendo o código do shader
vertex_shader = """
#version 330 core                         // Declara a versão do OpenGL utilizada
in vec3 coord3d;                          // Define o atributo de entrada para as coordenadas 3D dos vértices
void main(void) {                         // Função principal do shader de vértices
    gl_Position = vec4(coord3d, 1.0);     // Define a posição do vértice na cena
}
"""

# Define o shader de fragmentos como uma string contendo o código do shader
fragment_shader = """
#version 330 core                         // Declara a versão do OpenGL utilizada
uniform vec4 color;                       // Define uma variável uniforme para a cor do fragmento
out vec4 frag_color;                      // Define a cor de saída do fragmento
void main(void) {                         // Função principal do shader de fragmentos
    frag_color = color;                   // Define a cor do fragmento como a cor uniforme especificada
}
"""

# Variáveis globais para armazenar informações e recursos da aplicação
shaderProgram = None           # Programa de shader compilado
attribute_coord3d = None       # Localização do atributo de coordenadas 3D nos shaders
window = None                  # Janela GLFW
vao = None                     # Array de vértices OpenGL
vbo = None                     # Buffer de vértices OpenGL

# Função chamada ao exibir a janela OpenGL
def onDisplay():
    global window, vao, vbo
    # Limpa os buffers de cor e profundidade
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    # Usa o programa de shader especificado
    glUseProgram(shaderProgram)
    
    # Vincula o buffer de vértices
    glBindVertexArray(vao)
    glEnableVertexAttribArray(attribute_coord3d)
    
    # Define cores para os triângulos
    red = [1.0, 0.0, 0.0, 1.0]    # Cor para o primeiro triângulo (abaixo)
    green = [0.0, 1.0, 0.0, 1.0]  # Cor para o segundo triângulo (acima)

    # Desenha o primeiro triângulo (abaixo) com a cor vermelha
    glUniform4fv(glGetUniformLocation(shaderProgram, "color"), 1, red)
    # Desenha os vértices do primeiro triângulo
    glDrawArrays(GL_TRIANGLES, 0, 3)

    # Desenha o segundo triângulo (acima) com a cor verde
    glUniform4fv(glGetUniformLocation(shaderProgram, "color"), 1, green)
    # Desenha os vértices do segundo triângulo
    glDrawArrays(GL_TRIANGLES, 3, 3)

    # Desabilita o atributo de coordenadas 3D após desenhar
    glDisableVertexAttribArray(attribute_coord3d)
    # Troca os buffers da janela OpenGL
    glfw.swap_buffers(window)

# Libera os recursos alocados na GPU
def free_resources():
    global shaderProgram, vao, vbo
    # Deleta o programa de shader
    glDeleteProgram(shaderProgram)
    # Deleta o array de vértices
    glDeleteVertexArrays(1, [vao])
    # Deleta o buffer de vértices
    glDeleteBuffers(1, [vbo])

# Inicializa os recursos necessários na GPU
def init_resources():
    global shaderProgram, attribute_coord3d, vao, vbo
    # Gera e vincula o array de vértices
    vao = glGenVertexArrays(1) # Gera o array de vértices
    glBindVertexArray(vao)     # Vincula o array de vértices

    # Define as coordenadas dos vértices
    vertices = np.array([
        -1.0, -1.0, 0.0,  # Vértice 1 (abaixo, esquerda)
         1.0, -1.0, 0.0,  # Vértice 2 (abaixo, direita)
         0.0,  1.0, 0.0,  # Vértice 3 (acima, centro)
        -1.0,  1.0, 0.0,  # Vértice 4 (acima, esquerda)
         1.0,  1.0, 0.0,  # Vértice 5 (acima, direita)
         0.0, -1.0, 0.0,  # Vértice 6 (abaixo, centro)
    ], dtype=np.float32)

    # Gera e vincula o buffer de vértices
    vbo = glGenBuffers(1)                                   # Gera o buffer de vértices
    glBindBuffer(GL_ARRAY_BUFFER, vbo)                      # Vincula o buffer de vértices
    glBufferData(GL_ARRAY_BUFFER, vertices, GL_STATIC_DRAW) # Preenche o buffer de vértices com os dados
    
    # Compila e vincula os shaders de vértices e fragmentos
    vs = shaders.compileShader(vertex_shader, GL_VERTEX_SHADER)      # Cria um shader de vértices
    fs = shaders.compileShader(fragment_shader, GL_FRAGMENT_SHADER)  # Cria um shader de fragmentos
    shaderProgram = shaders.compileProgram(vs, fs)                   # Cria um programa de shader
    
    # Obtém a localização do atributo de coordenadas 3D
    attribute_name = "coord3d" # Obtém a localização do atributo de coordenadas 3D
    attribute_coord3d = glGetAttribLocation(shaderProgram, attribute_name)
    if attribute_coord3d == -1: # Verifica se o atributo foi vinculado corretamente
        print(f"Não foi possível vincular o atributo {attribute_name}")
        return

    # Define os atributos dos vértices
    glVertexAttribPointer(attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, ctypes.c_void_p(0)) # Define os atributos dos vértices
    glEnableVertexAttribArray(attribute_coord3d) #  Habilita o atributo de coordenadas 3D

# Callback para eventos de teclado
def key_callback(window, key, scancode, action, mods):
    # Verifica se a tecla ESC foi pressionada e define a flag de fechamento da janela
    if key == glfw.KEY_ESCAPE and action == glfw.PRESS:
        glfw.set_window_should_close(window, True) # Fecha a janela GLFW

# Função principal
def main():
    global window
    # Inicialização do GLFW
    if not glfw.init():
        return
    # Configuração da versão OpenGL e similares
    glfw.window_hint(glfw.SAMPLES, 4) # Configura o número de amostras para anti-aliasing
    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3) # Configura a versão principal do contexto OpenGL
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3) # Configura a versão secundária do contexto OpenGL
    glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, GL_TRUE) # Habilita o modo de compatibilidade avançado OpenGL
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE) # Configura o perfil de contexto OpenGL para núcleo
    # Cria uma janela OpenGL
    window = glfw.create_window(640, 480, "Triângulos OpenGL", None, None)
    if not window: # Verifica se a janela foi criada corretamente
        glfw.terminate() # Encerra a biblioteca GLFW
        return
    
    # Define a janela como contexto atual
    glfw.make_context_current(window)
    # Ativa o teste de profundidade para renderização 3D
    glEnable(GL_DEPTH_TEST)
    # Inicializa os recursos na GPU
    init_resources()

    # Configura o callback para eventos de teclado
    glfw.set_key_callback(window, key_callback)

    # Loop principal de renderização
    while not glfw.window_should_close(window):
        # Renderiza a cena
        onDisplay()
        # Processa eventos da janela
        glfw.poll_events()

    # Libera os recursos alocados na GPU
    free_resources()
    # Termina o GLFW
    glfw.terminate()

# Executa a função main quando este script é executado diretamente
if __name__ == "__main__":
    main()
