# Interpretador Shell

## Objetivo
Implementar um interpretador de comandos em terminal para Linux.

## Requisitos 

O interpretador de comandos deverá:
> 1. Executar comandos unitários, com múltiplos parâmetros. Exemplo:
# ls -la
2. Executar comandos encadeados, utilizando o operador pipe “|” para combinar saída e entrada
entre n comandos. Exemplo:
# ls -la | grep “teste”
# cat /proc/cpuinfo | grep "model name" | wc -l
3. Executar comandos condicionados com operadores OR (||) e AND (&&). Exemplo:
# cat 12345 || echo "aquivo inexistente"
# ping -c1 www.unifesp.br.br && echo "SERVIDOR DISPONIVEL" || echo "SERVIDOR
INDISPONIVEL"
4. Executar comandos em background, liberando o shell para receber novos comandos do usuário.
# ping -c5 www.unifesp.br &
