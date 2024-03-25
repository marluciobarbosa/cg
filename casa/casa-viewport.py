from OpenGL.GL import * # Importa todas as funções e constantes da biblioteca OpenGL.GL
# A biblioteca OpenGL.GL fornece funções e constantes para trabalhar com OpenGL em Python

import glfw # Importa a biblioteca glfw para gerenciamento de janelas e entrada de usuário
# A biblioteca glfw (Graphics Library Framework) fornece uma interface simples para criar e gerenciar janelas, junto com entrada de teclado, mouse e joystick

import glm # Importa a biblioteca glm para operações matemáticas e transformações geométricas
# A biblioteca glm (OpenGL Mathematics) fornece classes e funções para operações matemáticas e transformações geométricas usadas em gráficos 3D

import numpy as np # Importa a biblioteca numpy para trabalhar com arrays numéricos
# A biblioteca numpy fornece suporte para trabalhar com arrays numéricos de alta performance em Python

# Objeto de Array de Vértices (VAO)
IDArrayVertices = None # Inicializa uma variável global como None para armazenar o identificador do objeto Vertex Array (VAO)
# O Vertex Array Object (VAO) é um objeto que armazena os buffers de dados de vértices

# Objeto de Buffer de Vértices (VBO)
bufferVertices = None # Inicializa uma variável global como None para armazenar o identificador do objeto Vertex Buffer (VBO)
# O Vertex Buffer Object (VBO) é um objeto que armazena os dados dos vértices (posições, coordenadas de textura, normais, etc.)

# Objeto de Buffer de Cores (CBO)
bufferCores = None # Inicializa uma variável global como None para armazenar o identificador do objeto Color Buffer (CBO)
# O Color Buffer Object (CBO) é um objeto que armazena as cores dos vértices

# Programa GLSL a partir dos shaders
IDPrograma = None # Inicializa uma variável global como None para armazenar o identificador do programa GLSL compilado a partir dos shaders
# O programa GLSL é o resultado da compilação e link dos shaders (vértice e fragmento) em um programa executável pela GPU

# Define o código fonte do shader de vértices como uma string
CodigoShaderVertices = """
#version 330 core
// A linha acima especifica a versão do GLSL (OpenGL Shading Language) que será usada (330 core)

// Entradas de dados de vértices e cores
layout(location = 0) in vec3 posicaoVertice;
// A linha acima declara uma variável de entrada chamada 'posicaoVertice' do tipo vec3 (vetor de 3 componentes) que receberá as coordenadas dos vértices. O layout(location = 0) especifica que esta variável será associada ao índice 0 dos atributos de vértice.

layout(location = 1) in vec3 corVertice;
// A linha acima declara uma variável de entrada chamada 'corVertice' do tipo vec3 (vetor de 3 componentes) que receberá as cores dos vértices. O layout(location = 1) especifica que esta variável será associada ao índice 1 dos atributos de vértice.

// Matriz de transformação da janela para a viewport
uniform mat4 mvp;
// A linha acima declara uma variável uniforme chamada 'mvp' do tipo mat4 (matriz 4x4) que receberá a matriz de transformação da janela para a viewport.

// Dados de saída para o shader de fragmentos
out vec3 corFragmento;
// A linha acima declara uma variável de saída chamada 'corFragmento' do tipo vec3 (vetor de 3 componentes) que enviará as cores interpoladas dos vértices para o shader de fragmentos.

void main() {
// A linha acima define a função principal do shader de vértices.

    // Projeta cada vértice em coordenadas homogêneas
    gl_Position = mvp * vec4(posicaoVertice, 1.0);
    // A linha acima projeta as coordenadas de vértice 'posicaoVertice' em coordenadas homogêneas (4D) usando a matriz de transformação 'mvp'. O resultado é armazenado na variável built-in 'gl_Position', que define a posição final do vértice.

    // O shader de vértices apenas passa a cor para o shader de fragmentos
    corFragmento = corVertice;
    // A linha acima simplesmente atribui a cor do vértice 'corVertice' à variável de saída 'corFragmento', que será interpolada e enviada para o shader de fragmentos.
}
"""
# Essa string contém o código GLSL (OpenGL Shading Language) do shader de vértices.
# Ele recebe as posições dos vértices e cores como entrada, aplica a transformação mvp e passa as cores interpoladas para o shader de fragmentos.

