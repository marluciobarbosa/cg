#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <sstream>

// Função para escrever uma imagem em tons de cinza no formato PGM
void escrever_pgm(const std::vector<std::vector<int>>& imagem, const std::string& nome_arquivo) {
    // Verificar se a imagem é uma matriz 2D
    if (imagem.empty() || imagem[0].empty()) {
        throw std::invalid_argument("A imagem deve ser uma matriz 2D.");
    }

    // Verificar se a imagem é em tons de cinza (valores entre 0 e 255)
    for (const auto& linha : imagem) {
        for (int pixel : linha) {
            if (pixel < 0 || pixel > 255) {
                throw std::invalid_argument("A imagem deve ser em tons de cinza (valores entre 0 e 255).");
            }
        }
    }

    // Obter as dimensões da imagem
    int altura = imagem.size();
    int largura = imagem[0].size();

    // Criar o cabeçalho do arquivo PGM
    std::stringstream cabecalho;
    cabecalho << "P5 " << largura << " " << altura << " 255\n";

    // Converter a matriz em bytes
    std::vector<unsigned char> dados;
    for (const auto& linha : imagem) {
        for (int pixel : linha) {
            dados.push_back(static_cast<unsigned char>(pixel));
        }
    }

    // Abrir o arquivo PGM em modo binário e escrever o cabeçalho e os dados
    std::ofstream arquivo(nome_arquivo, std::ios::binary);
    if (!arquivo) {
        throw std::runtime_error("Não foi possível abrir o arquivo para escrita.");
    }
    arquivo.write(cabecalho.str().c_str(), cabecalho.str().size());
    arquivo.write(reinterpret_cast<const char*>(dados.data()), dados.size());
}

int main() {
    // Exemplo de uso
    std::vector<std::vector<int>> imagem = {
        {0, 50, 100},
        {150, 200, 255},
        {220, 180, 130},
    };

    try {
        escrever_pgm(imagem, "imagem.pgm");
        std::cout << "Imagem PGM criada com sucesso." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }

    return 0;
}