# Esonero di Laboratorio UDP - Reti di Calcolatori (ITPS A-L) 2025-26

## Obiettivo Generale
**Migrare** l'applicazione client/server del servizio meteo dal protocollo **TCP** (realizzata nel primo esonero) al protocollo **UDP** (User Datagram Protocol).

L'obiettivo è prendere il codice già scritto per l'assegnazione TCP e modificarlo per utilizzare UDP, mantenendo invariato il protocollo applicativo (strutture dati, formati, funzionalità).

## Cosa Cambia nella Migrazione da TCP a UDP

### Modifiche da Apportare al Codice

Nel passaggio da TCP a UDP, dovrete modificare **solo il livello di trasporto**, lasciando invariato tutto il resto:

**DA MODIFICARE:**
- Tipo di socket
- Chiamate di sistema
- Gestione affidabilità (implementare timeout e ritrasmissione lato client)

**NON MODIFICARE:**
- Protocollo applicativo: strutture `struct request` e `struct response`
- Interfaccia a linea di comando (opzioni `-s`, `-p`, `-r`)
- Logica di business: parsing richieste, validazione città, generazione dati meteo
- Formati di output
- Funzioni `get_temperature()`, `get_humidity()`, `get_wind()`, `get_pressure()`

### Caratteristiche del Protocollo UDP

- **Connectionless**: UDP non stabilisce una connessione prima dello scambio dati
- **Inaffidabilità**: UDP non garantisce la consegna dei pacchetti, né il loro ordine
- **Dimensione pacchetti**: Considerare la dimensione massima dei datagrammi UDP
- **Timeout e ritrasmissione**: Necessario implementare meccanismi di gestione della perdita di pacchetti

## Protocollo Applicativo

**IMPORTANTE**: Il protocollo applicativo rimane **identico** all'assegnazione TCP. Le strutture dati definite in `protocol.h` **NON devono essere modificate**.

### Strutture Dati

Le seguenti strutture devono rimanere invariate rispetto al primo esonero:

**Richiesta Client:**
```c
struct request {
    char type;      // 't'=temperatura, 'h'=umidità, 'w'=vento, 'p'=pressione
    char city[64];  // nome città (null-terminated)
};
```

**Risposta Server:**
```c
struct response {
    unsigned int status;  // 0=successo, 1=città non trovata, 2=richiesta invalida
    char type;            // eco del tipo richiesto
    float value;          // dato meteo generato
};
```

### Formati di Output

I formati di output rimangono **identici** al primo esonero:

**Successo (status=0):**
- Temperatura: `"NomeCittà: Temperatura = XX.X°C"`
- Umidità: `"NomeCittà: Umidità = XX.X%"`
- Vento: `"NomeCittà: Vento = XX.X km/h"`
- Pressione: `"NomeCittà: Pressione = XXXX.X hPa"`

**Errori:**
- Status 1: "Città non disponibile"
- Status 2: "Richiesta non valida"
- **Nuovo**: Timeout: "Timeout: nessuna risposta dal server" (specifico per UDP)

## Interfaccia Client

**Sintassi:**
```
./client-project [-s server] [-p port] -r "type city"
```

**Parametri:**
- `-s server`: indirizzo server (default: 127.0.0.1)
- `-p port`: porta server (default: 56700)
- `-r request`: richiesta meteo obbligatoria

**Flusso operativo:**
1. Analizza argomenti da linea di comando
2. Crea socket UDP (SOCK_DGRAM)
3. Invia richiesta al server con `sendto()`
4. Attende risposta con timeout (3 secondi)
5. In caso di timeout, ritrasmette la richiesta (massimo 3 tentativi totali)
6. Riceve risposta con `recvfrom()`
7. Visualizza risultato formattato
8. Chiude socket

**Gestione errori:**
- Dopo 3 tentativi senza risposta, il client deve terminare con un messaggio di errore
- Implementare `setsockopt()` con `SO_RCVTIMEO` per il timeout di ricezione

## Interfaccia Server

**Sintassi:**
```
./server-project [-p port]
```

**Parametri:**
- `-p port`: porta ascolto (default: 56700)

**Comportamento:**
Il server rimane attivo continuamente in ascolto sulla porta specificata. Per ogni datagramma ricevuto:
1. Riceve richiesta con `recvfrom()` (acquisendo indirizzo client)
2. Valida il tipo di richiesta e il nome della città
3. Genera il valore meteo con le funzioni `get_*()`
4. Invia risposta al client con `sendto()` usando l'indirizzo acquisito
5. Continua in ascolto per nuove richieste

**Note:**
- Non c'è fase di "connessione" o "accettazione" come in TCP
- Il server può gestire richieste da client multipli in modo concorrente
- Ogni richiesta è indipendente e stateless

## Funzioni di Generazione Dati

Le funzioni di generazione dati rimangono **identiche** al primo esonero. Il server implementa quattro funzioni che generano valori casuali:
- `get_temperature()`: temperatura casuale tra -10.0 e 40.0 °C
- `get_humidity()`: umidità casuale tra 20.0 e 100.0%
- `get_wind()`: velocità vento casuale tra 0.0 e 100.0 km/h
- `get_pressure()`: pressione casuale tra 950.0 e 1050.0 hPa

## Città Supportate

Le città supportate rimangono **identiche** al primo esonero. Il server deve riconoscere almeno 10 città italiane (confronto case-insensitive):
- Bari
- Roma
- Milano
- Napoli
- Torino
- Palermo
- Genova
- Bologna
- Firenze
- Venezia

## Requisiti Tecnici

### 1. Organizzazione del Codice
- **File header `protocol.h`**: riutilizzare lo stesso file del primo esonero, che contiene le definizioni delle strutture dati (`struct request`, `struct response`), i prototipi delle funzioni e le costanti condivise tra client e server
- **File sorgente client**: `client.c` nell'apposita directory - modificare solo le parti relative ai socket
- **File sorgente server**: `server.c` nell'apposita directory - modificare solo le parti relative ai socket

### 2. Portabilità Multi-Piattaforma
Il codice deve compilare ed eseguire correttamente su:
- Windows (utilizzare Winsock2)
- Linux
- macOS

Utilizzare direttive di preprocessore (`#ifdef _WIN32`) per gestire le differenze tra piattaforme.

### 3. Gestione Memoria e Sicurezza
- Nessun buffer overflow
- Nessun memory leak
- Validazione corretta degli input utente
- Gestione appropriata degli errori di sistema

### 4. Socket UDP
- Utilizzare `SOCK_DGRAM` per creare socket UDP
- Implementare timeout sul socket di ricezione del client
- Gestire correttamente gli indirizzi con `struct sockaddr_in`

### 5. Timeout e Ritrasmissione
- Timeout di ricezione: 3 secondi
- Numero massimo di ritrasmissioni: 3 tentativi totali
- Messaggio di errore appropriato se tutti i tentativi falliscono

### 6. Dimensioni Datagrammi
- Dimensione massima datagramma: 512 byte
- Verificare che le strutture dati non eccedano questo limite

### 7. Compatibilità Eclipse CDT
Il progetto deve essere compatibile con Eclipse CDT e includere i file di configurazione necessari (.project, .cproject).

## Consegna

- **Scadenza:** 28 novembre 2025, ore 23:59:59
- **Formato:** link repository GitHub pubblico
- **Note:**
  - Una sola consegna per coppia di studenti


---

**Buon lavoro!**
