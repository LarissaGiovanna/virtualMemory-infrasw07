# Memória Virtual — Infraestrutura de Software

Implementação em C do exercício *Designing a Virtual Memory Manager*
(Silberschatz et al., *Operating System Concepts*, 10ª ed., p. P-51), com as
modificações do enunciado: memória física com **128 frames**, tabela de páginas
com **256 entradas**, TLB com **16 entradas** e busca na TLB com **uma thread por
entrada**.

## Arquivos

| Arquivo | Descrição |
|---|---|
| `vm.c` | Código-fonte completo do gerenciador de memória virtual |
| `makefile` | Compilação e limpeza |
| `addresses.txt` | Arquivo de endereços lógicos usado nos testes |
| `BACKING_STORE.bin` | Backing store com 65.536 bytes (256 páginas de 256 bytes) |
| `correct.txt` | Saída de referência para `./vm addresses.txt lru fifo` |
| `evidencias.log` | Log de terminal das sessões de desenvolvimento |
| `docs/` | Cronograma, planejamento e diário de tentativas |

## Compilação

```bash
make clean
make
```

O `make` gera o executável `vm` na raiz do projeto. O `make clean` remove o
executável e os arquivos objeto.

## Execução

```bash
./vm <arquivo_de_enderecos> <algoritmo_de_pagina> <algoritmo_de_tlb>
```

- `<arquivo_de_enderecos>`: arquivo texto com um endereço lógico por linha.
- `<algoritmo_de_pagina>`: `fifo` ou `lru`.
- `<algoritmo_de_tlb>`: `fifo` ou `lru`.

Exemplo do enunciado:

```bash
./vm addresses.txt lru fifo
```

A saída vai para a saída padrão. Para salvar em arquivo:

```bash
./vm addresses.txt lru fifo > saida.txt
```

## Formato da saída

Uma linha por endereço traduzido:

```text
Virtual address: 16916 TLB: 0 Physical address: 20 Value: 0
```

- `Virtual address`: endereço lógico lido do arquivo.
- `TLB`: índice da entrada da TLB usada (0 a 15); `-1` quando a TLB está desligada.
- `Physical address`: `frame * 256 + offset`.
- `Value`: byte assinado armazenado no endereço físico.

No final:

```text
Number of Translated Addresses = 1000
Page Faults = 539
Page Fault Rate = 0.539
TLB Hits = 54
TLB Hit Rate = 0.054
```

## Fluxos da rubrica em um único executável

O enunciado exige exatamente três argumentos de linha de comando. Para
demonstrar os demais fluxos sem criar outros binários e sem aceitar argumentos
extras, o programa lê três variáveis de ambiente opcionais. **Sem nenhuma
variável definida, o comportamento é o do enunciado**: 128 frames, TLB ligada e
uma thread por entrada.

| Fluxo | Comando |
|---|---|
| 128 frames + TLB + threads (padrão) | `./vm addresses.txt lru fifo` |
| 128 frames, sem TLB | `VM_TLB=0 ./vm addresses.txt lru fifo` |
| 256 frames, sem TLB | `VM_TLB=0 VM_FRAMES=256 ./vm addresses.txt lru fifo` |
| 128 frames + TLB sem threads | `VM_THREADS=0 ./vm addresses.txt lru fifo` |

| Variável | Padrão | Valores | Efeito |
|---|---|---|---|
| `VM_FRAMES` | `128` | 1 a 256 | Quantidade de frames usados na memória física |
| `VM_TLB` | ligada | `0` desliga | Desativa a consulta e a inserção na TLB |
| `VM_THREADS` | ligadas | `0` desliga | Faz a busca na TLB de forma sequencial |

Um valor inválido em `VM_FRAMES` (fora de 1–256, vazio ou não numérico) encerra o
programa com mensagem de erro e código de retorno 1.

O fluxo de 256 frames corresponde ao projeto original do livro: 256 × 256 =
65.536 bytes, o tamanho inteiro da backing store. Nele todas as páginas cabem na
memória, então não há substituição; o arquivo `addresses.txt` toca 244 páginas
únicas e produz 244 page faults.

