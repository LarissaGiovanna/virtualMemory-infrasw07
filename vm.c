#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <errno.h>

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

    line[strcspn(line, "\r\n")] = '\0';//removendo o caractere de nova linha do final da string

    char *endptr;
    errno = 0; //zera o errno (ultimo erro registrado) antes de chamar strtoul para detectar erros de conversão

    unsigned long parsed = strtoul(line, &endptr, 10); //strtoul converte a string para um número inteiro sem sinal (unsigned long) e armazena o endereço do primeiro caractere inválido em endptr
    int overflow = (errno == ERANGE); //verifica se houve estouro de valor (ERANGE é um macro que indica que o valor convertido está fora do intervalo representável pelo tipo de dado)

    while (isspace((unsigned char)*endptr)) {
        endptr++; //verifica se há espaços em branco após o número convertido e avança o ponteiro endptr para o próximo caractere não branco
    }

    if (endptr == line || *endptr != '\0' || overflow || parsed > 65535) {
        fprintf(stdout, "Endereço inválido no arquivo: %s\n", line);
        fclose(address_file);
        return 1;
    }

        int address = (int)parsed;

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
