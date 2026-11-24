# oq tem q ter
- objetivo: traduzir um endereço logico para fisico para um espaço virtual no tamanho 2¹⁶ = 65.536 bytes
- atraves da TLB e de uma page table, nela vai ter a "tradução" de cada endereço logico ao seu endereço físico correspondente
- memoria fisica e espaço do endereço virtual devem ser de mesmo tamanho
- deve imprimir na saida o valor do byte que esta armazenado nesse endereço
- memoria fisica com 128 frames (0 a 127)
- quando a memória estiver cheia, aplicar o algoritmo de substituição a fim de identificar qual frame será atualizado
- dois algoritmos de substituição de página, a saber fifo e lru, e dois para substituição da TLB, também fifo e lru;
- O primeiro argumento por linha de comando será um arquivo de endereços lógicos (numeros inteiros)
- ex: `./vm address.txt lru fifo`
- algoritmo de substituição da página será o lru e TLB o fifo
- A busca pela referência na TLB deverá ser realizada com uma thread para cada entrada da TLB a fim de simular uma otimização da busca;
- saída será denominado como correct.txt
- checar todos os parâmetros antes de aceitá-los.
- BACKING_STORE.bin é o arquivo representando o backing store
- no final deve mostrar as estatisticas da porcentagem de page fault e porcentagem de tlb hit

# erros quando:
- número e o tipo de argumentos durante a execução não estiverem de acordo
- se o arquivo de endereços não existir, não puder ser aberto ou encontrar-se numa formatação diferente daquela indicada no exemplo anexado.

## page table
- os endereços recebidos serao int mas vamos trabalhar em cima dos 16 bits dos 32 que o int suporta
- 8 (8 bit ao 15 bit) -> numero da pagina; 8 (0 bit ao 7 bit)-> offset; 
- o numero da pagina atua como um index
- a tabela comtém o endereço base de cada frame na memoria fisica
- o offset é a localização do frame sendo referenciada (o pedaço do frame)
1. exatrir o numero da pagina e usar como indexe na tabela
2. Extrair o frame correspondente da tabela
3. replace o numero da pagina no endereço logico com o numero do frame
- offset nao muda e nao é replaced
- se nao achar o frame correspondente: page fault e aloca o proximo frame disponivle, le 256 bytes do .bin, salva no frame da memoria fisica e atualize a page table
- endereço fisico = (frame_number * 256) + offset

# fluxo
- o numero da pagina é extraida do endereço logico e a TLB é consultada
- caso o numero da pagina seja encontrado no TLB, o numero do frame é obitido
- caso contrario, deve ser consultada a page table
- quando um page fault acontecer, deve ser lido em uma pagina de 256 bytes do arquivo bin e guardar em um frame de pagina disponivel na memoria fisica (?)


# recomendação do livro
- 1. extrair o numero da pagina e o offset de cada valor que entrar
- 2. começar por uma page table inves do tlb
- 3. quando a page table estiver pronta, começar a implementar o tlb
- 4. dentro do tlb entra os algoritmos de substituição para atualizar o tlb
- usar char para facilitar, pois um char tem 8 bytes