# Define o código fonte do shader de fragmentos como uma string
CodigoShaderFragmentos = """
#version 330 core
// A linha acima especifica a versão do GLSL (OpenGL Shading Language) que será usada (330 core)

// Valores interpolados dos shaders de vértices
in vec3 corFragmento;
// A linha acima declara uma variável de entrada chamada 'corFragmento' do tipo vec3 (vetor de 3 componentes) que receberá as cores interpoladas dos vértices, enviadas pelo shader de vértices.

// Dados de saída (cor final do fragmento)
out vec3 cor;
// A linha acima declara uma variável de saída chamada 'cor' do tipo vec3 (vetor de 3 componentes) que armazenará a cor final do fragmento.

void main() {
// A linha acima define a função principal do shader de fragmentos.

    cor = corFragmento;
    // A linha acima simplesmente atribui o valor da variável de entrada 'corFragmento' (cores interpoladas dos vértices) à variável de saída 'cor', que é a cor final do fragmento.
}
"""
# Essa string contém o código GLSL (OpenGL Shading Language) do shader de fragmentos.
# Ele recebe as cores interpoladas dos vértices e as atribui à cor de saída do fragmento.

def CarregarShaders():
    # Cria os shaders
    IDShaderVertices = glCreateShader(GL_VERTEX_SHADER) # Cria um objeto de shader de vértices
    # A função glCreateShader cria um objeto de shader vazio do tipo especificado (neste caso, GL_VERTEX_SHADER).

    IDShaderFragmentos = glCreateShader(GL_FRAGMENT_SHADER) # Cria um objeto de shader de fragmentos
    # A função glCreateShader cria um objeto de shader vazio do tipo especificado (neste caso, GL_FRAGMENT_SHADER).

    # Compila o Shader de Vértices
    glShaderSource(IDShaderVertices, CodigoShaderVertices) # Define o código fonte do shader de vértices
    # A função glShaderSource define o código fonte GLSL para o objeto de shader especificado (neste caso, o shader de vértices).

    glCompileShader(IDShaderVertices) # Compila o código fonte do shader de vértices
    # A função glCompileShader compila o código fonte GLSL do objeto de shader especificado (neste caso, o shader de vértices).

    # Verifica se o Shader de Vértices compilou corretamente
    if not glGetShaderiv(IDShaderVertices, GL_COMPILE_STATUS):
        raise RuntimeError(glGetShaderInfoLog(IDShaderVertices))
        # A função glGetShaderiv recupera o valor do parâmetro especificado (neste caso, GL_COMPILE_STATUS) para o objeto de shader especificado (neste caso, o shader de vértices).
        # Se o valor do parâmetro GL_COMPILE_STATUS for False, significa que o shader não compilou corretamente.
        # Nesse caso, a função glGetShaderInfoLog recupera o log de erros da compilação, e uma exceção RuntimeError é lançada com esse log.

    # Compila o Shader de Fragmentos
    glShaderSource(IDShaderFragmentos, CodigoShaderFragmentos) # Define o código fonte do shader de fragmentos
    # A função glShaderSource define o código fonte GLSL para o objeto de shader especificado (neste caso, o shader de fragmentos).

    glCompileShader(IDShaderFragmentos) # Compila o código fonte do shader de fragmentos
    # A função glCompileShader compila o código fonte GLSL do objeto de shader especificado (neste caso, o shader de fragmentos).

    # Verifica se o Shader de Fragmentos compilou corretamente
    if not glGetShaderiv(IDShaderFragmentos, GL_COMPILE_STATUS):
        raise RuntimeError(glGetShaderInfoLog(IDShaderFragmentos))
        # A função glGetShaderiv recupera o valor do parâmetro especificado (neste caso, GL_COMPILE_STATUS) para o objeto de shader especificado (neste caso, o shader de fragmentos).
        # Se o valor do parâmetro GL_COMPILE_STATUS for False, significa que o shader não compilou corretamente.
        # Nesse caso, a função glGetShaderInfoLog recupera o log de erros da compilação, e uma exceção RuntimeError é lançada com esse log.

    # Faz o link do programa GLSL combinando os shaders compilados
    IDPrograma = glCreateProgram() # Cria um objeto de programa GLSL
    # A função glCreateProgram cria um objeto de programa GLSL vazio.

    glAttachShader(IDPrograma, IDShaderVertices) # Anexa o shader de vértices ao programa
    # A função glAttachShader anexa o objeto de shader especificado (neste caso, o shader de vértices) ao objeto de programa GLSL especificado.

    glAttachShader(IDPrograma, IDShaderFragmentos) # Anexa o shader de fragmentos ao programa
    # A função glAttachShader anexa o objeto de shader especificado (neste caso, o shader de fragmentos) ao objeto de programa GLSL especificado.

    glLinkProgram(IDPrograma) # Faz o link do programa combinando os shaders anexados
    # A função glLinkProgram faz o link (combina) todos os objetos de shader anexados ao objeto de programa GLSL especificado, criando um programa executável pela GPU.

    # Verifica se o programa GLSL foi linkado corretamente
    if not glGetProgramiv(IDPrograma, GL_LINK_STATUS):
        raise RuntimeError(glGetProgramInfoLog(IDPrograma))
        # A função glGetProgramiv recupera o valor do parâmetro especificado (neste caso, GL_LINK_STATUS) para o objeto de programa GLSL especificado.
        # Se o valor do parâmetro GL_LINK_STATUS for False, significa que o programa não foi linkado corretamente.
        # Nesse caso, a função glGetProgramInfoLog recupera o log de erros do link, e uma exceção RuntimeError é lançada com esse log.

    glDeleteShader(IDShaderVertices) # Exclui o objeto de shader de vértices, pois ele já foi incorporado ao programa
    # Depois que o programa GLSL é linkado com sucesso, os objetos de shader individuais podem ser excluídos, já que eles foram incorporados ao programa.

    glDeleteShader(IDShaderFragmentos) # Exclui o objeto de shader de fragmentos, pois ele já foi incorporado ao programa
    # Depois que o programa GLSL é linkado com sucesso, os objetos de shader individuais podem ser excluídos, já que eles foram incorporados ao programa.

    return IDPrograma # Retorna o identificador do programa GLSL linkado

