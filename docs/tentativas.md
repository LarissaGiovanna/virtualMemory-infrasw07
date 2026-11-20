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

## Pendências registradas

- [ ] Corrigir `line[strcspn(line, "\r\n")] = '0';` para `'\0'`.
- [ ] Tratar o espaço no fim da última linha de `addresses.txt`.
- [ ] Implementar a validação com `strtoul`, `endptr`, `errno` e `ERANGE`.
- [ ] Usar os vetores `offsets` e `page_numbers` na tradução (ou removê-los).
- [ ] Implementar page table com 256 entradas e 128 frames.
- [ ] Implementar FIFO/LRU de página.
- [ ] Implementar TLB FIFO/LRU e busca com threads.
- [ ] Comparar a saída com `docs/correct.txt`.

## Observação

Este arquivo é um diário de estudo. Não editar resultados depois para “melhorar” a aparência: o relatório precisa refletir o que foi realmente tentado, incluindo falhas.
