# Cronograma revisado — Memória Virtual

**Revisão 2:** 2026-11-20 00:39 -03
**Entrega:** terça 24/11/2026 até 23:59
**Premissa:** sexta 20, sábado 21 e domingo 22 continuam indisponíveis. Segunda e terça têm aula pela manhã; o trabalho acontece à tarde, à noite e madrugando.
**Carga real:** cerca de 10h na segunda + 9h na terça = **~19h**. É apertado; se você perder 2h, corte os extras, não o relatório.

## Estado atual medido (20/11, 00:39)

- [x] `make` compila e gera `vm`.
- [x] `argc != 4` corrigido.
- [x] `line[strcspn(line, "\r\n")] = '\0'` corrigido.
- [x] Máscara de página/offset escrita corretamente.
- [ ] Parser lê 999 de 1000 endereços: falha no último `12107 ` por causa do espaço.
- [ ] Ainda imprime `Caractere lido:` para cada dígito.
- [ ] Tradução para endereço físico não implementada.
- [ ] Page table não está sendo usada.
- [ ] FIFO/LRU de página não implementados.
- [ ] TLB e threads não implementados.
- [ ] Estatísticas e `diff` com `docs/correct.txt` não feitos.
- [ ] README e relatório não finalizados.

## Ordem de prioridade

1. **Fechar o parser** e ler os 1000 endereços sem debug.
2. **Traduzir endereço** com page table, `BACKING_STORE.bin` e 128 frames.
3. **FIFO e LRU de página**.
4. **TLB FIFO/LRU sem thread**, até o `diff` zero com `docs/correct.txt`.
5. **TLB com thread**.
6. **Tratamento de erro + README + relatório**.
7. **Empacotamento e envio**.

O relatório vale 20%, README/Makefile 10% e tratamento de erro 10%. Não sacrifique esses 40% para tentar ganhar os 10% do fluxo com 256 frames.

## Segunda 23/11 — núcleo funcionando

| Horário | Tarefa | Saída/checkpoint |
|---|---|---|
| 13:30–14:00 | Trocar o loop de caracteres por `strtoul` + `endptr` + `errno`; aceitar `12107 `; remover prints de debug | 1000 endereços lidos, `exit=0` |
| 14:00–16:00 | Page table com 256 entradas, `BACKING_STORE.bin`, 128 frames, tradução e saída | Endereço físico + byte; invariante do byte |
| 16:00–17:00 | FIFO de página + lista de frames livres | Page faults; checker independente |
| 17:00–18:00 | LRU de página | Mapeamento de frames coerente |
| 18:00–19:00 | Jantar/pausa | — |
| 19:00–20:30 | TLB FIFO/LRU **sem threads** e formato exato | **`diff` zero em `lru fifo`** |
| 20:30–22:30 | TLB com 16 threads por consulta | `diff` zero em 10 execuções |
| 22:30–00:30 | Tratamento de erro, warnings, valgrind/ASan | Matriz de erros + build limpo |
| 00:30–01:00 | Commit, log salvo e esqueleto do relatório | Baseline congelada |
| 01:00–02:00 | **Buffer** só para bug que impede o `diff` | Não começar feature nova |

**Checkpoint mais importante:** até 20:30 a saída de `./vm addresses.txt lru fifo` precisa ser idêntica a `docs/correct.txt`. Sem isso, não avance para threads.

### Se madrugar

Pare às 02:00. Código depois de 3h de sono rende menos que código descansado. Se não conseguiu dormir, pelo menos não comece uma feature nova às 02:00; use o tempo para testar e registrar evidência.

## Terça 24/11 — robustez, relatório e entrega

| Horário | Tarefa | Saída/checkpoint |
|---|---|---|
| 13:30–15:00 | Build do zero, 10 execuções, `-Wall -Wextra -Werror`, ASan/UBSan, valgrind | Tudo estável |
| 15:00–16:00 | **Opcional:** fluxos 256 frames / sem TLB / TLB sem thread e evidência | Logs dos fluxos |
| 16:00–18:00 | Relatório seções 1–8 + checklist + prints | PDF quase pronto |
| 18:00–19:00 | Jantar/pausa | — |
| 19:00–20:30 | README, Makefile, `lgsc/`, `lgsc.tar`, PDF | Pacote montado |
| 20:30–21:30 | Extrair o tar em `/tmp`, compilar e rodar; subir GitHub e Classroom | Arquivos enviados |
| 21:30–22:30 | Baixar o que foi enviado, conferir nomes e conteúdo; buffer | **Enviar até 22:30** |
| 22:30–23:59 | Reserva para problema de upload | Não deixar para 23:59 |