def TransferirDadosParaGPU():
    global IDArrayVertices, IDPrograma, bufferVertices, bufferCores

    # VAO (Vertex Array Object)
    IDArrayVertices = glGenVertexArrays(1) # Gera um objeto Vertex Array (VAO)
    # A função glGenVertexArrays gera um novo objeto Vertex Array (VAO) e retorna seu identificador.

    glBindVertexArray(IDArrayVertices) # Vincula o VAO criado como o VAO ativo
    # A função glBindVertexArray vincula o objeto VAO especificado como o VAO ativo.

    # Cria e compila o programa GLSL a partir dos shaders
    IDPrograma = CarregarShaders() # Carrega os shaders e cria o programa GLSL
    # A função CarregarShaders é chamada para compilar os shaders de vértices e fragmentos, e criar o programa GLSL linkado.

    # Dados de vértices para 3 triângulos
    dadosBufferVertices = np.array([
        0.0, 0.0, 0.0, 20.0,  0.0, 0.0, 20.0, 20.0, 0.0,  # primeiro triângulo
        0.0, 0.0, 0.0, 20.0, 20.0, 0.0, 0.0, 20.0, 0.0,   # segundo triângulo
        0.0, 20.0, 0.0, 20.0, 20.0, 0.0, 10.0, 30.0, 0.0  # terceiro triângulo
    ], dtype=np.float32)
    # Define um array numpy com as coordenadas dos vértices para três triângulos
    # Cada grupo de 3 valores (x, y, z) representa um vértice, e cada triângulo é definido por 3 vértices.

    # Uma cor para cada vértice
    dadosBufferCores = np.array([
        1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0,  # cor para os 3 vértices do primeiro triângulo
        1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0,  # cor para os 3 vértices do segundo triângulo
        0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0   # cor para os 3 vértices do terceiro triângulo
    ], dtype=np.float32)
    # Define um array numpy com as cores para cada vértice dos triângulos
    # Cada grupo de 3 valores (r, g, b) representa a cor de um vértice.

    # Move os dados de vértices para a memória de vídeo, especificamente para o VBO chamado bufferVertices
    bufferVertices = glGenBuffers(1) # Gera um objeto Vertex Buffer (VBO)
    # A função glGenBuffers gera um novo objeto de buffer e retorna seu identificador.

    glBindBuffer(GL_ARRAY_BUFFER, bufferVertices) # Vincula o VBO criado como o VBO ativo
    # A função glBindBuffer vincula o objeto de buffer especificado (neste caso, o VBO) como o buffer ativo do tipo especificado (neste caso, GL_ARRAY_BUFFER).

    glBufferData(GL_ARRAY_BUFFER, dadosBufferVertices, GL_STATIC_DRAW)
    # A função glBufferData transfere os dados do array numpy 'dadosBufferVertices' para a memória de vídeo (VBO ativo).
    # O parâmetro GL_STATIC_DRAW indica que os dados não serão modificados e serão usados apenas para renderização.

    # Move os dados de cores para a memória de vídeo, especificamente para o CBO chamado bufferCores
    bufferCores = glGenBuffers(1) # Gera um objeto Color Buffer (CBO)
    glBindBuffer(GL_ARRAY_BUFFER, bufferCores) # Vincula o CBO criado como o VBO ativo
    glBufferData(GL_ARRAY_BUFFER, dadosBufferCores, GL_STATIC_DRAW)
    # Transfere os dados das cores do array numpy para a memória de vídeo (CBO)

