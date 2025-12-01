# Esonero di Laboratorio UDP - Reti di Calcolatori (ITPS A-L) 2025-26

## Obiettivo Generale
**Migrare** l'applicazione client/server del servizio meteo dal protocollo **TCP** (realizzata nel primo esonero) al protocollo **UDP** (User Datagram Protocol).

L'obiettivo è modificare il codice già scritto per l'assegnazione TCP in modo da usare UDP come protocollo di trasporto, mantenendo invariato il protocollo applicativo (strutture dati, formati, funzionalità).

## Cosa Cambia nella Migrazione da TCP a UDP

### Modifiche da Apportare al Codice

Nel passaggio da TCP a UDP, dovrete modificare **solo il livello di trasporto**, lasciando invariato tutto il resto:

**DA MODIFICARE:**
- Tipo di socket
- Chiamate di sistema

**NON MODIFICARE:**
- Protocollo applicativo: strutture `struct request` e `struct response`
- Interfaccia a linea di comando (opzioni `-s`, `-p`, `-r`)
- Logica di business: parsing richieste, validazione città, generazione dati meteo
- Formati di output
- Funzioni `get_temperature()`, `get_humidity()`, `get_wind()`, `get_pressure()`

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

**Note di validazione:**
- Il campo `city` ha una lunghezza massima di 64 caratteri,  incluso il terminatore `\0` (la verifica è a carico del client prima di inviare la richiesta)
- Il campo `city` può contenere spazi multipli, ma non ammette caratteri di tabulazione (`\t`)

**Risposta Server:**
```c
struct response {
    unsigned int status;  // 0=successo, 1=città non trovata, 2=richiesta invalida
    char type;            // eco del tipo richiesto
    float value;          // dato meteo generato
};
```

### Network Byte Order

**IMPORTANTE**: Durante la serializzazione e deserializzazione delle strutture dati, è necessario gestire correttamente il network byte order per i campi numerici:

- **`unsigned int status`**: usare `htonl()` prima dell'invio e `ntohl()` dopo la ricezione
- **`float value`**: convertire in formato network byte order usando la tecnica mostrata a lezione (conversione `float` → `uint32_t` → `htonl/ntohl` → `float`)
- **`char type` e `char city[]`**: essendo campi a singolo byte, non richiedono conversione

Esempio conversione float (come mostrato a lezione):
```c
// Invio: float -> network byte order
uint32_t temp;
memcpy(&temp, &value, sizeof(float));
temp = htonl(temp);
memcpy(&response.value, &temp, sizeof(float));

// Ricezione: network byte order -> float
uint32_t temp;
memcpy(&temp, &response.value, sizeof(float));
temp = ntohl(temp);
memcpy(&value, &temp, sizeof(float));
```

### Formati di Output

I formati di output rimangono **identici** al primo esonero.

**IMPORTANTE**: Il formato dell'output deve essere **ESATTAMENTE** come specificato. Messaggio in italiano, senza caratteri extra.

**Successo (`status=0`):**
- Temperatura: `"NomeCittà: Temperatura = XX.X°C"`
- Umidità: `"NomeCittà: Umidità = XX.X%"` (sono accettate entrambe le grafie: "Umidità" e "Umidita'")
- Vento: `"NomeCittà: Vento = XX.X km/h"`
- Pressione: `"NomeCittà: Pressione = XXXX.X hPa"`

**Note sul formato:**
- I valori devono essere formattati con **una cifra decimale** (es. 23.5, non 23.50 o 23)
- Il nome della città deve mantenere la capitalizzazione corretta (prima lettera maiuscola)

**Errori:**
- `status 1`: "Città non disponibile"
- `status 2`: "Richiesta non valida"

## Interfaccia Client

**Sintassi:**
```
./client-project [-s server] [-p port] -r "type city"
```

**Parametri:**
- `-s server`: server può indicare o l'hostname del server oppure il suo indirizzo IP (default: localhost) 
- `-p port`: porta server (default: 56700)
- `-r request`: richiesta meteo obbligatoria (formato: `"type city"`)

**Note di validazione:**
- La stringa della richiesta può contenere spazi multipli, ma **non ammette caratteri di tabulazione** (`\t`)
- Per il parsing della richiesta: il primo carattere specifica il `type`, tutto il resto (dopo lo spazio) è da considerarsi come `city`
- Esempio: `-r "p Reggio Calabria"` → type='p', city="Reggio Calabria"

