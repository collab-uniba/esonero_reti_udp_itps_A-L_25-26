# Esonero di Laboratorio UDP - Reti di Calcolatori (ITPS A-L) 2025-26 :santa: :christmas_tree: :gift:

## Obiettivo Generale
**Migrare** l'applicazione client/server del servizio meteo dal protocollo **TCP** (realizzata nel primo esonero) al protocollo **UDP** (User Datagram Protocol).

L'obiettivo è modificare il codice già scritto per l'assegnazione TCP in modo da usare UDP come protocollo di trasporto, mantenendo invariato il protocollo applicativo (strutture dati, formati, funzionalità).

## Cosa Cambia nella Migrazione da TCP a UDP

### Modifiche da Apportare al Codice

Nel passaggio da TCP a UDP, dovrete modificare **solo il livello di trasporto**, lasciando invariato tutto il resto:

**DA MODIFICARE:**
- Tipo di socket
- Chiamate di sistema
- Output

**NON MODIFICARE:**
- Protocollo applicativo: strutture `struct request` e `struct response`
- Interfaccia a linea di comando (opzioni `-s`, `-p`, `-r`)
- Logica di business: parsing richieste, validazione città, generazione dati meteo
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

> [!NOTE]
> **Valori validi per il campo `type`:**
> - `'t'` = temperatura
> - `'h'` = umidità
> - `'w'` = vento
> - `'p'` = pressione
>
> **Qualsiasi altro carattere** è considerato **richiesta invalida** e deve generare `status=2` nella risposta del server.

**Risposta Server:**
```c
struct response {
    unsigned int status;  // 0=successo, 1=città non trovata, 2=richiesta invalida
    char type;            // eco del tipo richiesto
    float value;          // dato meteo generato
};
```

### Network Byte Order

> [!WARNING]
> Durante la serializzazione e deserializzazione delle strutture dati, è necessario gestire correttamente il network byte order per i campi numerici:
> - **`unsigned int status`**: usare `htonl()` prima dell'invio e `ntohl()` dopo la ricezione
> - **`float value`**: convertire in formato network byte order usando la tecnica mostrata a lezione (conversione `float` → `uint32_t` → `htonl/ntohl` → `float`)
> - **`char type` e `char city[]`**: essendo campi a singolo byte, non richiedono conversione
>
> **IMPORTANTE - Serializzazione manuale obbligatoria:**
> - **NON inviate le struct direttamente** con `sendto(&request, sizeof(request), ...)` o `sendto(&response, sizeof(response), ...)`
> - Il compilatore può aggiungere **padding** tra i campi della struct, causando problemi di portabilità
> - **Dovete creare un buffer separato** e copiare manualmente i campi uno per uno, come mostrato a lezione
> - Esempio: creare un `char buffer[...]` e copiare i campi in sequenza dopo averli convertiti in network byte order

Esempio conversione float e serializzazione (come mostrato a lezione):
```c
// Invio response: serializzazione in buffer separato
char buffer[sizeof(uint32_t) + sizeof(char) + sizeof(float)];
int offset = 0;

// Serializza status (con network byte order)
uint32_t net_status = htonl(response.status);
memcpy(buffer + offset, &net_status, sizeof(uint32_t));
offset += sizeof(uint32_t);

// Serializza type (1 byte, no conversione)
memcpy(buffer + offset, &response.type, sizeof(char));
offset += sizeof(char);

// Serializza value (float con network byte order)
uint32_t temp;
memcpy(&temp, &response.value, sizeof(float));
temp = htonl(temp);
memcpy(buffer + offset, &temp, sizeof(float));
offset += sizeof(float);

// Invio del buffer
sendto(sock, buffer, offset, 0, ...);

// Ricezione e deserializzazione analoga
```

### Formati di Output

> [!WARNING]
> - Il formato dell'output deve essere **ESATTAMENTE** come specificato di seguito
> - **Lingua italiana obbligatoria** - NON tradurre in inglese o altre lingue
> - **NO caratteri extra** - seguire esattamente gli esempi forniti
> - Gli **spazi e la punteggiatura** devono corrispondere esattamente agli esempi

Il client deve stampare l'indirizzo IP del server (risultato della risoluzione DNS) seguito dal messaggio appropriato:

**Successo (`status=0`):**
- Temperatura: `"Ricevuto risultato dal server <nomeserver> (ip <IP>). NomeCittà: Temperatura = XX.X°C"`
- Umidità: `"Ricevuto risultato dal server <nomeserver> (ip <IP>). NomeCittà: Umidità = XX.X%"` (sono accettate entrambe le grafie: "Umidità" e "Umidita'")
- Vento: `"Ricevuto risultato dal server <nomeserver> (ip <IP>). NomeCittà: Vento = XX.X km/h"`
- Pressione: `"Ricevuto risultato dal server <nomeserver> (ip <IP>). NomeCittà: Pressione = XXXX.X hPa"`

**Note sul formato:**
- I valori devono essere formattati con **una cifra decimale** (es. 23.5, non 23.50 o 23)
- Il nome della città deve mantenere la capitalizzazione corretta (prima lettera maiuscola)

**Errori:**
- `status 1`: `"Ricevuto risultato dal server <nomeserver> (ip <IP>). Città non disponibile"`
- `status 2`: `"Ricevuto risultato dal server <nomeserver> (ip <IP>). Richiesta non valida"`

> [!NOTE]
> **Risoluzione DNS del server (client):**
> - I valori `<nomeserver>` e `<IP>` dono essere ottenuti tramite le funzioni di **DNS lookup**
> - Se l'utente specifica `-s localhost`, il client risolve `localhost` → `127.0.0.1` e poi fa reverse lookup `127.0.0.1` → `localhost`
> - Se l'utente specifica `-s 127.0.0.1`, il client fa reverse lookup `127.0.0.1` → `localhost`

### Esempi di Output

**Richiesta con successo (usando default `localhost`):**
```bash
$ ./client-project -r "t bari"
Ricevuto risultato dal server localhost (ip 127.0.0.1). Bari: Temperatura = 22.5°C
```

**Richiesta esplicita con nome host:**
```bash
$ ./client-project -s localhost -r "h napoli"
Ricevuto risultato dal server localhost (ip 127.0.0.1). Napoli: Umidità = 65.3%
```

**Richiesta con indirizzo IP diretto:**
```bash
$ ./client-project -s 127.0.0.1 -r "w milano"
Ricevuto risultato dal server localhost (ip 127.0.0.1). Milano: Vento = 12.8 km/h
```

**Città non disponibile:**
```bash
$ ./client-project -r "h parigi"
Ricevuto risultato dal server localhost (ip 127.0.0.1). Città non disponibile
```

**Richiesta non valida:**
```bash
$ ./client-project -r "x roma"
Ricevuto risultato dal server localhost (ip 127.0.0.1). Richiesta non valida
```

## Interfaccia Client

**Sintassi:**
```
./client-project [-s server] [-p port] -r "type city"
```

**Parametri:**
- `-s server`: server può indicare o l'hostname del server oppure il suo indirizzo IP (default: `localhost`)
- `-p port`: porta server (default: `56700`)
- `-r request`: richiesta meteo obbligatoria (formato: `"type city"`)

