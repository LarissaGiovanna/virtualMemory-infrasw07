# oq tem q ter
- memoria fisica com 128 frames (0 a 127)
- quando a memória estiver cheia, aplicar o algoritmo de substituição a fim de identificar qual frame será atualizado
- dois algoritmos de substituição de página, a saber fifo e lru, e dois para substituição da TLB, também fifo e lru;
- O primeiro argumento por linha de comando será um arquivo de endereços lógicos
- ex: `./vm address.txt lru fifo`
- algoritmo de substituição da página será o lru e TLB o fifo
- A busca pela referência na TLB deverá ser realizada com uma thread para cada entrada da TLB a fim de simular uma otimização da busca;
- saída será denominado como correct.txt
- checar todos os parâmetros antes de aceitá-los.

# erros quando:
- número e o tipo de argumentos durante a execução não estiverem de acordo
- se o arquivo de endereços não existir, não puder ser aberto ou encontrar-se numa formatação diferente daquela indicada no exemplo anexado.