## Pontos de corte

- **Segunda 18:00 sem tradução funcionando:** esqueça 256 frames, sem TLB e threads. Faça 128 frames + FIFO/LRU + TLB sequencial + relatório.
- **Segunda 22:30 sem `diff` zero:** pare as threads. O `diff` zero vale mais que a thread; corrija a TLB sequencial primeiro.
- **Terça 15:00 sem `diff` zero:** cancele os fluxos opcionais e use o tempo para corrigir o núcleo e salvar o relatório.
- **Terça 18:00:** congele o código. Só relatório, PDF e empacotamento.
- **Threads instáveis:** entregue a TLB sequencial, registre a limitação no relatório e não troque o que funciona por um recurso que quebra o `diff`.

## Escopo mínimo para não zerar

Se o tempo acabar, priorize nesta ordem:

1. Parser lendo 1000 endereços.
2. Page table 256 + 128 frames + `BACKING_STORE.bin`.
3. Tradução e byte corretos.
4. LRU de página (é o que a referência `correct.txt` usa).
5. TLB FIFO sem thread, para bater o formato e as estatísticas.
6. Tratamento de erro dos argumentos e do arquivo.
7. README + Makefile.
8. Relatório com evidências reais.
9. Tar + PDF.

Isso já cobre a maior parte da rubrica. **TLB com thread** e **fluxos com 256 frames/sem TLB** são os extras a cortar primeiro.

## Se abrir uma janela no fim de semana

Não conte com isso, mas se conseguir 2–4h:

- **2h:** feche o parser e implemente a tradução com 256 frames + `BACKING_STORE.bin`.
- **4h:** adiante FIFO/LRU e a TLB sequencial até o `diff` zero.
- **1h:** escreva o esqueleto do relatório e o README.

Qualquer avanço no fim de semana reduz o risco de a segunda virar uma noite perdida.

## Alvos de aceitação

| Cenário | Page faults | TLB hits | Validação |
|---|---:|---:|---|
| `lru fifo` (128 frames) | 539 / 0.539 | 54 / 0.054 | `diff` exato com `docs/correct.txt` |
| `fifo fifo` (128 frames) | 538 / 0.538 | 54 / 0.054 | Invariante do byte + estatísticas |
| `lru lru` (128 frames) | 539 / 0.539 | 55 / 0.055 | Invariante do byte + estatísticas |
| 256 frames | 244 / 0.244 | 54 ou 55 | Sem substituição; 244 páginas únicas |

O campo `TLB:` do `correct.txt` é o **índice da entrada da TLB (0–15)**, não o frame.

## Comandos de checkpoint

```bash
make clean && make

./vm addresses.txt lru fifo > /tmp/saida_lru_fifo.txt
diff -u docs/correct.txt /tmp/saida_lru_fifo.txt

for i in $(seq 1 10); do
    ./vm addresses.txt lru fifo > /tmp/saida_$i.txt || exit 1
    diff -q docs/correct.txt /tmp/saida_$i.txt || exit 1
done
```

Para validar o byte sem depender da TLB, use um checker independente em Python: para cada linha, o valor deve ser o byte assinado de `BACKING_STORE.bin` no endereço lógico original.

## Evidência

- `script -a evidencias.log` em toda sessão.
- `date; whoami; pwd` no começo.
- Commit a cada checkpoint: parser, tradução, FIFO, LRU, TLB, threads, relatório.
- Registre o comando e a saída do `diff`, inclusive quando falhar.
- Não edite o log depois.

## Entrega

**GitHub:** código, Makefile, README, `evidencias.log` e `lgsc.pdf`.

**Classroom:** `lgsc.pdf` + `lgsc.tar`. Dentro do tar: pasta `lgsc/` com código, Makefile, README e `evidencias.log`. Sem `docs/base`, `addresses.txt`, `correct.txt`, `BACKING_STORE.bin` ou binários.

**Meta de envio:** terça 22:30. O prazo é 23:59, mas 1h30 de margem é o mínimo para problema de upload.
