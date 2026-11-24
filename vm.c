#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>

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

int8_t physical_memory[256][256]; //até 256 frames de 256 bytes cada

typedef struct{
    const TLB *entry;
    int page_number;
    int hit;
} TlbSearchArg;

//cada thread verifica apenas uma entrada da TLB
static void *tlb_search_entry(void *arg)
{
    TlbSearchArg *search = (TlbSearchArg *)arg;
    search->hit = (search->entry->valid && search->entry->page_number == search->page_number);
    return NULL;
}

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

    //fluxos da rubrica: o padrão continua sendo 128 frames, TLB com threads
    int frame_limit = 128;
    int use_tlb = 1;
    int use_threads = 1;

    const char *env_frames = getenv("VM_FRAMES");
    if (env_frames != NULL)
    {
        char *end = NULL;
        long value = strtol(env_frames, &end, 10);
        if (end == env_frames || *end != '\0' || value < 1 || value > 256)
        {
            fprintf(stderr, "VM_FRAMES inválido: use um valor entre 1 e 256\n");
            fclose(address_file);
            return 1;
        }
        frame_limit = (int)value;
    }

    const char *env_tlb = getenv("VM_TLB");
    if (env_tlb != NULL && strcmp(env_tlb, "0") == 0)
    {
        use_tlb = 0;
    }

    const char *env_threads = getenv("VM_THREADS");
    if (env_threads != NULL && strcmp(env_threads, "0") == 0)
    {
        use_threads = 0;
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
        int tlb_hits = 0; //contador de acertos na TLB
        int use_lru = (strcmp(page_replacement_algorithm, "lru") == 0);
        int tlb_use_lru = (strcmp(tlb_replacement_algorithm, "lru") == 0);

        //frame_to_page permite descobrir qual página está em um frame na hora de substituir
        int frame_to_page[frame_limit];
        //fifo: ordem de carga; lru: instante do último acesso
        unsigned long frame_order[frame_limit];
        for (int i = 0; i < frame_limit; i++)
        {
            frame_to_page[i] = -1;
            frame_order[i] = 0;
        }
        unsigned long access_clock = 0;

        //TLB com 16 entradas
        TLB tlb[16];
        unsigned long tlb_order[16];
        for (int i = 0; i < 16; i++)
        {
            tlb[i].page_number = -1;
            tlb[i].frame_number = -1;
            tlb[i].valid = 0;
            tlb[i].last_access = 0;
            tlb_order[i] = 0;
        }
        unsigned long tlb_clock = 0;

        FILE* backing_store = fopen("BACKING_STORE.bin", "rb");
        if (backing_store == NULL)
        {
            fprintf(stdout, "Erro ao abrir o arquivo BACKING_STORE.bin\n");
            fclose(address_file);
            return 1;
        }


        FILE* correct_file = fopen("correct.txt", "w");
            if (correct_file == NULL)
            {
                fprintf(stderr, "Error opening correct.txt\n");
                return 1;
            }

        for (size_t i = 0; i < address_count; i++)
        {
            int page_number = page_numbers[i];
            int offset = offsets[i];

            int frame;
            int tlb_index = -1;

            //procura a página na TLB (quando ela existe)
            if (use_tlb)
            {
                if (use_threads)
                {
                    //uma thread por entrada da TLB
                    pthread_t search_threads[16];
                    TlbSearchArg search_args[16];
                    for (int t = 0; t < 16; t++)
                    {
                        search_args[t].entry = &tlb[t];
                        search_args[t].page_number = page_number;
                        search_args[t].hit = 0;

                        if (pthread_create(&search_threads[t], NULL, tlb_search_entry, &search_args[t]) != 0)
                        {
                            fprintf(stderr, "Erro ao criar thread da TLB\n");
                            fclose(backing_store);
                            fclose(address_file);
                            return 1;
                        }
                    }

                    for (int t = 0; t < 16; t++)
                    {
                        pthread_join(search_threads[t], NULL);
                    }

                    //a primeira entrada que deu hit é o índice mostrado na saída
                    for (int t = 0; t < 16; t++)
                    {
                        if (search_args[t].hit)
                        {
                            tlb_index = t;
                            break;
                        }
                    }
                }
                else
                {
                    //mesma busca, sem threads
                    for (int t = 0; t < 16; t++)
                    {
                        if (tlb[t].valid && tlb[t].page_number == page_number)
                        {
                            tlb_index = t;
                            break;
                        }
                    }
                }
            }

            if (tlb_index >= 0) //TLB hit
            {
                tlb_hits++;
                frame = tlb[tlb_index].frame_number;

                //no LRU da TLB, o acerto atualiza a recência
                if (tlb_use_lru)
                {
                    tlb_order[tlb_index] = ++tlb_clock;
                }

                //um acerto na TLB também é um acesso à página na memória
                if (use_lru)
                {
                    frame_order[frame] = ++access_clock;
                }
            }
            else //TLB miss: consulta a page table
            {
                if (page_table[page_number].valid == 0) //page fault
                {
                    page_faults++;

                    if (next_frame < frame_limit) //ainda há frame livre
                    {
                        frame = next_frame;
                        next_frame++;
                    }
                    else //memória cheia: escolhe a vítima
                    {
                        frame = 0;
                        for (int f = 1; f < frame_limit; f++)
                        {
                            //FIFO usa a ordem de carga; LRU usa o último acesso
                            if (frame_order[f] < frame_order[frame])
                            {
                                frame = f;
                            }
                        }

                        int old_page = frame_to_page[frame];
                        if (old_page >= 0)
                        {
                            page_table[old_page].valid = 0;
                            page_table[old_page].page_number = -1;
                            page_table[old_page].frame_number = -1;

                            //a página saiu da memória: invalida a entrada dela na TLB
                            if (use_tlb)
                            {
                                for (int t = 0; t < 16; t++)
                                {
                                    if (tlb[t].valid && tlb[t].page_number == old_page)
                                    {
                                        tlb[t].valid = 0;
                                        tlb[t].page_number = -1;
                                        tlb[t].frame_number = -1;
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    //carrega a página da backing store no frame escolhido
                    if (fseek(backing_store, (long)page_number * 256, SEEK_SET) != 0)
                    {
                        fprintf(stdout, "Erro ao posicionar na BACKING_STORE.bin\n");
                        fclose(backing_store);
                        fclose(address_file);
                        return 1;
                    }

                    if (fread(physical_memory[frame], sizeof(int8_t), 256, backing_store) != 256)
                    {
                        fprintf(stdout, "Erro ao ler a pagina %d da BACKING_STORE.bin\n", page_number);
                        fclose(backing_store);
                        fclose(address_file);
                        return 1;
                    }

                    //atualiza a tabela de páginas e o mapa reverso
                    page_table[page_number].valid = 1;
                    page_table[page_number].page_number = page_number;
                    page_table[page_number].frame_number = frame;
                    frame_to_page[frame] = page_number;
                    frame_order[frame] = ++access_clock;
                }
                else //página já está na memória
                {
                    frame = page_table[page_number].frame_number;

                    //no LRU, todo acesso atualiza a recência; no FIFO, não
                    if (use_lru)
                    {
                        frame_order[frame] = ++access_clock;
                    }
                }

                //insere a página na TLB
                if (use_tlb)
                {
                    tlb_index = -1;
                    for (int t = 0; t < 16; t++)
                    {
                        if (!tlb[t].valid)
                        {
                            tlb_index = t;
                            break;
                        }
                    }

                    if (tlb_index < 0) //TLB cheia: escolhe a vítima
                    {
                        tlb_index = 0;
                        for (int t = 1; t < 16; t++)
                        {
                            //FIFO usa a ordem de entrada; LRU usa o último acesso
                            if (tlb_order[t] < tlb_order[tlb_index])
                            {
                                tlb_index = t;
                            }
                        }
                    }

                    tlb[tlb_index].valid = 1;
                    tlb[tlb_index].page_number = page_number;
                    tlb[tlb_index].frame_number = frame;
                    tlb[tlb_index].last_access = ++tlb_clock;
                    tlb_order[tlb_index] = tlb_clock;
                }
                else
                {
                    tlb_index = -1; //fluxo sem TLB
                }
            }

            int physical_address = frame * 256 + offset;

            
            fprintf(correct_file, "Virtual address: %u TLB: %d Physical address: %d Value: %d\n",
                    addresses[i], tlb_index, physical_address,
                    (int)physical_memory[frame][offset]);
        }

        fclose(backing_store);
        fclose(address_file);
        
        fprintf(correct_file, "Number of Translated Addresses = %zu\n", address_count);
        fprintf(correct_file, "Page Faults = %d\n", page_faults);
        fprintf(correct_file, "TLB Hits = %d\n", tlb_hits);
        fprintf(correct_file, "TLB Hit Rate = %.3f\n", address_count ? (double)tlb_hits / (double)address_count : 0.0);
        fclose(correct_file);
        
        return 0;
    }
