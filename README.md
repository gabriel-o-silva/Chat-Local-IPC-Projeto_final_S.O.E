# Local IPC Chat – Comunicação Entre Processos com Pipes Nomeados

##  Objetivo
Demonstrar o uso de:
- Processos e threads POSIX,
- Comunicação entre processos (IPC) 
- Controle de entrada/saída concorrente.

---

##  Arquitetura do Projeto

projeto_final/LocalIPCChat

 ├──common.h
 
 ├── server.c
 
 ├── user.c
  
 └── Makefile (se quiser)
 


O sistema tem 2 processos:

### **1. Servidor**
- Aguarda mensagens do cliente
- Responde usando 
- Usa threads para:
  - Ler mensagens
  - Enviar mensagens

### **2. Cliente**
- Envia mensagens para o servidor
- Recebe respostas
- Também possui duas threads internas


##  Como Executar

### 1.Abra o editor de texto nano no terminal da sua máquina( ao digitar a função no terminal: "nano NOME_DO_AQRUIVO"(se n houver o arquivo de texto com o nome que você colocar no lugar de "NOME_DO_AQRUIVO", ele irá criar um no diretório em que está).

### 2.já no diretório de destino, adicione os códigos no mesmo diretório: common.h, server.c, user.c, Makefile.(Exemplo: no terminal escreva: nano common.h, o terminal abrirá o editor de texto nano onde você poderá adicionar o código, no caso, nomeado de common.h)

### 3. Compilar, apenas dê "make" no terminal se você configurou certo o "makefile". ele irá compilcar todos os códigos automaticamente(irá liberar os comandos: ./server e ./user.


### 2. Executar o programa
### tendo compilado, abra os terminais no mesmo diretório projeto_final/LocalIPCChat, mínimo de 2. Em um, digite: ./server(irá ser sua central), em outro terminal digite: ./user(será o usuário)

chat estabelecido :D