def LimparDadosDaGPU():
    global IDArrayVertices, IDPrograma, bufferVertices, bufferCores
    glDeleteBuffers(1, [bufferVertices]) # Exclui o objeto VBO da GPU
    glDeleteBuffers(1, [bufferCores]) # Exclui o objeto CBO da GPU 
    glDeleteVertexArrays(1, [IDArrayVertices]) # Exclui o objeto VAO da GPU
    glDeleteProgram(IDPrograma) # Exclui o programa GLSL da GPU

def Desenhar():
    global IDPrograma, bufferVertices, bufferCores

    # Limpa o buffer de cores da tela
    # glClear(GL_COLOR_BUFFER_BIT) # Limpa o buffer de cor da tela

    # Utiliza o programa GLSL criado  
    glUseProgram(IDPrograma) # Ativa o programa GLSL criado

    # Transformação da janela para a viewport
    mvp = glm.ortho(-40.0, 40.0, -40.0, 40.0) # Cria uma matriz de projeção ortogonal

    # Obtém o local da variável uniforme da matriz de transformação
    matrizUniforme = glGetUniformLocation(IDPrograma, "mvp") # Obtém o local da variável uniforme "mvp" no programa GLSL
    glUniformMatrix4fv(matrizUniforme, 1, GL_FALSE, glm.value_ptr(mvp))
    # Envia a matriz mvp para a variável uniforme "mvp" no programa GLSL

    # Primeiro buffer de atributos: vértices
    glEnableVertexAttribArray(0) # Habilita o atributo de vértice no índice 0
    glBindBuffer(GL_ARRAY_BUFFER, bufferVertices) # Vincula o VBO com os dados de vértices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, None)
    # Define o ponteiro para o atributo de vértice no índice 0, com 3 componentes (x, y, z), tipo float

    # Segundo buffer de atributos: cores
    glEnableVertexAttribArray(1) # Habilita o atributo de cor no índice 1  
    glBindBuffer(GL_ARRAY_BUFFER, bufferCores) # Vincula o CBO com os dados de cores
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, None)
    # Define o ponteiro para o atributo de cor no índice 1, com 3 componentes (r, g, b), tipo float

    # Desenha os 3 triângulos!
    glDrawArrays(GL_TRIANGLES, 0, 9) # Desenha 3 triângulos a partir dos 9 vértices fornecidos

    # Desabilita os arrays de atributos para vértices
    glDisableVertexAttribArray(0) # Desabilita o atributo de vértice no índice 0
    glDisableVertexAttribArray(1) # Desabilita o atributo de cor no índice 1

