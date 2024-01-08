# Projeto de Desenvolvimento: Sensor de Rastreamento de Barris
<div style="display: flex; justify-content: space-between;">
  <img src="https://github.com/ZeroGalack/SensorBarril-V1.0.0/blob/main/sensor.png" alt="Imagem 1" width="45%">
</div>

---

## Descrição

Este repositório contém o código-fonte e a documentação para um projeto de sensor de rastreamento de barris. O sistema é composto por um microcontrolador ESP32, um módulo GPS NEO6, um módulo 2G SIM8000L e um sensor de temperatura BMP280.

O sensor de rastreamento de barris proporciona a capacidade de monitorar a localização de barris durante todo o seu trajeto, além de monitorar a temperatura do ambiente. Essa funcionalidade permite um controle mais eficiente da logística de transporte e armazenamento de materiais, resultando em operações mais precisas e otimizadas.

### Componentes Principais:

- **Microcontrolador ESP32:** Utilizado como o cérebro do sistema, o ESP32 gerencia a coleta de dados, processamento e controle do sensor.

- **Módulo GPS NEO6:** Garante a precisão na coleta de dados de localização, permitindo o rastreamento preciso da localização do barril.

- **Módulo 2G SIM8000L:** Possibilita a transmissão dos dados via rede celular, permitindo o monitoramento em tempo real da localização do barril.

- **Sensor de Temperatura BMP280:** Monitora a temperatura ambiente durante o trajeto do barril, fornecendo informações adicionais sobre as condições de transporte.

### Autenticação e Envio de Dados

O sistema utiliza autenticação com hash MD5 para garantir a integridade e segurança dos dados. Os dados coletados, incluindo localização e temperatura, são enviados por meio de um método POST para garantir a transmissão segura.

### Otimização do Consumo de Energia

A lógica do código foi projetada para otimizar o consumo de energia, garantindo uma eficiência energética durante o funcionamento do sensor. Estratégias como o uso de modos de baixo consumo e agendamento inteligente de leituras contribuem para a maximização da autonomia do sistema com uma fonte de energia limitada.