## Tratamento de erros

O programa encerra com código 1 e mensagem coerente quando:

1. o número de argumentos é diferente de 3;
2. o arquivo de endereços não existe ou não pode ser aberto;
3. o algoritmo de página ou de TLB não é `fifo` nem `lru`;
4. alguma linha do arquivo não é um inteiro entre 0 e 65535, ou contém lixo
   (por exemplo, `12 34` ou `abc`);
5. `VM_FRAMES` tem valor inválido;
6. a `BACKING_STORE.bin` não pode ser aberta, posicionada ou lida.

Espaços no início e no fim da linha são aceitos.

## Resultados medidos

Com `addresses.txt` (1000 endereços, 244 páginas únicas):

| Comando | Page faults | TLB hits |
|---|---|---|
| `./vm addresses.txt lru fifo` | 539 | 54 |
| `./vm addresses.txt fifo fifo` | 538 | 54 |
| `./vm addresses.txt lru lru` | 539 | 55 |
| `./vm addresses.txt fifo lru` | 538 | 55 |
| `VM_TLB=0 ./vm addresses.txt lru fifo` | 539 | 0 |
| `VM_TLB=0 ./vm addresses.txt fifo fifo` | 538 | 0 |
| `VM_TLB=0 VM_FRAMES=256 ./vm addresses.txt lru fifo` | 244 | 0 |
| `VM_THREADS=0 ./vm addresses.txt lru fifo` | 539 | 54 |

## Validação

```bash
make clean && make

./vm addresses.txt lru fifo > /tmp/saida.txt
diff -q correct.txt /tmp/saida.txt

for i in $(seq 1 10); do
    ./vm addresses.txt lru fifo > /tmp/saida_$i.txt || exit 1
    diff -q correct.txt /tmp/saida_$i.txt || exit 1
done
```

O `diff` contra `correct.txt` deve terminar sem diferenças, inclusive nas 10
execuções repetidas.

Casos inválidos:

```bash
./vm
./vm addresses.txt lru
./vm nao_existe.txt lru fifo
./vm addresses.txt xyz fifo
printf '12 34\n' > /tmp/invalido.txt && ./vm /tmp/invalido.txt lru fifo
printf '65536\n'   > /tmp/fora.txt   && ./vm /tmp/fora.txt lru fifo
```

## Organização do `vm.c`

1. Validação dos argumentos, do arquivo e dos algoritmos.
2. Leitura e validação dos endereços com `strtoul`, `endptr` e `errno`.
3. Configuração dos fluxos por variável de ambiente.
4. Extração de página (`>> 8`) e offset (`& 0xFF`).
5. Tabela de páginas com 256 entradas e memória física de 128 a 256 frames.
6. Busca na TLB com uma thread por entrada (ou sequencial com `VM_THREADS=0`).
7. Page fault, leitura da backing store e substituição FIFO/LRU de página.
8. Inserção e substituição FIFO/LRU na TLB.
9. Impressão da tradução e das estatísticas.

## Decisões técnicas

- **Endereço de 16 bits:** página nos bits 8–15 e offset nos bits 0–7.
- **`frame_to_page[]`:** identifica qual página ocupa cada frame na hora da
  substituição.
- **`frame_order[]`:** guarda a ordem de carga no FIFO e o instante do último
  acesso no LRU, permitindo usar a mesma estrutura para os dois algoritmos.
- **TLB hit também atualiza a recência da página:** o acesso via TLB é um acesso
  real à memória e precisa influenciar o LRU de página; sem isso a contagem de
  page faults fica incorreta.
- **Invalidação da TLB na substituição:** quando uma página sai da memória, sua
  entrada na TLB é invalidada para não devolver um frame reciclado.
- **Validação com `strtoul`:** evita o comportamento silencioso de `atoi` e
  detecta estouro com `errno == ERANGE`.
