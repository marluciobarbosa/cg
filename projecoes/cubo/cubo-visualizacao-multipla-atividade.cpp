// Inclui cabeçalhos padrão
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>

// Inclui GLEW
#include <GL/glew.h>

// Inclui GLFW
#include <GLFW/glfw3.h>
GLFWwindow* janela;

// Cabeçalho GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

// ID de objeto de array de vértices
GLuint idObjetoArrayVertices;

// ID de objeto de buffer de vértices
GLuint idBufferVertices;

// ID de objeto de buffer de cores
GLuint idBufferCores;

// ID de objeto de array de vértices dos eixos
GLuint idBufferVerticesEixos;

// ID de objeto de buffer de cores dos eixos
GLuint idBufferCoresEixos;

// ID do programa GLSL dos shaders
GLuint idPrograma;

// ID da matriz de transformação MVP (Model-View-Projection)
GLuint idMatrizMVP;
glm::mat4 MVP;

GLint larguraJanela = 1024;
GLint alturaJanela = 768;

void limparDadosDaGPU();
void desenhar(void);
void desenharEixos();
std::string getCaminhoShader(std::string arquivo);
void inicializarDadosEixos();
GLuint carregarShaders(const char* caminhoArquivoVertice, const char* caminhoArquivoFragmento);
void configurarMVP(void);
void transferirDadosParaMemoriaGPU(void);

//--------------------------------------------------------------------------------
GLuint carregarShaders(const char* caminhoArquivoVertice, const char* caminhoArquivoFragmento) {
    // Cria os shaders
    GLuint idShaderVertice = glCreateShader(GL_VERTEX_SHADER);
    GLuint idShaderFragmento = glCreateShader(GL_FRAGMENT_SHADER);

    // Lê o código do Shader de Vértice do arquivo
    std::string codigoShaderVertice;
    std::ifstream streamShaderVertice(caminhoArquivoVertice, std::ios::in);
    if (streamShaderVertice.is_open()) {
        std::string linha = "";
        while (getline(streamShaderVertice, linha))
            codigoShaderVertice += "\n" + linha;
        streamShaderVertice.close();
    }

    // Lê o código do Shader de Fragmento do arquivo
    std::string codigoShaderFragmento;
    std::ifstream streamShaderFragmento(caminhoArquivoFragmento, std::ios::in);
    if (streamShaderFragmento.is_open()) {
        std::string linha = "";
        while (getline(streamShaderFragmento, linha))
            codigoShaderFragmento += "\n" + linha;
        streamShaderFragmento.close();
    }

    GLint resultado = GL_FALSE;
    int tamanhoLog;

    // Compila o Shader de Vértice
    printf("Compilando shader: %s\n", caminhoArquivoVertice);
    char const* ponteiroCodigoVertice = codigoShaderVertice.c_str();
    glShaderSource(idShaderVertice, 1, &ponteiroCodigoVertice, NULL);
    glCompileShader(idShaderVertice);

    // Verifica o Shader de Vértice
    glGetShaderiv(idShaderVertice, GL_COMPILE_STATUS, &resultado);
    glGetShaderiv(idShaderVertice, GL_INFO_LOG_LENGTH, &tamanhoLog);
    std::vector<char> mensagemErroShaderVertice(tamanhoLog);
    glGetShaderInfoLog(idShaderVertice, tamanhoLog, NULL, &mensagemErroShaderVertice[0]);
    fprintf(stdout, "%s\n", &mensagemErroShaderVertice[0]);

    // Compila o Shader de Fragmento
    printf("Compilando shader: %s\n", caminhoArquivoFragmento);
    char const* ponteiroCodigoFragmento = codigoShaderFragmento.c_str();
    glShaderSource(idShaderFragmento, 1, &ponteiroCodigoFragmento, NULL);
    glCompileShader(idShaderFragmento);

    // Verifica o Shader de Fragmento
    glGetShaderiv(idShaderFragmento, GL_COMPILE_STATUS, &resultado);
    glGetShaderiv(idShaderFragmento, GL_INFO_LOG_LENGTH, &tamanhoLog);
    std::vector<char> mensagemErroShaderFragmento(tamanhoLog);
    glGetShaderInfoLog(idShaderFragmento, tamanhoLog, NULL, &mensagemErroShaderFragmento[0]);
    fprintf(stdout, "%s\n", &mensagemErroShaderFragmento[0]);

    // Linka o programa
    fprintf(stdout, "Linkando programa\n");
    GLuint idPrograma = glCreateProgram();
    glAttachShader(idPrograma, idShaderVertice);
    glAttachShader(idPrograma, idShaderFragmento);
    glLinkProgram(idPrograma);

    // Verifica o programa
    glGetProgramiv(idPrograma, GL_LINK_STATUS, &resultado);
    glGetProgramiv(idPrograma, GL_INFO_LOG_LENGTH, &tamanhoLog);
    std::vector<char> mensagemErroPrograma(std::max(tamanhoLog, int(1)));
    glGetProgramInfoLog(idPrograma, tamanhoLog, NULL, &mensagemErroPrograma[0]);
    fprintf(stdout, "%s\n", &mensagemErroPrograma[0]);

    glDeleteShader(idShaderVertice);
    glDeleteShader(idShaderFragmento);

    return idPrograma;
}

