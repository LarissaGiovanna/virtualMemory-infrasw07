#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    //lendo as entradas
    if (argc <= 1 || argc > 4)
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
    int *addresses[1000];
    int address_count = 0;

    while (fgets(line, sizeof(line), address_file))
    {
        //removendo o \n do final da linha
        line[strcspn(line, "\n")] = 0;

        //verificando se a linha é um endereço válido
        if (strlen(line) > 7) //65636\0\n
        {
            fprintf(stdout, "Endereço inválido: %s\n", line);
            continue;
        }
        //alocando memória para o endereço e copiando a linha para o array de endereços
        addresses[address_count] = malloc(sizeof(int));
        *addresses[address_count] = atoi(line);
        address_count++;

        //fazendo o corte para separar as partes do endereço int
        //8 (0 bit ao 7 bit)-> offset; 8 (8 bit ao 15 bit) -> numero da pagina
        //outros bits não são utilizados
        //bit masking
        int offsets[address_count];
        int page_numbers[address_count];

        for (int i = 0; i < address_count; i++)
        {
            offsets[i] = *addresses[i] & 0xFF; //8 bits
            page_numbers[i] = (*addresses[i] >> 8) & 0xFF; //8 bits
        }

    }

    fclose(address_file);



    return 0;
}
