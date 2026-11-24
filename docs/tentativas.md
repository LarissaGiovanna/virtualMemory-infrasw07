# Tentativas — Memória Virtual

Registro iniciado em **2026-11-18 21:30 -03**.

As entradas anteriores a esse horário foram consolidadas retroativamente nesta data. Quando existe um horário medido em arquivo, commit ou log, ele aparece em **Evidência temporal**; quando não existe, está escrito que o horário exato não foi capturado.

Modelo usado nas próximas entradas:

```text
## AAAA-MM-DD HH:MM — Prompt N
Prompt:
> ...

O que tentei:
...

Resultado:
...

Evidência:
...
```

---

## 2026-11-18 21:30 — Registro retroativo — Prompt 1

Prompt:

> leia prompts.md e organize para mim um cronograma para eu realizar essa atividade, sabendo que temos que entregar até terça feira ate as 23:59 e que sexta, sabado e domingo não estarei disponivel

O que tentei:

- Ler o arquivo de instruções em `docs/base/prompt.md` (não existe `prompts.md` na raiz).
- Ler o enunciado, o template do relatório e a rubrica.
- Montar um cronograma considerando a entrega em 24/11/2026 e a indisponibilidade de sexta a domingo.

Resultado:

- Cronograma criado em `docs/cronograma.md`, com blocos por dia, pontos de corte, alvos de teste e checklist de entrega.

Evidência temporal:

- Commit `1eb6d92` em 2026-11-18 19:49:32.
- Commit `3511d25` em 2026-11-18 19:49:51.
- `docs/cronograma.md` criado às 19:59:01.
- Horário exato do prompt não capturado.

---

## 2026-11-18 21:30 — Registro retroativo — Prompt 2

Prompt:

> coloque esse nosso planejamento em um markdown. quero que vc analise meu codigo e me explique um pouco como funciona essa parte de mascarar os 16 bits. eu pesquisei um pouco e vi que podemos utilizar as tecnicas de bit masking, mas nao entendi muito bem como funciona

O que tentei:

- Implementar a leitura do arquivo de endereços.
- Usar `atoi` para converter a linha e `& 0xFF` / `>> 8 & 0xFF` para extrair offset e página.
- Criar uma `PageTable` e começar a estruturar o programa.
- Pedir uma revisão do código e uma explicação sobre bit masking.

Resultado:

- A máscara estava correta: `endereco & 0xFF` para offset e `(endereco >> 8) & 0xFF` para página.
- Foram encontrados problemas no parser: `argc` aceitava quantidade errada de argumentos, `atoi` não validava lixo, a page table tinha 1000 entradas em vez de 256 e os arrays de página/offset eram recalculados dentro do `while`.
- O build compilava com 5 warnings.

Evidência temporal:

- `vm.c` com alterações às 19:55.
- Build com warnings de variáveis não usadas e `const`.
- Horário exato do prompt não capturado.

---

## 2026-11-18 21:30 — Registro retroativo — Prompt 3

Prompt:

> mudei algumas coisas e tambem na logica de converter para numero. ele esta lendo os caracteres mas na hora de comparar esta dizendo que esta fora da faixa. como pode ser isso se é um numero?

O que tentei:

- Trocar o parser para percorrer a linha caractere por caractere com `isdigit`.
- Copiar os caracteres para `address_str` e converter com `atoi`.
- Alterar a remoção do `\n` para `line[strcspn(line, "\r\n")] = '0';`.
- Testar com o `addresses.txt`.

Resultado:

- O programa lia `16916` e imprimia `Endereço lido: 169160`, acusando fora da faixa.
- Causa: `'0'` é o caractere zero (ASCII 48), não o terminador `'\0'`. O `\n` era substituído por um dígito `0`, acrescentando um dígito ao número.
- Também foi identificado que a última linha de `addresses.txt` tem um espaço no final (`12107 `), que precisa ser tratado.
- Testei uma correção em uma cópia `/tmp/vm_fix2.c`: os 1000 endereços foram lidos, o último `12107` foi aceito e o programa terminou com `exit=0`.

