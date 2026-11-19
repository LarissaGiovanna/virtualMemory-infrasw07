# Cronograma — Memória Virtual (Infraestrutura de Software)

**Entrega:** terça 24/11/2026 até 23:59
**Indisponível:** sexta 20/11, sábado 21/11 e domingo 22/11
**Janelas de trabalho:** quarta 18/11, quinta 19/11, segunda 23/11 e terça 24/11
**Meta:** chegar na quinta à noite com o fluxo principal funcionando e o relatório iniciado.

## Status atual (18/11/2026, 19:55)

- [x] Makefile gera `vm` e `make clean` funciona.
- [x] `evidencias.log` iniciado.
- [x] Leitura do arquivo e início da extração de página/offset.
- [ ] Validação correta das linhas do arquivo.
- [ ] Tradução de endereço e leitura do `BACKING_STORE.bin`.
- [ ] Page table com 256 entradas e 128 frames.
- [ ] Substituição de página FIFO/LRU.
- [ ] TLB FIFO/LRU.
- [ ] Busca na TLB com threads.
- [ ] Estatísticas e `diff` com `docs/correct.txt`.
- [ ] README, relatório e empacotamento.

## Quarta 18/11 — base funcionando

| Horário | Tarefa | Saída/evidência |
|---|---|---|
| 18:30–19:00 | `script -a evidencias.log`, `date; whoami; pwd`, `git status`, `make clean`; Makefile com target `vm` e `-pthread` | Log iniciado; `make` compila |
| 19:00–20:15 | Ler arquivo, validar linhas, mascarar 16 bits e extrair página/offset. Testar `1, 256, 32768, 32769, 128, 65534, 33153` | Página/offset corretos; erros de arquivo inválido |
| 20:15–21:45 | Page table com 256 entradas + `BACKING_STORE.bin` + 256 frames + demand paging; formato de saída e estatísticas | Tradução + byte; checker independente |
| 21:45–23:00 | Reduzir para 128 frames e implementar substituição FIFO de página | Page faults/FIFO; commit do marco |

Se render menos, o mínimo é: **Makefile + parser + page table**.

## Quinta 19/11 — dia decisivo

| Horário | Tarefa | Saída/evidência |
|---|---|---|
| 08:30–10:00 | LRU de página + frames livres + invalidação da page table | 128 frames com LRU |
| **10:00–12:00** | TLB FIFO e LRU **sem threads**; formato exato; comparar com `docs/correct.txt` | **CHECKPOINT: `diff` zero em `lru fifo`** |
| 13:30–15:30 | TLB com **16 threads por consulta** (uma por entrada), `pthread_join`, resultado protegido | `diff` continua zero; 10 execuções |
| 15:30–17:00 | Erros: sem args, 2/4 args, algoritmo inválido, arquivo inexistente/ilegível/formatado errado; valgrind/ASan | Mensagens coerentes, exit code, log |
| 17:00–17:30 | Congelar: build limpo, commit, log salvo; não deixar WIP | Baseline estável para o fim de semana |
| 19:00–21:30 | README + relatório seções 1–4 e esqueleto 5–8; diário de tentativas | Relatório já começado |

**Meta da quinta:** `./vm docs/addresses.txt lru fifo` idêntico a `docs/correct.txt`.

## Sexta 20 a domingo 22 — indisponível

Não programe. Só garanta que o repositório ficou compilando e o log salvo.

## Segunda 23/11 — robustez + relatório

| Horário | Tarefa | Saída/evidência |
|---|---|---|
| 09:00–10:30 | Build do zero, 10 execuções, `-Wall -Wextra -Werror`, ASan/UBSan, valgrind | Sem warnings/leaks; resultados iguais |
| 10:30–12:00 | Casos-limite e fluxos da rubrica: 256 frames, sem TLB, TLB sem thread (flags de compilação) | Log de cada fluxo |
| 13:30–15:30 | Relatório seções 5–8, checklist de requisitos e prints obrigatórios | Relatório ~90% |
| 15:30–17:30 | README/Makefile finais; montar `lgsc/`, gerar `lgsc.tar`, extrair e compilar em `/tmp` | Tar testado fora do repo |
| 19:00–21:00 | Ensaio completo: build limpo, 3 algoritmos, diff, abrir PDF, revisar nomes | Entrega pronta |

## Terça 24/11 — revisão e envio

| Horário | Tarefa | Saída/evidência |
|---|---|---|
| 09:00–10:30 | Auditar rubrica item por item; cada afirmação com evidência | Checklist final preenchido |
| 10:30–12:00 | Build final a partir do tar extraído; conferir GitHub e PDF | Tudo reproduzível |
| 14:00–15:30 | Subir GitHub e Classroom; baixar os arquivos enviados e testar | Entrega confirmada |
| 15:30–16:30 | Buffer e confirmação final | **Enviar até 17:00** |

