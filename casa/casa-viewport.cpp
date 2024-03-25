// Inclui as bibliotecas necessárias
#include <iostream> // Biblioteca padrão de entrada/saída em C++
#include <GL/glew.h> // Biblioteca GLEW (OpenGL Extension Wrangler Library) para carregar funções OpenGL
#include <GLFW/glfw3.h> // Biblioteca GLFW (Graphics Library Framework) para gerenciamento de janelas e entrada
#include <glm/glm.hpp> // Biblioteca GLM (OpenGL Mathematics) para operações matemáticas
#include <glm/gtc/matrix_transform.hpp> // Extensão da biblioteca GLM para transformações geométricas
#include <glm/gtc/type_ptr.hpp> // Extensão da biblioteca GLM para conversão de tipos

// Protótipos de funções (declarações)
GLuint CarregarShaders(); // Função para carregar e compilar os shaders
void TransferirDadosParaGPU(); // Função para transferir dados para a GPU
void LimparDadosDaGPU(); // Função para limpar dados da GPU
void Desenhar(); // Função para desenhar na janela


// Objeto de Array de Vértices (VAO)
GLuint IDArrayVertices; // Inicializa uma variável global como None para armazenar o identificador do objeto Vertex Array (VAO)
// O Vertex Array Object (VAO) é um objeto que armazena os buffers de dados de vértices

// Objeto de Buffer de Vértices (VBO)
GLuint bufferVertices; // Inicializa uma variável global como None para armazenar o identificador do objeto Vertex Buffer (VBO)
// O Vertex Buffer Object (VBO) é um objeto que armazena os dados dos vértices (posições, coordenadas de textura, normais, etc.)

// Objeto de Buffer de Cores (CBO)
GLuint bufferCores; // Inicializa uma variável global como None para armazenar o identificador do objeto Color Buffer (CBO)
// O Color Buffer Object (CBO) é um objeto que armazena as cores dos vértices

// Programa GLSL
GLuint IDPrograma; // Inicializa uma variável global como None para armazenar o identificador do programa GLSL compilado a partir dos shaders
// O programa GLSL é o resultado da compilação e link dos shaders (vértice e fragmento) em um programa executável pela GPU

// Define o código fonte do shader de vértices como uma string
const char* CodigoShaderVertices = R"(
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
)";

// Define o código fonte do shader de fragmentos como uma string
const char* CodigoShaderFragmentos = R"(
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
)";

// Função principal
int main() {
    // Inicializa a biblioteca GLFW
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar GLFW" << std::endl; // Imprime mensagem de erro no fluxo de erro padrão
        return -1; // Retorna um valor de erro (-1) para indicar que o programa falhou
    }

    // Configurações da versão OpenGL e compatibilidade
    glfwWindowHint(GLFW_SAMPLES, 4); // Define o número de amostras para anti-aliasing (suavização de bordas) como 4
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Define a versão principal do contexto OpenGL como 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Define a versão secundária do contexto OpenGL como 3
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Habilita o modo de compatibilidade avançado do OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Define o perfil de contexto OpenGL como núcleo

    // Cria uma janela de modo windowed e seu contexto OpenGL
    GLFWwindow* janela = glfwCreateWindow(1024, 768, "Casa em vermelho e verde", nullptr, nullptr);
    // Cria uma janela de modo windowed (sem bordas) com resolução 1024x768 e título "Casa em vermelho e verde"
    // Os dois nullptr no final são para ignorar os parâmetros de monitor e janela compartilhada
    if (!janela) {
        std::cerr << "Falha ao criar janela GLFW" << std::endl; // Imprime mensagem de erro no fluxo de erro padrão
        glfwTerminate(); // Termina a biblioteca GLFW
        return -1; // Retorna um valor de erro (-1) para indicar que o programa falhou
    }

    // Define o contexto OpenGL da janela como o contexto atual
    glfwMakeContextCurrent(janela); // Define o contexto OpenGL da janela como o contexto atual para renderização

    // Inicializa a biblioteca GLEW
    glewExperimental = GL_TRUE; // Necessário para utilizar funções modernas do OpenGL
    if (glewInit() != GLEW_OK) {
        std::cerr << "Falha ao inicializar GLEW" << std::endl; // Imprime mensagem de erro no fluxo de erro padrão
        glfwTerminate(); // Termina a biblioteca GLFW
        return -1; // Retorna um valor de erro (-1) para indicar que o programa falhou
    }

    // Transfere os dados (vértices, cores e shaders) para a memória da GPU
    TransferirDadosParaGPU(); // Chama a função para transferir dados para a GPU
    CarregarShaders(); // Chama a função para carregar e compilar os shaders

    // Divide a janela em 4 regiões e desenha em cada uma delas
    int larguraJanela, alturaJanela;
    glfwGetFramebufferSize(janela, &larguraJanela, &alturaJanela); // Obtém a largura e altura da janela
    int larguraRegiao = larguraJanela / 2; // Calcula a largura de cada região (metade da largura da janela)
    int alturaRegiao = alturaJanela / 2; // Calcula a altura de cada região (metade da altura da janela)

    GLuint framebufferID; // Identificador do framebuffer
    glGenFramebuffers(1, &framebufferID); // Gera um framebuffer (objeto que armazena a imagem renderizada)

    // Loop até que o usuário feche a janela
    while (!glfwWindowShouldClose(janela)) { // Enquanto a janela não for fechada
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Vincula o framebuffer padrão (a janela)
        glClear(GL_COLOR_BUFFER_BIT); // Limpa o buffer de cor da tela (preenche com a cor de fundo)

        for (int i = 0; i < 2; i++) { // Itera sobre as linhas das regiões
            for (int j = 0; j < 2; j++) { // Itera sobre as colunas das regiões
                // Define a viewport para a região atual
                glViewport(j * larguraRegiao, i * alturaRegiao, larguraRegiao, alturaRegiao);
                // Define a região retangular da janela que será renderizada
                // Desenha nesta região
                Desenhar(); // Chama a função para desenhar na região atual
            }
        }

        // Troca os buffers de frente e fundo da janela
        glfwSwapBuffers(janela); // Troca os buffers de frente e fundo da janela (double buffering)

        // Processa eventos de entrada (teclado, mouse, etc.)
        glfwPollEvents(); // Processa eventos de entrada (teclado, mouse, etc.)
    }

    // Limpa os objetos OpenGL da GPU
    LimparDadosDaGPU(); // Chama a função para limpar os dados da GPU
    glDeleteFramebuffers(1, &framebufferID); // Exclui o framebuffer da GPU

    // Termina a biblioteca GLFW
    glfwTerminate(); // Termina a biblioteca GLFW

    return 0; // Retorna 0 para indicar que o programa executou com sucesso
}