**Flusso operativo:**
1. Analizza argomenti da linea di comando
2. Crea socket UDP
3. Invia richiesta al server
4. Riceve risposta
5. Visualizza risultato formattato
6. Chiude socket

**Formato di Output:**

Il client deve visualizzare i risultati con il seguente formato:
```
Ricevuto risultato dal server [indirizzo_ip]. [messaggio]
```

Dove `[messaggio]` varia in base allo status della risposta (vedi sezione "Formati di Output" sopra).

**Esempio di output completo:**
```
Ricevuto risultato dal server 127.0.0.1. Roma: Temperatura = 23.5°C
```

## Interfaccia Server

**Sintassi:**
```
./server-project [-p port]
```

**Parametri:**
- `-p port`: porta ascolto (default: 56700)

**Comportamento:**
Il server rimane attivo continuamente in ascolto sulla porta specificata. Per ogni datagramma ricevuto:
1. Riceve richiesta (acquisendo indirizzo client)
2. Valida il tipo di richiesta e il nome della città
3. Genera il valore meteo con le funzioni `get_*()`
4. Invia risposta al client, usando l'indirizzo acquisito
5. Continua in ascolto per nuove richieste

**Logging delle Richieste:**

Il server deve stampare a console un log per ogni richiesta ricevuta, includendo l'indirizzo IP del client.

**Esempio di log:**
```
Richiesta ricevuta da 127.0.0.1: type='t', city='Roma'
```

**Note:**
- Non c'è fase di "connessione" o "accettazione" come in TCP
- Ogni richiesta è indipendente e stateless
- Il server non termina autonomamente, rimane in esecuzione indefinitamente e può essere interrotto solo forzatamente tramite **Ctrl+C** (SIGINT)

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

**Note sulla validazione nomi città:**
- Le città possono contenere spazi singoli (es. "San Marino")
- Le città con **spazi multipli consecutivi** sono gestite normalizzando gli spazi (es. "San  Marino" → "San Marino")
- **Caratteri speciali** (es. @, #, $, %, ecc.) e **caratteri di tabulazione** non sono ammessi e devono causare un errore di validazione
- Il confronto rimane case-insensitive (es. "bari", "BARI", "Bari" sono tutti validi)

## Requisiti Tecnici

### 1. Organizzazione del Codice
- **File header** `protocol.h`: riutilizzare lo stesso file del primo esonero, che contiene le definizioni delle strutture dati (`struct request`, `struct response`), i prototipi delle funzioni e le costanti condivise tra client e server
- **File sorgente client**: `client.c` nell'apposita directory - modificare solo le parti relative ai socket
- **File sorgente server**: `server.c` nell'apposita directory - modificare solo le parti relative ai socket

### 2. Portabilità Multi-Piattaforma
Il codice deve compilare ed eseguire correttamente su:
- Windows
- Linux
- macOS

### 3.Network Byte Order
- Durante la serializzazione e deserializzazione delle strutture dati, è necessario gestire opportunamento il network byte order in base al tipo del campo

### 4. Risoluzione Nomi DNS
- Il client deve utilizzare `localhost` come indirizzo predefinito invece di `127.0.0.1`
- Questo requisito permette di sfruttare le funzioni di risoluzione dei nomi DNS del sistema operativo
- Il codice deve supportare sia nomi simbolici (es. `localhost`, `example.com`) che indirizzi IP  (es. `127.0.0.1`, `192.168.1.1`)

### 5. Gestione Memoria e Sicurezza
- Nessun buffer overflow
- Nessun memory leak
- Validazione corretta degli input utente
- Gestione appropriata degli errori di sistema
- **Validazione lunghezza nome città (lato client)**: se il nome della città supera 64 caratteri, il client deve troncare la stringa a 63 caratteri (lasciando spazio per il null-terminator) oppure segnalare un errore all'utente prima dell'invio

### 6. Compatibilità Eclipse CDT
Il progetto deve essere compatibile con Eclipse CDT e includere i file di configurazione necessari (`.project`, `.cproject`).

## Consegna

- **Scadenza**: xx dicembre 2025, entro h. 23.59.59
- **Form di prenotazione / consegna**: [link](https://forms.gle/P4kWH3M3zjXjsWWP7)
- **Formato**: Link a repository GitHub accessibile pubblicamente
- **Note**:
   - Una sola consegna per coppia. Le coppie non si possono modificare. Al limite i due partecipanti di una coppia possono decidere di effettuare l'esonero UDP da soli.
 
_Ver. 1.0.0_
