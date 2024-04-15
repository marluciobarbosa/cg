import glfw  # Importa a biblioteca GLFW
from OpenGL.GL import *  # Importa todas as funções OpenGL
from OpenGL.GLU import *  # Importa todas as funções GLU

def main():  # Função principal do programa
    # Inicializa o GLFW
    glfw.init()

    # Cria uma janela com as dimensões desejadas
    larguraJanela = 800
    alturaJanela = 600
    glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 3)  # Define a versão major do contexto OpenGL para 3
    glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 3)  # Define a versão minor do contexto OpenGL para 3
    glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)  # Define o perfil do OpenGL como core profile
    janela = glfw.create_window(larguraJanela, alturaJanela, "Configuração da Viewport", None, None)  # Cria a janela
    glfw.make_context_current(janela)  # Define a janela atual como contexto OpenGL

    # Define as dimensões da viewport para coincidir com as dimensões da janela inicialmente
    glViewport(0, 0, larguraJanela, alturaJanela)

    # Dimensões da cena (supondo que a cena está carregada ou calculada)
    larguraCena = 1280
    alturaCena = 720

    # Calcula a proporção de aspecto da cena
    proporcaoAspectoCena = larguraCena / alturaCena

    # Loop principal
    while not glfw.window_should_close(janela):  # Enquanto a janela não deve ser fechada
        # Verifica eventos
        glfw.poll_events()

        # Ajusta as dimensões da viewport com base na proporção de aspecto da cena
        if proporcaoAspectoCena > 1.0:  # Se a proporção de aspecto da cena for maior que 1.0
            larguraViewportAjustada = larguraJanela
            alturaViewportAjustada = larguraJanela / proporcaoAspectoCena
        else:
            larguraViewportAjustada = alturaJanela * proporcaoAspectoCena
            alturaViewportAjustada = alturaJanela

        # Define as dimensões atualizadas da viewport
        glViewport(0, 0, int(larguraViewportAjustada), int(alturaViewportAjustada))  # (x, y, larguraViewportAjustada, alturaViewportAjustada)

        # Limpa a tela com um fundo preto
        glClearColor(0.0, 0.0, 0.0, 1.0)
        glClear(GL_COLOR_BUFFER_BIT)

        # Renderiza a geometria da cena aqui
        # Troca os buffers e desenha
        glfw.swap_buffers(janela)

    # Finaliza o GLFW
    glfw.terminate()

if __name__ == "__main__":
    main()  # Chama a função principal se este script for executado diretamente