GLuint CarregarShaders() {
    // Cria os shaders
    GLuint IDShaderVertices = glCreateShader(GL_VERTEX_SHADER); // Cria um objeto de shader de vértices
    GLuint IDShaderFragmentos = glCreateShader(GL_FRAGMENT_SHADER); // Cria um objeto de shader de fragmentos

    // Compila o Shader de Vértices
    glShaderSource(IDShaderVertices, 1, &CodigoShaderVertices, nullptr);
    glCompileShader(IDShaderVertices); // Compila o código fonte do shader de vértices

    // Verifica se o Shader de Vértices compilou corretamente
    GLint sucesso;
    GLchar infoLog[512];
    glGetShaderiv(IDShaderVertices, GL_COMPILE_STATUS, &sucesso);
    if (!sucesso) {
        glGetShaderInfoLog(IDShaderVertices, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        // Se o shader de vértices não compilou, exibe o log de erros da compilação
    }

    // Compila o Shader de Fragmentos
    glShaderSource(IDShaderFragmentos, 1, &CodigoShaderFragmentos, nullptr);
    glCompileShader(IDShaderFragmentos); // Compila o código fonte do shader de fragmentos

    // Verifica se o Shader de Fragmentos compilou corretamente
    glGetShaderiv(IDShaderFragmentos, GL_COMPILE_STATUS, &sucesso);
    if (!sucesso) {
        glGetShaderInfoLog(IDShaderFragmentos, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        // Se o shader de fragmentos não compilou, exibe o log de erros da compilação
    }

    // Faz o link do programa GLSL combinando os shaders compilados
    IDPrograma = glCreateProgram(); // Cria um objeto de programa GLSL
    glAttachShader(IDPrograma, IDShaderVertices); // Anexa o shader de vértices ao programa
    glAttachShader(IDPrograma, IDShaderFragmentos); // Anexa o shader de fragmentos ao programa
    glLinkProgram(IDPrograma); // Faz o link do programa combinando os shaders anexados

    // Verifica se o programa GLSL foi linkado corretamente
    glGetProgramiv(IDPrograma, GL_LINK_STATUS, &sucesso);
    if (!sucesso) {
        glGetProgramInfoLog(IDPrograma, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        // Se o programa não foi linkado corretamente, exibe o log de erros do link
    }

    glDeleteShader(IDShaderVertices); // Exclui o objeto de shader de vértices, pois ele já foi incorporado ao programa
    glDeleteShader(IDShaderFragmentos); // Exclui o objeto de shader de fragmentos, pois ele já foi incorporado ao programa

    return IDPrograma; // Retorna o identificador do programa GLSL linkado
}

void TransferirDadosParaGPU() {
    // Dados de vértices para 3 triângulos
    GLfloat dadosBufferVertices[] = {
        0.0f, 0.0f,  0.0f,  20.0f, 0.0f,
        0.0f, 20.0f, 20.0f, 0.0f,  // primeiro triângulo
        0.0f, 0.0f,  0.0f,  20.0f, 20.0f,
        0.0f, 0.0f,  20.0f, 0.0f,  // segundo triângulo
        0.0f, 20.0f, 0.0f,  20.0f, 20.0f,
        0.0f, 10.0f, 30.0f, 0.0f   // terceiro triângulo
    };
    // Define um array com as coordenadas dos vértices para três triângulos

    // Uma cor para cada vértice
    GLfloat dadosBufferCores[] = {
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,  // cor para os 3 vértices do primeiro triângulo
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,  // cor para os 3 vértices do segundo triângulo
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f   // cor para os 3 vértices do terceiro triângulo
    };
    // Define um array com as cores para cada vértice dos triângulos

    // VAO (Vertex Array Object)
    glGenVertexArrays(1, &IDArrayVertices); // Gera um objeto Vertex Array (VAO)
    glBindVertexArray(IDArrayVertices); // Vincula o VAO criado como o VAO ativo

    // Move os dados de vértices para a memória de vídeo, especificamente para o VBO
    glGenBuffers(1, &bufferVertices); // Gera um objeto Vertex Buffer (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, bufferVertices); // Vincula o VBO criado como o VBO ativo
    glBufferData(GL_ARRAY_BUFFER, sizeof(dadosBufferVertices), dadosBufferVertices, GL_STATIC_DRAW);
    // Transfere os dados dos vértices do array para a memória de vídeo (VBO)

    // Move os dados de cores para a memória de vídeo, especificamente para o CBO
    glGenBuffers(1, &bufferCores); // Gera um objeto Color Buffer (CBO)
    glBindBuffer(GL_ARRAY_BUFFER, bufferCores); // Vincula o CBO criado como o VBO ativo
    glBufferData(GL_ARRAY_BUFFER, sizeof(dadosBufferCores), dadosBufferCores, GL_STATIC_DRAW);
    // Transfere os dados das cores do array para a memória de vídeo (CBO)

    // Atributos de vértices
    glBindBuffer(GL_ARRAY_BUFFER, bufferVertices); // Vincula o VBO com os dados de vértices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    // Define o ponteiro para o atributo de vértice no índice 0, com 3 componentes (x, y, z), tipo float
    glEnableVertexAttribArray(0); // Habilita o atributo de vértice no índice 0

    // Atributos de cores
    glBindBuffer(GL_ARRAY_BUFFER, bufferCores); // Vincula o CBO com os dados de cores
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    // Define o ponteiro para o atributo de cor no índice 1, com 3 componentes (r, g, b), tipo float
    glEnableVertexAttribArray(1); // Habilita o atributo de cor no índice 1
}

void LimparDadosDaGPU() {
    glDeleteBuffers(1, &bufferVertices); // Exclui o objeto VBO da GPU
    glDeleteBuffers(1, &bufferCores); // Exclui o objeto CBO da GPU
    glDeleteVertexArrays(1, &IDArrayVertices); // Exclui o objeto VAO da GPU
    glDeleteProgram(IDPrograma); // Exclui o programa GLSL da GPU
}

void Desenhar() {
    // Limpa o buffer de cores da tela
    // glClear(GL_COLOR_BUFFER_BIT); // Limpa o buffer de cor da tela

    // Utiliza o programa GLSL criado
    glUseProgram(IDPrograma); // Ativa o programa GLSL criado

    // Transformação da janela para a viewport
    glm::mat4 mvp = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f); // Cria uma matriz de projeção ortogonal

    // Obtém o local da variável uniforme da matriz de transformação
    GLint matrizUniforme = glGetUniformLocation(IDPrograma, "mvp"); // Obtém o local da variável uniforme "mvp" no programa GLSL
    glUniformMatrix4fv(matrizUniforme, 1, GL_FALSE, glm::value_ptr(mvp));
    // Envia a matriz mvp para a variável uniforme "mvp" no programa GLSL

    // Desenha os 3 triângulos
    glBindVertexArray(IDArrayVertices); // Vincula o VAO com os dados de vértices e cores
    glDrawArrays(GL_TRIANGLES, 0, 9); // Desenha 3 triângulos a partir dos 9 vértices fornecidos
}