//--------------------------------------------------------------------------------
std::string getCaminhoShader(std::string arquivo) {
    // Suporte para C++ 17 
    std::filesystem::path dirCorrente = std::filesystem::current_path();
    std::filesystem::path caminho = dirCorrente.append(arquivo);
    std::filesystem::path caminhoAbsoluto = std::filesystem::absolute(caminho);

    return caminhoAbsoluto.string();
}

//--------------------------------------------------------------------------------
void transferirDadosParaMemoriaGPU(void) {
    // No início do programa, após inicializar o cubo
    inicializarDadosEixos();

    // VAO
    glGenVertexArrays(1, &idObjetoArrayVertices);
    glBindVertexArray(idObjetoArrayVertices);
    
    // Cria e compila nosso programa GLSL a partir dos shaders
    idPrograma = carregarShaders(getCaminhoShader("TransformVertexShader.vertexshader").c_str(), getCaminhoShader("ColorFragmentShader.fragmentshader").c_str());
    
    // Nossos vértices. Três floats consecutivos formam um vértice 3D; Três vértices consecutivos formam um triângulo.
    // Um cubo tem 6 faces com 2 triângulos cada, o que resulta em 6*2=12 triângulos, e 12*3 vértices
    static const GLfloat dadosBufferVertices[] = {
        -1.0f,-1.0f,-1.0f,  -1.0f,-1.0f, 1.0f,  -1.0f, 1.0f, 1.0f, // Face 1
        -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,  -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,   -1.0f,-1.0f, 1.0f,  1.0f,-1.0f,-1.0f, // Face 2
        1.0f,-1.0f,-1.0f,   -1.0f,-1.0f,-1.0f,  -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f,   1.0f, 1.0f, 1.0f, // Face 3
        -1.0f, 1.0f, 1.0f,  -1.0f, 1.0f,-1.0f,  1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,  1.0f,-1.0f,-1.0f,   1.0f, 1.0f,-1.0f, // Face 4
        -1.0f,-1.0f,-1.0f,  -1.0f, 1.0f,-1.0f,  1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,  1.0f,-1.0f, 1.0f,   1.0f, 1.0f, 1.0f, // Face 5
        -1.0f,-1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,   1.0f, 1.0f,-1.0f,   1.0f, 1.0f, 1.0f, // Face 6
        1.0f,-1.0f,-1.0f,   1.0f,-1.0f, 1.0f,   1.0f, 1.0f, 1.0f
    };

    // Cores para cada face, repetidas para cada vértice
    static const GLfloat dadosBufferCores[] = {
        1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // Face 1 Vermelho
        1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // Face 2 Verde
        0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f, // Face 3 Azul
        0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f, // Face 4 Amarelo
        1.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f, // Face 5 Magenta
        1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f, // Face 6 Ciano
        0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f
    };

    // Move os dados dos vértices para a memória de vídeo; especificamente para o VBO chamado idBufferVertices
    glGenBuffers(1, &idBufferVertices);
    glBindBuffer(GL_ARRAY_BUFFER, idBufferVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dadosBufferVertices), dadosBufferVertices, GL_STATIC_DRAW);
    
    // Move os dados das cores para a memória de vídeo; especificamente para o CBO chamado idBufferCores
    glGenBuffers(1, &idBufferCores);
    glBindBuffer(GL_ARRAY_BUFFER, idBufferCores);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dadosBufferCores), dadosBufferCores, GL_STATIC_DRAW);
}


