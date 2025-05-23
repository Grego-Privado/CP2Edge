# 🍷 Vinheria Data Logger

Este projeto é um sistema completo de **monitoramento ambiental inteligente** voltado para vinherias. Ele utiliza sensores para capturar informações de temperatura, umidade e luminosidade, registra eventos críticos na EEPROM e permite controle via cartão RFID e teclado numérico.

---

## 🔧 Funcionalidades

- 📟 Interface em LCD I2C 16x2 com animações e menus
- 🌡️ Leitura de **temperatura e umidade** com sensor DHT22
- 💡 Leitura de **luminosidade ambiente** com LDR
- 🕒 Registro de eventos com **data e hora** usando módulo RTC (DS3231)
- 🛂 Controle de acesso via **cartão RFID** (MFRC522)
- 🎛️ Interação via **teclado matricial 4x3** para configurar limites e iniciar monitoramento
- 💾 Armazenamento dos eventos fora dos limites em **EEPROM**
- 🔔 Alerta com **LED e buzzer** quando os valores passam do ideal

---

## 📦 Componentes usados

- Arduino (UNO, Nano ou compatível)
- Módulo **DHT22** (temperatura/umidade)
- **LDR** + resistor (luminosidade)
- Módulo **RTC DS3231**
- Módulo **RFID RC522**
- Display **LCD I2C 16x2**
- **Teclado matricial 4x3**
- Buzzer e LED para alerta
- Resistores, fios, protoboard ou PCB

---

## 📋 Como funciona

1. Ao iniciar, o sistema mostra logs armazenados na EEPROM.
2. Exibe o menu após autenticação via cartão RFID autorizado.
3. Permite ajustar os valores máximos de:
   - Temperatura (`Tmax`)
   - Umidade (`Umax`)
   - Luminosidade (`Lmax`)
4. Ao entrar em modo de monitoramento, o sistema:
   - Lê continuamente os sensores
   - Calcula média de luminosidade
   - Ativa alerta e registra evento caso algum valor passe do limite
5. Os registros ficam salvos na EEPROM com timestamp.

---

## 🖥️ Interface no LCD

- Menu principal:
1:Tmax 2:Umax
3:Lmax 9:Monitor
- Feedbacks:
- "Acesso Liberado/Negado"
- "!!! ALERTA !!!"
- Valores atuais em tempo real

---

## 📂 Organização do código

- Funções separadas para:
- Leitura de sensores
- Controle de estado (RFID, menu, monitoramento)
- Escrita/leitura na EEPROM
- Animações de LCD
- Uso de enums para estados do sistema (`AGUARDANDO_RFID`, `MENU_PRINCIPAL`, etc.)

---

## ⚠️ Limites padrão

- `Tmax`: 30.0 °C  
- `Umax`: 60.0 %  
- `Lmax`: 70 % (relativo à escala do LDR)

---

## 🧪 Testes e ajustes

- Use o botão '8' no teclado para mostrar os logs
- Use '7' para apagar a EEPROM
- Monitore pelo `Serial Monitor` para ver dados salvos com data e hora

---


## 👨‍💻 Autor

**Nome:** Pedro Henrique Silva GregoliniVictor Simões Altieri, Rafael Falaguasta Ferraz   
**Projeto acadêmico para:** FIAP

---