Evidência temporal:

- `vm.c` modificado às 21:24:49.
- `evidencias.log` atualizado às 21:24:51.
- Saída observada: `Endereço lido: 169160` e `Endereço fora do intervalo válido (0 a 65535): 169160`.
- `tail -3 addresses.txt | cat -A` mostrou `12107 $`.

---

## 2026-11-18 21:30 — Prompt 4

Prompt:

> ao longo do que eu for fazendo e te perguntando, coloque essas minhas tentativas em tentativas.md, colocando o que tentei fazer com a data e a hora atual. coloque tambem os prompts que estou te passando. sobre ler o endereço que tem um espaço, podemos ainda verificar se tem espaço entre os caracteres e ignorar, adicionando o espaço na condicional

O que tentei:

- Pedir o registro das tentativas e dos prompts neste arquivo.
- Perguntar se é possível permitir espaço na validação dos caracteres para ignorar o espaço no fim da linha.

Resultado:

- Este arquivo foi criado/atualizado com os prompts e as tentativas até aqui.
- A recomendação é **não** simplesmente adicionar `' '` na condição de `isdigit` sem tratar a conversão, porque `atoi("12 34")` retorna 12 e perde o restante. O melhor é validar com `strtoul` e permitir apenas espaços no início/fim, ou remover os espaços para uma string limpa se a decisão for aceitar espaço em qualquer posição.

Evidência:

- Data/hora do registro: 2026-11-18 21:30:30 -03.
- `docs/tentativas.md` atualizado neste horário.

---

## 2026-11-18 21:38 — Prompt 5

Prompt:

> o que seria o errno? como funciona o strtoul e porque o resultado dele deve ser armazenado em um eunsigned long? o que significam?

O que tentei:

- Entender o significado de `errno`, da função `strtoul`, do tipo `unsigned long` e do `endptr`.
- Relacionar esses conceitos com a validação da linha `12107 ` e com a conversão do endereço.

Resultado:

- `errno` é uma variável de erro preenchida por funções da biblioteca; deve ser zerada antes da chamada e conferida logo depois.
- `strtoul` converte texto em `unsigned long`, para no primeiro caractere inválido e devolve esse ponto em `endptr`.
- O retorno é `unsigned long` porque a função pode representar valores maiores que `int` e não trabalha com números negativos; o valor só deve ser convertido para `int` depois da validação.

Evidência:

- Data/hora do registro: 2026-11-18 21:38:00 -03.
- Nenhuma alteração de código feita neste passo.

---

## 2026-11-20 00:37 — Prompt 6 — Decisão: `errno` + `strtoul`

Prompt:

> certo. coloque essa decisão do errno e stroul em tentativas. vou implementar ela

O que tentei:

- Entender `errno`, `strtoul`, `endptr` e `unsigned long`.
- Comparar com `atoi`, que não informa onde parou nem se houve overflow.
- Decidir usar `strtoul` para validar e converter a linha do arquivo.

Decisão registrada:

1. Zerar `errno` antes da conversão.
2. Guardar o retorno em `unsigned long`, que é o tipo de retorno de `strtoul`.
3. Usar `endptr` para saber onde a leitura parou.
4. Aceitar espaços no início e no fim da linha, mas rejeitar espaço no meio.
5. Rejeitar quando `endptr == line` (nenhum dígito lido), quando `*endptr != '\0'` depois de pular os espaços finais, quando `errno == ERANGE` e quando `parsed > 65535`.
6. Só converter para `int` depois de todas as validações.

Código de referência da decisão:

```c
line[strcspn(line, "\r\n")] = '\0';

char *endptr;
errno = 0;

unsigned long parsed = strtoul(line, &endptr, 10);
int overflow = (errno == ERANGE);

while (isspace((unsigned char)*endptr)) {
    endptr++;
}

if (endptr == line || *endptr != '\0' || overflow || parsed > 65535) {
    fprintf(stdout, "Endereço inválido no arquivo: %s\n", line);
    fclose(address_file);
    return 1;
}

int address = (int)parsed;
```

Exemplos que a implementação deve aceitar/rejeitar:

| Linha | Resultado esperado |
|---|---|
| `12107 ` | aceitar como `12107` |
| ` 12107` | aceitar como `12107` |
| `12 34` | rejeitar (sobrou `34` depois do espaço) |
| `abc` | rejeitar (nenhum dígito) |
| vazio | rejeitar |
| `65536` | rejeitar (fora da faixa) |
| `-1` | rejeitar (vira `ULONG_MAX` e passa de `65535`) |
| número gigante | rejeitar por `ERANGE` |

Resultado:

- Decisão tomada; implementação ainda **não feita** neste momento.
- A correção deve vir junto com a troca de `line[strcspn(line, "\r\n")] = '0';` por `'\0'`.

Evidência:

- Data/hora do registro: 2026-11-20 00:37:17 -03.
- Nenhuma alteração de código feita neste passo.

---

## 2026-11-20 00:39 — Prompt 7 — Cronograma refeito

Prompt:

> refaça o meu cronograma. infelizmente nao consegui fazer nada na quinta e nao terminei tudo da quarta. refaça o meu cronograma para as datas ainda restantes, sabendo que segunda e terça feira tenho aula pela manaha e so temos a tarde e a noite (provavelmente teremos que madrugar)

O que tentei:

- Revisar o estado real do código antes de refazer o plano.
- Rodar o build e o programa com `addresses.txt`.
- Considerar que sexta, sábado e domingo continuam indisponíveis e que segunda/terça só rendem à tarde e à noite.

Resultado:

- Estado medido: o parser lê 999 endereços e falha no último (`12107 `) por causa do espaço; ainda imprime debug caractere por caractere.
- Tradução, page table em uso, TLB, threads e estatísticas ainda não existem.
- Cronograma refeito em `docs/cronograma.md` para segunda 23/11 e terça 24/11, com madrugada, pontos de corte e escopo mínimo.
- Conclusão do replanejamento: o prazo cabe apenas se o núcleo (page table + 128 frames + LRU/FIFO + TLB) for feito na segunda; terça fica para robustez, relatório e entrega.

Evidência:

- Data/hora do registro: 2026-11-20 00:39:23 -03.
- `./vm addresses.txt lru fifo` terminou com `exit=1`, leu 999 endereços e rejeitou `12107 `.
- Commits recentes: `a141f9e` (tratamento de entradas, não finalizado), `a303acd` (tentativas/cronograma), `7ebe25f` (decisão do errno).
- Nenhuma alteração de código feita neste passo.

---

## 2026-11-23 15:33 — Prompt 8 — Auditoria de estado

Prompt:

> verifique aqui o que ja esta feito e me diga de acordo com o nosso cronograma o que ainda falta

O que tentei:

- Rodar `git status`, `git log`, `make clean && make` e o executável.
- Comparar o estado atual com o cronograma de 20/11 e com a rubrica.

Resultado:

- O build compila, mas o parser regrediu: o `while (fgets(...))` foi removido, então `line` não é preenchida e `address_count` fica em 0.
- `./vm addresses.txt lru fifo` falha imediatamente com “Endereço inválido no arquivo” e lê lixo.
- Tradução, `BACKING_STORE.bin`, page table em uso, FIFO/LRU, TLB, threads e estatísticas continuam não implementados.
- README ainda é o template; relatório e pacote de entrega não existem.
- O bloco das 13:30–18:00 de segunda já passou sem o núcleo começar; o cronograma está atrasado.

Evidência:

- Data/hora: 2026-11-23 15:33:15 -03.
- `make clean && make` terminou com `build_exit=0`, mas com 4 warnings de variáveis não usadas.
- `./vm addresses.txt lru fifo` imprimiu `Endereço inválido no arquivo:` e encerrou.
- Último commit de código: `8f65e79` em 2026-11-20 00:51:08.

---

## 2026-11-23 15:43 — Prompt 9 — Restaurar o `while`

Prompt:

> entendi. coloque a parte do while que estava dando certo para eu revisar e dar uma olhada.

O que tentei:

- Recolocar o `while (fgets(...))` em volta da leitura e da conversão.
- Manter a decisão de usar `strtoul`, `endptr`, `errno` e `ERANGE`.
- Aceitar espaço no fim da linha e armazenar cada endereço em `addresses[address_count++]`.

Resultado:

- O parser voltou a ler o arquivo inteiro: `./vm addresses.txt lru fifo` terminou com `exit=0` e imprimiu `se chegou aqui, leu tudo certo`.
- Ainda restam warnings de `page_table`, `offsets` e `page_numbers` não usados.
- Tradução, page table em uso, TLB, threads e estatísticas continuam não implementados.

Evidência:

- Data/hora: 2026-11-23 15:43:33 -03.
- `make clean && make` terminou com `build_exit=0` e 3 warnings.
- `git diff -- vm.c` mostra o `while` restaurado em volta do parser.

---

## 2026-11-23 22:08 — Prompt 10 — Análise e novo modo de trabalho

Prompt:

> analise meu codigo ate agora e atualize nosso cronograma. a partir de agora, eu irei te pedir para implementar algumas coisas e eu vou validar, por conta de nosso curto periodo de tempo

O que tentei:

- Analisar o `vm.c` atual, rodar o build e executar o programa.
- Testar também em `/tmp` com a backing store no nome esperado pelo código.
- Comparar o que existe com o cronograma e com a rubrica.

Resultado:

- Já existem: parser lendo 1000 endereços, page table de 256 entradas, memória física de 128 frames, alocação inicial por page fault e contagem de page faults.
- O programa abre `backing_store.bin`, mas o arquivo real é `BACKING_STORE.bin`; em Linux isso falha por diferença de maiúsculas/minúsculas.
- Com o nome corrigido em `/tmp`, o programa chega até “Memória física cheia” na página 190, porque ainda não há substituição.
- Faltam: FIFO/LRU de página, saída de endereço físico e byte, estatísticas, TLB, threads, `diff` com `correct.txt`, README, relatório e pacote.
- Novo modo de trabalho: o assistente implementa um bloco por vez; a aluna compila, roda a validação e confirma antes do próximo bloco.
- Cronograma atualizado para a revisão 3 em `docs/cronograma.md`, com corte dos fluxos de 256 frames/sem TLB para priorizar o núcleo e o relatório.

Evidência:

- Data/hora: 2026-11-23 22:08:09 -03.
- `./vm addresses.txt lru fifo` no repositório: `Erro ao abrir o arquivo backing_store.bin`, `run_exit=1`.
- Em `/tmp`, com `backing_store.bin`: `Memória física cheia. Não é possível carregar a página 190`, `exit=1`.
- Build com warning de `offset` não usado.

---

## 2026-11-23 22:32 — Prompt 11 — Erro na página 190

Prompt:

> ok. corrigi o nome do arquivo e ele abriu, mas esta dizendo que nao conseguiu carregar a pagina 190

O que tentei:

- Corrigir o nome da backing store para `BACKING_STORE.bin`.
- Rodar o programa com `addresses.txt`.
- Verificar em qual ponto do arquivo a página 190 aparece e quantas páginas únicas existem.

Resultado:

- O erro agora é esperado: os 128 frames livres foram preenchidos e a página 190 é a **129ª página única** do arquivo.
- Primeira ocorrência da página 190: linha 172, endereço lógico 48855.
- O arquivo tem 244 páginas únicas; portanto, 116 páginas precisam de substituição.
- O código ainda não tem FIFO/LRU; o `if (next_frame >= 128)` apenas encerra o programa.
- Próximo passo: implementar substituição de página FIFO e LRU.

Evidência:

- Data/hora: 2026-11-23 22:32:56 -03.
- Simulação sobre `addresses.txt`: página 190 = 129ª única; total de 244 páginas únicas.
- `vm.c` linha 134: `if (next_frame >= 128)`.

---

## 2026-11-23 22:37 — Prompt 12 — FIFO/LRU de página

Prompt:

> certo. implemente a parte do fifo e do lru da pagina para eu validar

O que tentei:

- Substituir o `return 1` de “memória física cheia” por escolha de vítima.
- Manter `frame_to_page[128]` para saber qual página ocupa cada frame.
- Manter `frame_order[128]`: no FIFO guarda a ordem de carga; no LRU guarda o instante do último acesso.
- `access_clock` incrementa a cada carga e, no LRU, também a cada acesso a página já presente.
- No page fault com memória cheia: escolhe o frame de menor `frame_order`, invalida a página antiga, lê a nova página e atualiza tabela e mapa reverso.
- Verificar `fseek` e `fread`.
- Deixar `Page faults = %d` em `stderr` como diagnóstico temporário.

Resultado:

- Build sem warnings.
- `./vm addresses.txt lru fifo` → `Page faults = 539`.
- `./vm addresses.txt fifo fifo` → `Page faults = 538`.
- `lru fifo` repetido 5 vezes: sempre 539.
- Harness temporário (fora do repositório) confirmou 1000/1000 pares (endereço físico, valor) iguais aos de `correct.txt` para a LRU.
- FIFO conferido contra simulação independente em Python: 1000/1000.
- Parser ainda rejeita `12 34`, `abc` e `65536`; aceita `12107 ` com espaço final.
- Ainda não há saída oficial em `stdout`; só o diagnóstico em `stderr`.

Evidência:

- Data/hora: 2026-11-23 22:37:35 -03.
- `make clean && make` → `build_exit=0`.
- `./vm addresses.txt lru fifo` → `Page faults = 539`, `lru_exit=0`.
- `./vm addresses.txt fifo fifo` → `Page faults = 538`, `fifo_exit=0`.
- Simulação: 244 páginas únicas no arquivo; 116 substituições.

---

## 2026-11-23 22:42 — Prompt 13 — Saída oficial, estatísticas e TLB (bloco validado pela aluna)

Prompt anterior validado pela aluna:

> fifo lru -> page faults 538
> lru fifo -> page faults 539
> fifo fifo -> page faults 538
> lru lru -> page faults 539

O que tentei neste bloco:

- Implementar a TLB com 16 entradas, FIFO/LRU pelo `argv[3]`.
- Consultar a TLB antes da page table.
- Inserir a página na TLB a cada TLB miss (após resolver pela page table).
- Invalidar a entrada da TLB quando a página é substituída na memória física.
- Atualizar a recência da página também em TLB hit, para o LRU de página continuar correto.
- Imprimir `Virtual address`, `TLB`, `Physical address` e `Value` em `stdout`.
- Imprimir as cinco estatísticas finais no formato do `correct.txt`.
- Remover o diagnóstico `Page faults` de `stderr`.

Resultado:

- Primeira tentativa deu 537 page faults e `diff` diferente: o TLB hit não atualizava a recência do LRU de página.
- Corrigido: `./vm addresses.txt lru fifo` ficou com 539 page faults, 54 TLB hits e `diff -q correct.txt` com `exit=0`.
- 10 execuções seguidas de `lru fifo`: `diff` zero em todas.
- Combinações medidas:
  - `lru fifo`: 539 page faults, 54 TLB hits
  - `fifo fifo`: 538 page faults, 54 TLB hits
  - `lru lru`: 539 page faults, 55 TLB hits
  - `fifo lru`: 538 page faults, 55 TLB hits