//--------------------------------------------------------------------------------
void limparDadosDaGPU() {
    glDeleteBuffers(1, &idBufferVertices);
    glDeleteBuffers(1, &idBufferCores);
    glDeleteVertexArrays(1, &idObjetoArrayVertices);
    glDeleteProgram(idPrograma);
}

//--------------------------------------------------------------------------------
void configurarMVP(void) {
    // Obtém um identificador para nossa uniforme "MVP"
    idMatrizMVP = glGetUniformLocation(idPrograma, "MVP");
    
    // Matriz de projeção: campo de visão de 45 graus, proporção 4:3, alcance de exibição: 0.1 unidade <-> 100 unidades
    glm::mat4 Projecao = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    // Matriz da câmera
    glm::mat4 Visualizacao = glm::lookAt(
        glm::vec3(4,3,-3), // A câmera está em (4,3,-3), no Espaço do Mundo
        glm::vec3(0,0,0),  // e olha para a origem
        glm::vec3(0,1,0)   // A cabeça está para cima (configure para 0,-1,0 para olhar de cabeça para baixo)
    );
    // Matriz do modelo: uma matriz identidade (o modelo estará na origem)
    glm::mat4 Modelo = glm::mat4(1.0f);
    // Nossa ModelViewProjection: multiplicação de nossas 3 matrizes
    MVP = Projecao * Visualizacao * Modelo; 
}

//--------------------------------------------------------------------------------
void desenhar() {
    // Limpa a tela
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Obtém o tamanho do framebuffer para lidar com DPIs diferentes
    glfwGetFramebufferSize(janela, &larguraJanela, &alturaJanela);

    // Define os viewports e as configurações de câmera
    std::vector<std::tuple<int, int, int, int>> viewports = {
        std::make_tuple(0, 0, larguraJanela / 2, alturaJanela / 2), // Inferior Esquerdo - Frente
        std::make_tuple(larguraJanela / 2, 0, larguraJanela / 2, alturaJanela / 2), // Inferior Direito - Trás
        std::make_tuple(0, alturaJanela / 2, larguraJanela / 2, alturaJanela / 2), // Superior Esquerdo - Esquerda
        std::make_tuple(larguraJanela / 2, alturaJanela / 2, larguraJanela / 2, alturaJanela / 2) // Superior Direito - Direita
    };

    std::vector<glm::vec3> posicoes_da_camera = {
        glm::vec3(0, 0, 0), // Inferior Esquerdo - Frente
        glm::vec3(0, 0, 0), // Inferior Direito - Trás
        glm::vec3(0, 0, 0), // Superior Esquerdo - Esquerda
        glm::vec3(0, 0, 0) // Superior Direito - Direita
    };

    std::vector<glm::vec3> posicoes_alvo = {
        glm::vec3(0, 0, 0), // Olhando para a origem
        glm::vec3(0, 0, 0), // Olhando para a origem
        glm::vec3(0, 0, 0), // Olhando para a origem
        glm::vec3(0, 0, 0) // Olhando para a origem
    };

    // Desenha em cada viewport
    for (size_t i = 0; i < viewports.size(); ++i) {
        int x, y, width, height;
        std::tie(x, y, width, height) = viewports[i];
        glViewport(x, y, width, height);
        float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
        glm::mat4 Projecao = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 100.0f);
        glm::mat4 Visualizacao = glm::lookAt(
            posicoes_da_camera[i],
            posicoes_alvo[i],
            glm::vec3(0, 1, 0)
        );
        MVP = Projecao * Visualizacao * glm::mat4(1.0f);

        // Usa nosso shader
        glUseProgram(idPrograma);
        glUniformMatrix4fv(idMatrizMVP, 1, GL_FALSE, &MVP[0][0]);

        // Primeiro buffer de atributo: vértices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, idBufferVertices);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        // Segundo buffer de atributo: cores
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, idBufferCores);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        // Desenha o cubo
        glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 é o número total de vértices a serem desenhados

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // Desenha os eixos
        desenharEixos();
    }

    // Restaura o viewport original para abranger toda a janela
    glViewport(0, 0, larguraJanela, alturaJanela);
}