Não deixe para 23:00: o Classroom pode falhar.

## Pontos de corte

- **Quinta 12:00 sem `diff` zero:** pare os extras (256 frames, sem TLB) e corrija o fluxo principal.
- **Quinta 21:30 sem baseline estável:** segunda começa por código, não por relatório; o esqueleto do relatório já estará salvo.
- **Segunda 17:30 com tar não testado:** pare de codar e entregue o que funciona.
- **Terça 12:00:** só relatório e empacotamento; nada de mudança grande no código.

## Prioridade pela rubrica

| Item | Peso | Observação |
|---|---:|---|
| Relatório | 20% | Não deixar para a última hora; precisa de log e diário |
| README + Makefile | 10% | Pontos relativamente baratos |
| Tratamento de erro | 10% | Testar argumentos e arquivo inválido |
| Tradução de endereços | 5% | Máscara, página, offset, byte |
| 256 frames sem TLB | 10% | Evidenciar o fluxo intermediário |
| 128 frames FIFO sem TLB | 10% | Evidenciar o fluxo intermediário |
| 128 frames LRU sem TLB | 15% | Maior peso da parte de páginas |
| TLB FIFO/LRU sem thread | 10% | 5% + 5% |
| TLB com thread | 10% | Uma thread por entrada |

## Alvos de aceitação medidos

Simulação independente sobre os 1000 endereços de `docs/addresses.txt`:

| Cenário | Page faults | TLB hits | Como validar |
|---|---:|---:|---|
| `lru fifo` (128 frames) | 539 / 0.539 | 54 / 0.054 | `diff` exato com `docs/correct.txt` |
| `fifo fifo` (128 frames) | 538 / 0.538 | 54 / 0.054 | Invariante do byte + estatísticas |
| `lru lru` (128 frames) | 539 / 0.539 | 55 / 0.055 | Invariante do byte + estatísticas |
| 256 frames | 244 / 0.244 | 54 ou 55 | Sem substituição; 244 páginas únicas |

O campo `TLB:` do `correct.txt` é o **índice da entrada da TLB (0–15)**, não o frame. Simulei FIFO de 16 entradas: bate nas 1000 linhas, com 54 hits. Imprimir o frame nesse campo faz o `diff` falhar mesmo com a tradução correta.

## Regras de evidência

- Toda sessão começa com `script -a evidencias.log` e `date; whoami; pwd`.
- Commit a cada marco; **você** commita, o assistente não mexe no histórico.
- Registre também o que falhou: comando, erro exato e o que tentou antes.
- Rode o teste oficial **10 vezes**, não uma.
- Mantenha o diário pessoal fora do commit: use `docs/diario.local.md` e coloque em `.git/info/exclude`.
- Não edite o `evidencias.log` depois; print principal do bug e print da validação final são obrigatórios.

## Armadilhas que vão custar tempo

1. `TLB: N` é índice da entrada, não frame.
2. Ao substituir uma página, invalide a entrada antiga da page table **e** qualquer entrada da TLB que aponte para ela.
3. Page table tem 256 entradas; memória física tem 128 frames. Não confunda.
4. LRU de página deve atualizar a recência em **todo acesso**, inclusive TLB hit.
5. TLB com threads: 16 threads por consulta, uma por entrada; `pthread_join` em todas; resultado compartilhado com mutex/atômico.
6. Não sobrescreva o `correct.txt` de referência. Gere sua saída em `/tmp` ou no root e compare com `docs/correct.txt`.
7. Valide cada linha com `strtoul` (nunca `atoi`); rejeite lixo, vazio, negativo e linha incompleta. Decida e documente se aceita inteiros de 32 bits mascarando os 16 bits ou apenas `0..65535`.
8. `BACKING_STORE.bin` deve ser aberto no diretório de execução, não em `docs/`.
9. Use `signed char` para o valor e imprima como `int`.
10. Adicione `vm` e `program` ao `.gitignore` para não commitar binário.

## Entrega

**GitHub:** código, Makefile, README, `evidencias.log` e relatório `lgsc.pdf`.

**Classroom:** `lgsc.pdf` + `lgsc.tar`. Dentro do tar: pasta `lgsc/` com código, Makefile, README e `evidencias.log`. Não inclua `docs/base`, `addresses.txt`, `correct.txt`, `BACKING_STORE.bin` nem binários.

Confirme suas iniciais: o `.gitignore` já tem `!lgsc.pdf`, então provavelmente é `lgsc`. Teste o tar em `/tmp` antes de enviar.

## Comece agora

```bash
cd "/media/larissa/Disco Local_SSD/backup disco e/cc/3-periodo/infra soft/atividades/virtualMemory-infrasw07"
script -a evidencias.log
date; whoami; pwd
git status
make clean
```
