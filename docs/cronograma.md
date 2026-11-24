# Cronograma revisado — Memória Virtual

**Revisão 3:** 2026-11-23 22:06 -03
**Entrega:** terça 24/11/2026 até 23:59
**Modo de trabalho a partir de agora:** o assistente implementa um bloco; você compila, roda o comando de validação e confirma. Se falhar, você cola a saída e o bloco é corrigido antes de avançar.

## Status atual medido (23/11, 22:06)

Já existe:

- [x] Makefile gera `vm` e `make clean` funciona.
- [x] Validação de `argc`, arquivo e algoritmos `fifo`/`lru`.
- [x] Parser com `strtoul` lê os 1000 endereços.
- [x] Máscara de página/offset.
- [x] Page table com 256 entradas.
- [x] Memória física com 128 frames de 256 bytes.
- [x] Alocação dos 128 primeiros frames e leitura da backing store por page fault.
- [x] Contagem de page faults (ainda não impressa).
- [x] Nome da backing store corrigido para `BACKING_STORE.bin`.

Ainda falta:

- [x] Implementar substituição de página FIFO e LRU quando os 128 frames encherem.
  - LRU: 539 page faults; bateu endereço físico e valor com `correct.txt`.
  - FIFO: 538 page faults; bateu com simulação independente.
  - Diagnóstico temporário `Page faults = ...` foi removido quando a saída oficial entrou.
- [x] Calcular e imprimir endereço físico e byte.
- [x] Imprimir estatísticas no formato do `correct.txt`.
- [x] Implementar TLB FIFO/LRU (sequencial).
- [x] Implementar a busca na TLB com uma thread por entrada (16 threads por consulta, `diff` zero em 10 execuções).
- [x] Fechar o `diff` com `correct.txt` para `lru fifo`.
- [x] Fluxos da rubrica em um único `vm`, via variáveis de ambiente: `VM_FRAMES`, `VM_TLB=0`, `VM_THREADS=0`.
- [x] README completo, com compilação, uso, fluxos por variável de ambiente, erros, resultados e validação.
- [ ] Relatório, PDF e pacote de entrega.

Teste de agora:

```text
./vm addresses.txt lru fifo > /tmp/out.txt
diff -q correct.txt /tmp/out.txt   # exit=0
Number of Translated Addresses = 1000
Page Faults = 539
Page Fault Rate = 0.539
TLB Hits = 54
TLB Hit Rate = 0.054
```

10 execuções seguidas de `lru fifo` deram `diff` zero. Combinações: `fifo fifo` 538/54, `lru lru` 539/55, `fifo lru` 538/55.

## Prioridade

1. Backing store + checagens de `fseek`/`fread`.
2. Substituição de página FIFO e LRU.
3. Saída: endereço físico + valor + estatísticas.
4. TLB FIFO/LRU sem thread.
5. `diff` zero com `correct.txt`.
6. TLB com thread.
7. Erros, README, relatório e entrega.

**Corte já decidido:** não implementar os fluxos separados de 256 frames/sem TLB. Eles valem 20% da rubrica, mas não cabem nas ~12h restantes. Se sobrar tempo na terça, fazer só uma flag simples de compilação.

## Segunda 23/11 — noite/madrugada

| Horário | Bloco | Validação sua |
|---|---|---|
| 22:10–22:30 | Corrigir caminho `BACKING_STORE.bin`, abrir uma vez, checar `fseek`/`fread`, imprimir endereço físico e byte | `./vm addresses.txt lru fifo` passa dos 128 frames; byte correto |
| 22:30–23:30 | FIFO e LRU de página, escolhidos por `argv[2]` | Roda os 1000 endereços sem “memória cheia” |
| 23:30–00:30 | TLB FIFO/LRU sem thread + formato exato + estatísticas | `diff` zero em `lru fifo` |
| 00:30–01:45 | TLB com 16 threads por consulta | `diff` zero em 10 execuções |
| 01:45–02:00 | Commit, log e anotações para o relatório | Baseline congelada |
| 02:00–02:30 | Buffer máximo | Só bug que impeça o `diff` |

**Checkpoint decisivo:** 00:30 com `diff` zero. Se não chegar, não faça threads; corrija a TLB sequencial e durma.

## Terça 24/11 — fechamento

| Horário | Bloco | Validação sua |
|---|---|---|
| 13:30–14:30 | Terminar threads ou corrigir o que ficou da noite | `diff` zero em 10 execuções |
| 14:30–15:30 | Erros, warnings, valgrind/ASan | Casos inválidos + build limpo |
| 15:30–18:00 | Relatório com evidências reais | PDF quase pronto |
| 18:00–19:00 | Pausa | — |
| 19:00–20:30 | README, PDF, `lgsc.tar`, teste de extração | Tar compila em `/tmp` |
| 20:30–21:30 | GitHub + Classroom; baixar e conferir o envio | Entrega confirmada |
| 21:30–22:30 | Buffer e confirmação final | **Enviar até 22:30** |

## Pontos de corte

- **23:00 sem substituição de página:** implemente só FIFO primeiro; LRU depois. Sem isso não há `diff`.
- **00:30 sem `diff` zero:** cancele threads. Entregue TLB sequencial.
- **Terça 15:30 sem `diff` zero:** cancele relatório “bonito”; use o template e entregue o que existe, com limitações registradas.
- **Terça 18:00:** congele o código. Só relatório, PDF e tar.
- **Threads quebrando o `diff`:** remova as threads e mantenha a versão sequencial. O `diff` zero vale mais.

## Escopo mínimo aceitável

Se o tempo acabar, entregue:

- parser lendo 1000 endereços;
- page table + 128 frames + `BACKING_STORE.bin`;
- FIFO e LRU de página;
- endereço físico, byte e estatísticas;
- TLB FIFO/LRU sequencial com `diff` zero;
- tratamento de erro;
- README;
- relatório com o que foi feito e limitações;
- tar + PDF.

Isso cobre a maior parte da rubrica. Threads e fluxos com 256 frames são os primeiros cortes.

## Comandos de validação

```bash
make clean && make

./vm addresses.txt lru fifo > /tmp/saida.txt
diff -u correct.txt /tmp/saida.txt

for i in $(seq 1 10); do
    ./vm addresses.txt lru fifo > /tmp/saida_$i.txt || exit 1
    diff -q correct.txt /tmp/saida_$i.txt || exit 1
done
```

## Evidência

- `script -a evidencias.log` em toda sessão.
- Commit depois de cada bloco validado.
- Se um bloco falhar, colar a saída exata no chat e não avançar.
- Não editar o log depois.

## Entrega

**GitHub:** código, Makefile, README, `evidencias.log`, `lgsc.pdf`.

**Classroom:** `lgsc.pdf` + `lgsc.tar` com a pasta `lgsc/` contendo código, Makefile, README e `evidencias.log`.

**Meta:** enviar até terça 22:30.