def main():
    global IDPrograma, IDArrayVertices, bufferVertices, bufferCores

    # Inicializa a biblioteca glfw
    if not glfw.init(): # Chama a função init() da biblioteca glfw para inicializá-la
        # Se a inicialização falhar, a função retorna False
        return -1 # Retorna -1 para indicar que o programa falhou

    # Configurações da versão OpenGL e compatibilidade
    glfw.window_hint(glfw.SAMPLES, 4) # Define o número de amostras para anti-aliasing (suavização de bordas) como 4
    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3) # Define a versão principal do contexto OpenGL como 3
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3) # Define a versão secundária do contexto OpenGL como 3
    glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, GL_TRUE) # Habilita o modo de compatibilidade avançado do OpenGL
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE) # Define o perfil de contexto OpenGL como núcleo

    # Cria uma janela de modo windowed e seu contexto OpenGL
    janela = glfw.create_window(1024, 768, "Casa em vermelho e verde", None, None)
    # Cria uma janela de modo windowed (sem bordas) com resolução 1024x768 e título "Casa em vermelho e verde"
    # Os dois None no final são para ignorar os parâmetros de monitor e janela compartilhada
    if not janela: # Se a janela não foi criada com sucesso
        glfw.terminate() # Termina a biblioteca glfw
        return -1 # Retorna -1 para indicar que o programa falhou

    # Define o contexto OpenGL da janela como o contexto atual
    glfw.make_context_current(janela) # Define o contexto OpenGL da janela como o contexto atual para renderização

    # Transfere os dados (vértices, cores e shaders) para a memória da GPU
    TransferirDadosParaGPU() # Chama a função TransferirDadosParaGPU() para transferir os dados para a GPU

    # Divide a janela em 4 regiões e desenha em cada uma delas
    largura_janela, altura_janela = glfw.get_window_size(janela) # Obtém a largura e altura da janela
    largura_regiao = largura_janela // 2 # Calcula a largura de cada região (metade da largura da janela)
    altura_regiao = altura_janela // 2 # Calcula a altura de cada região (metade da altura da janela)

    FramebufferName = 0 # Inicializa o nome do framebuffer como 0
    glGenFramebuffers(1, FramebufferName) # Gera um framebuffer (objeto que armazena a imagem renderizada)
    glClear(GL_COLOR_BUFFER_BIT) # Limpa o buffer de cor da tela (preenche com a cor de fundo)

    # Loop até que o usuário feche a janela
    while not glfw.window_should_close(janela): # Enquanto a janela não for fechada
        # Renderiza aqui, por exemplo, usando pyOpenGL
        glBindFramebuffer(GL_FRAMEBUFFER, 0) # Vincula o framebuffer padrão (a janela)
        for i in range(2): # Itera sobre as linhas das regiões
            for j in range(2): # Itera sobre as colunas das regiões
                # Define a viewport para a região atual
                glViewport(j * largura_regiao, i * altura_regiao, largura_regiao, altura_regiao)
                # Define a região retangular da janela que será renderizada
                # Desenha nesta região
                Desenhar() # Chama a função Desenhar() para renderizar nesta região

        # Troca os buffers de frente e fundo da janela
        glfw.swap_buffers(janela) # Troca os buffers de frente e fundo da janela (double buffering)

        # Processa eventos de entrada (teclado, mouse, etc.)
        glfw.poll_events() # Processa eventos de entrada (teclado, mouse, etc.)

    # Exclui o framebuffer
    glDeleteFramebuffers(1, FramebufferName) # Exclui o framebuffer da GPU

    # Limpa os objetos OpenGL da GPU
    LimparDadosDaGPU() # Chama a função LimparDadosDaGPU() para excluir os objetos OpenGL da GPU

    glfw.terminate() # Termina a biblioteca glfw

if __name__ == "__main__":
    main() # Chama a função main() se este script for executado diretamente