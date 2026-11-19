#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

typedef struct{
    int page_number;
    int frame_number;
} PageTable;

int main(int argc, char *argv[])
{
    //lendo as entradas
    if (argc != 4)
    {
        //./vm address.txt lru fifo
        fprintf(stdout, "Uso incorreto. Uso: %s <nome_arquivo> <algoritmo_substituicao_pagina> <algoritmo_substituicao_tlb>\n", argv[0]);
        return 1;
    }

    //lendo o arquivo
    FILE *address_file = fopen(argv[1], "r");
    if (address_file == NULL)
    {
        fprintf(stdout, "Erro ao abrir o arquivo %s\n", argv[1]);
        return 1;
    }

    //lendo os algoritmos
    char *page_replacement_algorithm = argv[2];
    char *tlb_replacement_algorithm = argv[3];
    if (page_replacement_algorithm == NULL || tlb_replacement_algorithm == NULL)
    {
        fprintf(stdout, "Erro ao ler os algoritmos de substituição de página e TLB\n");
        return 1;
    }else if ((strcmp(page_replacement_algorithm, "lru") != 0 && strcmp(page_replacement_algorithm, "fifo") != 0) || (strcmp(tlb_replacement_algorithm, "lru") != 0 && strcmp(tlb_replacement_algorithm, "fifo") != 0))
    {
        fprintf(stdout, "Algoritmo de substituição de página ou TLB inválido. Use 'lru' ou 'fifo'.\n");
        return 1;
    }

    //lendo os endereços do arquivo
    char line[256];
    uint32_t addresses[1000]; //array para armazenar os endereços (uint32_t é um tipo de dado inteiro sem sinal de 32 bitss)
    size_t address_count = 0; //contador de endereços válidos (size_t é um tipo de dado inteiro sem sinal que é usado para representar o tamanho de objetos em bytes)

    while (fgets(line, sizeof(line), address_file))
    {
        //removendo o \n do final da linha
        line[strcspn(line, "\r\n")] = '\0';

        //verificando se a linha é um endereço válido
        //se ta vazio o arquivo
        if (*line == '\0')
        {
            fprintf(stdout, "Linha vazia no arquivo de endereços\n");
            fclose(address_file);
            return 1; //encerra
        }
        
        char *address_str = line;
        for (char *p = line; *p != '\0'; p++) //p apontando para o primeiro caractere da linha, enquanto não for o final da string, incrementa p e percorre a linha
        {
            fprintf(stdout, "Caractere lido: %c\n", *p);
            if (isdigit(*p) == 0) //se nao for numero
            {
                fprintf(stdout, "Endereço inválido no arquivo: %s\n", line);
                fclose(address_file);
                return 1; //encerra
            }
            address_str[p - line] = *p; //copiando o caractere para o endereço
        }
        int address = atoi(address_str); //converte a string para inteiro
        fprintf(stdout, "Endereço lido: %d\n", address);
        if (address < 0 || address > 65535) //verifica se o endereço é válido (0 a 65535)
        {
            fprintf(stdout, "Endereço fora do intervalo válido (0 a 65535): %d\n", address);
            fclose(address_file);
            return 1; //encerra
        }

        if (address_count >= 1000) {
            fprintf(stderr, "Muitos enderecos.\n");
            fclose(address_file);
            return 1;
        }
        addresses[address_count++] = (uint32_t)address; //converte o endereço para uint32_t e armazena no array de endereços, incrementando o contador de endereços
    }

        //fazendo o corte para separar as partes do endereço int
        //8 (0 bit ao 7 bit)-> offset; 8 (8 bit ao 15 bit) -> numero da pagina
        //outros bits não são utilizados
        //bit masking
        int offsets[address_count];
        int page_numbers[address_count];

        for (size_t i = 0; i < address_count; i++)
        {
            offsets[i] = addresses[i] & 0xFF; //8 bits
            page_numbers[i] = (addresses[i] >> 8) & 0xFF; //8 bits
        }

        PageTable page_table[256] = {0}; //tabela de páginas com 256 entradas e inicializada com 0

        fprintf(stdout, "se chegou aqui, leu tudo certo\n");

        fclose(address_file);
        return 0;
    }
