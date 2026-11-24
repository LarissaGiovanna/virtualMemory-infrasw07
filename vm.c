#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <errno.h>

typedef struct{
    int page_number;
    int frame_number;
    int valid;
} PageTable;

typedef struct{
    int page_number;
    int frame_number;
    int valid;
    int last_access;
}TLB;

int8_t physical_memory[128][256]; //128 frames de 256 bytes cada

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
        //removendo o \n (e o \r, se for Windows) do final da linha
        line[strcspn(line, "\r\n")] = '\0';

        //converte a linha para número; aceita espaço no começo e no fim
        char *endptr;
        errno = 0;
        unsigned long parsed = strtoul(line, &endptr, 10);
        int overflow = (errno == ERANGE);

        //ignora espaços que sobraram depois do número
        while (isspace((unsigned char)*endptr))
        {
            endptr++;
        }

        //se não leu nenhum digito, sobrou lixo ou passou da faixa, rejeita
        if (endptr == line || *endptr != '\0' || overflow || parsed > 65535)
        {
            fprintf(stdout, "Endereço inválido no arquivo: %s\n", line);
            fclose(address_file);
            return 1;
        }

        if (address_count >= 1000)
        {
            fprintf(stderr, "Muitos enderecos.\n");
            fclose(address_file);
            return 1;
        }

        addresses[address_count++] = (uint32_t)parsed;
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
        //inicializando a tabela com valid = 0, page_number = -1, frame_number = -1
        for (int i = 0; i < 256; i++)
        {
            page_table[i].valid = 0;
            page_table[i].page_number = -1;
            page_table[i].frame_number = -1;
        }
        
        int next_frame = 0; //próximo frame livre na memória física
        int page_faults = 0; //contador de page faults

        FILE* backing_store = fopen("BACKING_STORE.bin", "rb");
                if (backing_store == NULL)
                {
                    fprintf(stdout, "Erro ao abrir o arquivo BACKING_STORE.bin\n");
                    fclose(address_file);
                    return 1;
            }
        for (size_t i = 0; i < address_count; i++)
        {
            int page_number = page_numbers[i];
            int offset = offsets[i];

            if (page_table[page_number].valid == 0) //se a página não está na memória física
            {
                page_faults++;
                if (next_frame >= 128) //se não há mais frames livres
                {
                    fprintf(stdout, "Memória física cheia. Não é possível carregar a página %d\n", page_number);
                    fclose(address_file);
                    return 1;
                }

                //carrega a página da backing store para a memória física
                

                fseek(backing_store, page_number * 256, SEEK_SET); //posiciona o ponteiro do arquivo no início da página
                fread(physical_memory[next_frame], sizeof(int8_t), 256, backing_store); //lê os 256 bytes da página para a memória física
                
                //atualiza a tabela de páginas
                page_table[page_number].valid = 1;
                page_table[page_number].frame_number = next_frame;
                next_frame++;
            }
        }
        fclose(backing_store);

        fclose(address_file);
        return 0;
    }