//--------------------------------------------------------------------------------

void inicializarDadosEixos() {
    // Cria o CBO para os eixos (parte positiva dos eixos)
    static const GLfloat dadosBufferVerticesEixos[] = {
        0.0f, 0.0f, 0.0f,  4.0f, 0.0f, 0.0f,  // Eixo X (Vermelho)
        0.0f, 0.0f, 0.0f,  0.0f, 4.0f, 0.0f,  // Eixo Y (Verde)
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 4.0f   // Eixo Z (Azul)
    };

    static const GLfloat dadosBufferCoresEixos[] = {
        1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,   // Vermelho
        0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,   // Verde
        0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f    // Azul
    };

    glGenBuffers(1, &idBufferVerticesEixos);
    glBindBuffer(GL_ARRAY_BUFFER, idBufferVerticesEixos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dadosBufferVerticesEixos), dadosBufferVerticesEixos, GL_STATIC_DRAW);

    glGenBuffers(1, &idBufferCoresEixos);
    glBindBuffer(GL_ARRAY_BUFFER, idBufferCoresEixos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dadosBufferCoresEixos), dadosBufferCoresEixos, GL_STATIC_DRAW);
}
//--------------------------------------------------------------------------------

void desenharEixos() {
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, idBufferVerticesEixos);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, idBufferCoresEixos);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_LINES, 0, 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
//--------------------------------------------------------------------------------


int main(void) {
    // Inicializa GLFW
    glfwInit();

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Para alegria do MacOS; não deveria ser necessário
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Abre uma janela
    janela = glfwCreateWindow(larguraJanela, alturaJanela, "Um cubo com 12 triângulos", NULL, NULL);

    // Cria contexto da janela
    glfwMakeContextCurrent(janela);

    // Inicializa GLEW
    glewExperimental = true; // Necessário para perfilamento do core
    glewInit();

    // Garante que podemos capturar a tecla ESC sendo pressionada abaixo
    glfwSetInputMode(janela, GLFW_STICKY_KEYS, GL_TRUE);

    // Fundo branco
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    // Limpa a tela
    glClear(GL_COLOR_BUFFER_BIT);

    // Habilita teste de profundidade
    glEnable(GL_DEPTH_TEST);
    // Aceita o fragmento se ele estiver mais próximo da câmera que o anterior
    glDepthFunc(GL_LESS);

    // Transfere meus dados (vértices, cores e shaders) para o lado da GPU
    transferirDadosParaMemoriaGPU();

    // Configura a matriz de projeção-visualização-modelo
    configurarMVP();

    // Renderiza cena para cada frame
    do {
        // Desenha o cubo
        desenhar();
        // Troca os buffers
        glfwSwapBuffers(janela);
        // Procura por eventos
        glfwPollEvents();

    } // Verifica se a tecla ESC foi pressionada ou a janela foi fechada
    while (glfwGetKey(janela, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(janela) == 0);

    // Limpa VAO, VBOs e shaders da GPU
    limparDadosDaGPU();

    // Fecha a janela OpenGL e termina GLFW
    glfwTerminate();

    return 0;
}