- Nenhuma saída em `stderr`.
- Build sem warnings.

Evidência:

- Data/hora: 2026-11-23 22:42:58 -03.
- `diff -q correct.txt /tmp/out.txt` → `diff_q_exit=0`.
- `make clean && make` → sem warnings.

---

## 2026-11-23 22:47 — Prompt 14 — Busca na TLB com uma thread por entrada

Prompt/validação anterior:

> faltou compilar e rodar. agora resultou o diff resultou em 0

O que tentei:

- Adicionar `#include <pthread.h>`.
- Criar `TlbSearchArg` e a função `tlb_search_entry`, que verifica **uma única** entrada.
- Em cada endereço: criar 16 threads, uma por entrada da TLB, e aguardar todas com `pthread_join`.
- Cada thread escreve apenas no seu próprio `search_args[t]`, sem mutex.
- Depois do join, a primeira entrada com `hit` vira o índice da TLB mostrado na saída.
- Tratar falha de `pthread_create` com mensagem em `stderr` e `return 1`.

Resultado:

- `diff -q correct.txt` → `diff_exit=0`.
- 10 execuções seguidas de `lru fifo`: `diff` zero em todas.
- Combinações: `fifo fifo` 54 hits, `lru lru` 55, `fifo lru` 55; nada em `stderr`.
- Tempo de `./vm addresses.txt lru fifo`: ~0,36 s.
- Build sem warnings.

Evidência:

- Data/hora: 2026-11-23 22:47 -03.

---

## 2026-11-23 23:06 — Prompt 15 — Fluxos da rubrica (256 frames, sem TLB, sem threads)

Prompt:

> vamos fazer todas as rubricas do enunciado (preciso da nota toda). sobre os outros fluxos, como é possivel fazer o fluxo da tlb com 256 frames sendo que estamos trabalhando com 128?

O que tentei:

- Explicar que 256 frames × 256 bytes = 65.536 bytes, o tamanho inteiro da backing store, então o fluxo de 256 frames não tem substituição; ele existe no projeto original do livro e na rubrica, enquanto o enunciado modificado pede 128 frames.
- Parametrizar o `vm.c` com macros: `FRAMES` (128 padrão), `USE_TLB` (1 padrão) e `USE_THREADS` (1 padrão).
- Manter o alvo `make` gerando `vm` exatamente como antes.
- Criar alvos separados no `makefile`:
  - `vm_notlb`: 128 frames, sem TLB.
  - `vm256`: 256 frames, sem TLB.
  - `vm_seq`: 128 frames, TLB sem threads.
  - `all`: compila os quatro.
- No fluxo sem TLB, imprimir `TLB: -1` e estatísticas de TLB zeradas.

Resultado:

- `make` → `vm` com `diff` zero contra `correct.txt`.
- `vm_seq` (TLB sem thread) → `diff` zero e mesmos 539 page faults / 54 hits.
- `vm_notlb` 128 frames: 539 faults no LRU e 538 no FIFO; endereço físico e valor batem com `correct.txt` (LRU) e com simulação FIFO.
- `vm256`: 244 page faults (244 páginas únicas), sem substituição; endereço físico e valor batem com simulação independente.
- Nenhuma saída em `stderr`; build sem warnings.

Evidência:

- Data/hora: 2026-11-23 23:06:02 -03.
- `make all` compila `vm`, `vm_notlb`, `vm256`, `vm_seq`.
- `diff -q correct.txt` em `vm` e `vm_seq` → 0.
- `vm256` → `Page Faults = 244`, `TLB Hits = 0`.

---

## 2026-11-23 23:15 — Prompt 16 — Fluxos em um único executável

Prompt:

> veja, entendi que temos que compilar com outras flags, mas o enunciado diz que temos que ter um executável só. é possivel fazer esse mesmo processo gerando um unico executavel?

O que tentei:

- Trocar as macros de compilação por configuração em tempo de execução via variáveis de ambiente.
- Manter `make` gerando apenas `vm`, com a interface de 3 argumentos intacta.
- Padrões iguais aos do enunciado: 128 frames, TLB ligada, threads ligadas.
- `VM_FRAMES` aceita 1–256 e valida o valor; `VM_TLB=0` desliga a TLB; `VM_THREADS=0` desliga as threads.
- `physical_memory` passou a ser `[256][256]` e o limite usado é `frame_limit`.
- Remover os alvos `vm_notlb`, `vm256` e `vm_seq` do `makefile`; `make clean` apaga resíduos desses nomes.

Resultado:

- `make` → só `vm`.
- `./vm addresses.txt lru fifo` → `diff` zero e 10 execuções sem diferença.
- `VM_THREADS=0 ./vm addresses.txt lru fifo` → `diff` zero.
- `VM_TLB=0 ./vm addresses.txt lru fifo` → 539 faults, 0 hits, físico/valor iguais ao `correct.txt`.
- `VM_TLB=0 VM_FRAMES=256 ./vm addresses.txt lru fifo` → 244 faults, 0 hits, físico/valor iguais à simulação.
- `VM_FRAMES=300` e `VM_FRAMES=abc` → erro e `exit=1`.
- Build sem warnings.

Evidência:

- Data/hora: 2026-11-23 23:15 -03.
- `make clean; make` gera somente `vm`.

---

## 2026-11-24 10:42 — Prompt 17 — README e decisão dos fluxos

Prompt:

> sobre a decisão dos fluxos diferentes, conversei com varias pesoas e elas fizeram de formas diferentes. ou seja, nao tem um padrao especifico. precisamos que tenha um unico arquivo e um unico executável apenas. vamos seguir com essa ideia que vc tinha dito para compilar com as variaveis diferentes. documente isso no readme e diga como posso colocar essa parte no relatorio

O que tentei:

- Confirmar que a implementação atual já tem um único `vm.c`, um Makefile e um único executável `vm`.
- Escrever o `README.md` com: descrição, arquivos, compilação, execução, formato da saída, fluxos por variável de ambiente, tratamento de erros, resultados medidos, validação e organização do código.
- Registrar a decisão: os fluxos da rubrica são reproduzidos no mesmo `vm` por `VM_FRAMES`, `VM_TLB` e `VM_THREADS`, mantendo a interface de 3 argumentos.

Resultado:

- README substituiu o template do professor.
- Interface padrão inalterada: sem variáveis, 128 frames + TLB + threads.
- Nenhuma mudança de código neste passo.

Evidência:

- Data/hora: 2026-11-24 10:42:11 -03.

---

## Pendências registradas

- [x] Corrigir `line[strcspn(line, "\r\n")] = '0';` para `'\0'`.
- [x] Tratar o espaço no fim da última linha de `addresses.txt`.
- [x] Implementar a validação com `strtoul`, `endptr`, `errno` e `ERANGE`.
- [x] Usar os vetores `offsets` e `page_numbers` na tradução (ou removê-los).
- [x] Implementar page table com 256 entradas e 128 frames.
- [x] Implementar FIFO/LRU de página.
- [ ] Remover o diagnóstico `Page faults` de `stderr` quando a saída oficial existir.
- [ ] Imprimir a saída oficial (`Virtual address`, `TLB`, `Physical address`, `Value`) e as estatísticas.
- [ ] Implementar TLB FIFO/LRU e busca com threads.
- [ ] Comparar a saída com `correct.txt`.

## Observação

Este arquivo é um diário de estudo. Não editar resultados depois para “melhorar” a aparência: o relatório precisa refletir o que foi realmente tentado, incluindo falhas.