**Note di parsing richiesta:**
- La stringa della richiesta può contenere spazi multipli, ma **non ammette caratteri di tabulazione** (`\t`)
- Per il parsing della richiesta: **il primo token (prima dello spazio) deve essere un singolo carattere** che specifica il `type`, tutto il resto (dopo lo spazio) è da considerarsi come `city`
- **Se il primo token contiene più di un carattere** (es. `-r "temp bari"`), il client **deve segnalare un errore e NON inviare** la richiesta
- Sono accettati entrambi i casi di spelling dei **caratteri accentati** (e.g., Umidita' e Umidità sono entrambi validi)
- Esempio valido: `-r "p Reggio Calabria"` → type='p', city="Reggio Calabria"
- Esempio invalido: `-r "temp roma"` → errore lato client (primo token non è un singolo carattere)

**Flusso operativo:**
1. Analizza argomenti da linea di comando
2. Crea socket UDP
3. Invia richiesta al server
4. Riceve risposta
5. Visualizza risultato formattato
6. Chiude socket

## Interfaccia Server

**Sintassi:**
```
./server-project [-p port]
```

**Parametri:**
- `-p port`: porta ascolto (default: `56700`)

**Comportamento:**
Il server rimane attivo continuamente in ascolto sulla porta specificata. Per ogni datagramma ricevuto:
1. Riceve richiesta (acquisendo indirizzo client)
2. Valida il tipo di richiesta e il nome della città
3. Genera il valore meteo con le funzioni `get_*()`
4. Invia risposta al client, usando l'indirizzo acquisito
5. Continua in ascolto per nuove richieste

**Logging delle Richieste:**

Il server deve stampare a console un log per ogni richiesta ricevuta, includendo sia il nome host che l'indirizzo IP del client.

**Esempio di log:**
```
Richiesta ricevuta da localhost (ip 127.0.0.1): type='t', city='Roma'
```

> [!NOTE]
> **Risoluzione DNS:**
> - Così come il client, anche il server deve usare opportunamente le funzioni di **DNS lookup** a partire da nome host o indirizzo.

**Note:**
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
- **Gli spazi multipli consecutivi NON vengono normalizzati**: la stringa viene trattata così com'è (es. "San  Marino" con due spazi probabilmente non verrà riconosciuta e darà `status=1`)
- **Caratteri speciali** (es. @, #, $, %, ecc.) e **caratteri di tabulazione** non sono ammessi e devono causare un errore di validazione (`status=2`)
- Il confronto rimane case-insensitive (es. "bari", "BARI", "Bari" sono tutti validi)

## Requisiti Tecnici

### 1. Organizzazione del Codice
- Non modificate nomi dei file e delle cartelle fornite nel progetto template
- Potete aggiungere nuovi file .h e .c se necessario

### 2. Portabilità Multi-Piattaforma
Il codice deve compilare ed eseguire correttamente su:
- Windows
- Linux
- macOS

### 3. Network Byte Order
- Durante la serializzazione e deserializzazione delle strutture dati, è necessario gestire opportunamente il network byte order in base al tipo del campo

### 4. Risoluzione Nomi DNS
Il codice deve supportare **sia nomi simbolici** (es. `localhost`) che **indirizzi IP** (es. `127.0.0.1`) come parametro `-s` del client. Vedere le note nelle sezioni "Formati di Output" e "Logging delle Richieste" per i dettagli sulla risoluzione e reverse lookup DNS

### 5. Gestione errori
- Gestione appropriata degli errori
- Nessun crash
- Validazione corretta degli input utente
- **Validazione lunghezza nome città (lato client)**: se il nome della città supera 63 caratteri (64 incluso il null-terminator), il client **deve segnalare un errore all'utente e NON inviare** la richiesta al server

### 6. Compatibilità Eclipse CDT
Il progetto deve essere compatibile con Eclipse CDT e includere i file di configurazione necessari (`.project`, `.cproject`).

## Note di Validazione

### Client

> [!NOTE]
> **Validazione del campo `city`:**
> - Il campo `city` ha una lunghezza massima di 64 caratteri, incluso il terminatore `\0`
> - La verifica della lunghezza è a carico del client prima di inviare la richiesta
> - Il campo `city` può contenere spazi multipli, ma non ammette caratteri di tabulazione (`\t`)
>
> **Validazione della richiesta da linea di comando:**
> - La stringa della richiesta (`-r`) può contenere spazi multipli, ma non ammette caratteri di tabulazione (`\t`)

### Server

> [!NOTE]
> **Caratteristiche del protocollo UDP:**
> - Non c'è fase di "connessione" o "accettazione" come in TCP
> - Ogni richiesta è indipendente e stateless
> - Il server deve acquisire l'indirizzo del client da ogni datagramma ricevuto
>
> **Ciclo di vita del server:**
> - Il server non termina autonomamente
> - Rimane in esecuzione indefinitamente
> - Può essere interrotto solo forzatamente tramite **Ctrl+C** (SIGINT)

## Consegna

- **Scadenza**: 21 dicembre 2025, entro le ore 23.59.59
- **Form di prenotazione / consegna**: [link](https://forms.gle/P4kWH3M3zjXjsWWP7)
- **Formato**: Link a repository GitHub accessibile pubblicamente
- **Note**:
   - Una sola consegna per coppia.
   - Le coppie non si possono modificare, al limite i due partecipanti di una coppia possono decidere di effettuare l'esonero UDP da soli.
   - Non è necessario ricompilare il form di consegna dopo aggiornamenti al repository
   - La pagina dei risultati si aggiorna ogni ora, scaricando l'ultima versione di ciascun progetto fino alla scadenza.

_Ver. 1.0.0_
