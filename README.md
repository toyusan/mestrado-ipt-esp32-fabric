# ESP32 Secure Firmware Update Project

## Sumário

1. [Visão Geral](#visão-geral)
2. [Características Principais](#características-principais)
3. [Estrutura dos Arquivos](#estrutura-dos-arquivos)
4. [Fluxo de Atualização de Firmware](#fluxo-de-atualização-de-firmware)
5. [Configurações](#configurações)
6. [Executando os Testes](#executando-os-testes)
7. [Como Construir e Executar](#como-construir-e-executar)
8. [Exemplo de Execução](#exemplo-de-execução)

## Visão Geral

Este projeto foi desenvolvido para gerenciar de forma segura atualizações de firmware em dispositivos ESP32 utilizando comunicação segura via HTTPS. O projeto usa o FreeRTOS para gerenciamento de tarefas e a funcionalidade de Over-The-Air (OTA) do ESP32 para aplicar novas versões do firmware. Todas as comunicações são criptografadas com AES para garantir a integridade e segurança dos dados. Além disso, um hash SHA-256 é utilizado para verificar a integridade do firmware antes da atualização.

## Características Principais

- **Conectividade Wi-Fi**: O ESP32 se conecta automaticamente a uma rede Wi-Fi configurável, cujas credenciais podem ser ajustadas no arquivo `sysconfig.h`.
- **Comunicação Segura via HTTPS**: Toda a comunicação com o servidor, tanto para verificação de firmware quanto para download de firmware, é realizada via HTTPS utilizando certificados SSL.
- **Atualizações OTA**: Utiliza o mecanismo OTA do ESP32 para atualizar o firmware do dispositivo de maneira eficiente e segura.
- **Criptografia AES**: O firmware baixado é criptografado com AES-128/AES-256 e armazenado com segurança no dispositivo. O firmware é decriptado antes da aplicação da atualização.
- **Verificação de Integridade**: Um hash SHA-256 é calculado e comparado com o hash fornecido pelo servidor para garantir que o firmware não foi corrompido ou adulterado.
- **Metadados do Firmware**: Gerencia metadados do firmware, como versão, autor, modelo de hardware, hash de integridade e descrição.

## Estrutura dos Arquivos

A seguir está uma visão detalhada dos arquivos que compõem este projeto e suas respectivas funcionalidades:

### Diretório `src/`

- **`main.c`**: Ponto de entrada do aplicativo. Inicializa o FreeRTOS, configura a conexão Wi-Fi, e inicia as tarefas responsáveis pela verificação de atualizações de firmware e a execução das funções principais.
- **`wifi_app.c` / `wifi_app.h`**: Gerencia a conectividade Wi-Fi do dispositivo ESP32. Inclui a lógica para conectar a uma rede definida e reconectar automaticamente em caso de desconexão.
  
- **`https_app.c` / `https_app.h`**: Gerencia as comunicações HTTPS com o servidor para verificar a versão do firmware e baixar atualizações. O download de firmware também é gerenciado por esse módulo.
  
- **`fw_update.c` / `fw_update.h`**: Lida com o processo de decriptação do firmware utilizando AES, além da verificação de integridade usando SHA-256. Também gerencia a aplicação de atualizações de firmware OTA.

- **`main_app.c` / `main_app.h`**: Implementa a lógica principal da aplicação, incluindo a orquestração das etapas de verificação de firmware, download, decriptação e atualização OTA.
  
- **`main_test.c` / `main_test.h`**: Contém funções de teste para simular diferentes cenários (integridade, confidencialidade, etc.) e validar o fluxo de atualização.

- **`tasks_common.h`**: Define as configurações de tarefas como tamanho de pilha e prioridades para as principais tarefas da aplicação.
  
- **`version.h`**: Armazena as definições de versão do firmware e modelo de hardware.

- **`sysconfig.h`**: Contém todas as configurações do sistema, como as credenciais de Wi-Fi, URLs dos servidores de firmware e IPFS, chaves de criptografia AES, e outras constantes importantes para o projeto.

## Fluxo de Atualização de Firmware

O processo de atualização de firmware segue o seguinte fluxo:

1. **Conexão à Rede Wi-Fi**: Após a inicialização, o ESP32 se conecta à rede Wi-Fi usando o SSID e senha definidos no arquivo `sysconfig.h`.
   
2. **Verificação de Atualização**: O dispositivo envia uma solicitação HTTPS ao servidor configurado para verificar se há uma nova versão de firmware disponível, comparando a versão instalada com a mais recente armazenada no servidor.
   
3. **Download de Firmware**: Se uma atualização estiver disponível, o firmware é baixado de forma segura do servidor via HTTPS. O firmware é armazenado na partição OTA do ESP32.
   
4. **Decriptação**: O firmware baixado é criptografado com AES-128. O dispositivo decripta o firmware usando a chave AES e o vetor de inicialização (IV) definidos no projeto.
   
5. **Verificação de Integridade**: Um hash SHA-256 é calculado para o firmware decriptado e comparado com o hash fornecido pelo servidor para garantir que o firmware não foi corrompido durante o download.
   
6. **Aplicação de Atualização**: Se a verificação de integridade for bem-sucedida, o novo firmware é aplicado e o dispositivo é reinicializado para completar o processo de atualização.

## Configurações

### Configuração de Wi-Fi

No arquivo `sysconfig.h`, você pode configurar o SSID e a senha da rede Wi-Fi que o dispositivo ESP32 utilizará para se conectar:

```c
#define PERSONAL_SSID "NomeDaSuaRedeWiFi"
#define PERSONAL_PASS "SuaSenhaWiFi"
```

### Configuração do Servidor

- **Servidor Blockchain**: Usado para verificar a versão do firmware e registrar o dispositivo.
- **Servidor IPFS**: Usado para armazenar e fornecer o firmware para download.

As URLs desses servidores podem ser configuradas em `sysconfig.h`:

```c
#define HTTPS_BLOCKCHAIN_SERVER_URL "https://seu-servidor-blockchain.com"
#define HTTPS_IPFS_SERVER_URL "http://seu-servidor-ipfs.com"
```

### Chaves de Criptografia
Este projeto utiliza criptografia AES-128 para garantir a segurança dos dados durante a transmissão e armazenamento. As chaves AES e o IV podem ser configurados em `sysconfig.h`:

```c
#define AES_KEY {0x37, 0x2a, 0x0e, 0xd9, 0x4f, 0x6b, 0x42, 0xc5, 0xbc, 0x8d, 0x4e, 0x24, 0x25, 0x11, 0x04, 0x67}
#define AES_IV  {0x17, 0xfa, 0xfe, 0xb9, 0x31, 0x0a, 0x23, 0x16, 0x5d, 0x7f, 0x3d, 0x8f, 0xf5, 0x6c, 0x5f, 0x87}
```

### Executando os Testes
O projeto inclui uma suíte de testes para validar a confidencialidade, integridade e autenticidade do processo de atualização de firmware. No arquivo `main_test.h`, você pode ativar ou desativar testes específicos:

```c
#define CONFIDENTIALITY_TEST_ENABLED 1  // Ativa o teste de confidencialidade
#define INTEGRITY_TEST_ENABLED 1        // Ativa o teste de integridade
```

Os resultados dos testes são registrados no console de depuração.

### Como Construir e Executar

#### Requisitos

- ESP-IDF (v5.1 ou superior)
- Ferramentas de desenvolvimento para ESP32 (Python, CMake, Ninja, etc.)

#### Instruções de Compilação

Clone este repositório e navegue até o diretório do projeto:

```bash
git clone https://github.com/seu-usuario/seu-repositorio.git
cd seu-repositorio
```

Configure o ambiente de desenvolvimento ESP-IDF:

```bash
. $HOME/esp/esp-idf/export.sh
```


Compile o projeto:

```bash
idf.py build
```

Flash o firmware no dispositivo ESP32:

```bash
idf.py flash
```

Monitore a saída serial:

```bash
idf.py monitor
```


