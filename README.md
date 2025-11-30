# Esonero UDP - Corso di Reti di Calcolatori - ITPS A-L 2025-26

Repository per l'assegnazione della prima prova di esonero: client-server UDP.


## Descrizione

Questo repository contiene la struttura base per lo sviluppo di un'applicazione client-server UDP in linguaggio C, utilizzando la libreria standard delle socket. Il codice è progettato per essere portabile su sistemi operativi **Windows**, **Linux** e **macOS**.

## Struttura del Repository

Il repository è organizzato in due progetti Eclipse CDT separati:

```
.
├── client-project/         # Progetto Eclipse per il client
│   ├── .project            # Configurazione progetto Eclipse
│   ├── .cproject           # Configurazione Eclipse CDT
│   └── src/
│       ├── main.c          # File principale del client
│       └── protocol.h      # Header con definizioni e prototipi
│
└── server-project/         # Progetto Eclipse per il server
    ├── .project            # Configurazione progetto Eclipse
    ├── .cproject           # Configurazione Eclipse CDT
    └── src/
        ├── main.c          # File principale del server
        └── protocol.h      # Header con definizioni e prototipi
```

**⚠️ IMPORTANTE - Struttura del Progetto:**
- **NON modificare** i nomi dei file esistenti (`main.c`, `protocol.h`)
- **NON modificare** i nomi delle cartelle esistenti (`client-project`, `server-project`, `src`)
- **NON eliminare o modificare a
  mano** i file di configurazione Eclipse (`.project`, `.cproject`)
- È possibile **aggiungere nuovi file** se necessario (es. file `.c` o `.h` aggiuntivi)

## Come Utilizzare il Repository

### 1. Creare la propria copia del repository

1. Cliccare su "Use this template" in alto a destra su GitHub
2. Creare un nuovo repository personale
   - Rinominate il repository sostituendo la parola _template_ con il vostro _cognome_ seguito dalla _matricola_ (`template_esonero_udp` -> `cognome_12345_esonero_udp`)
   - Se l'esonero è svolto in coppia, il nome del repository sarà `cognome1_12345_cognome2_6789_esonero_udp`
4. Clonare il repository sul proprio computer:
   ```bash
   git clone <url-del-tuo-repository>
   ```

### 2. Importare i progetti in Eclipse

1. Aprire **Eclipse CDT**
2. Selezionare `File → Import → General → Existing Projects into Workspace`
3. Selezionare la directory `client-project`
4. Ripetere i passi 2-3 per `server-project`

### 3. Configurare il progetto in Eclipse

Dopo aver importato i progetti, è necessario verificare e configurare le impostazioni del compilatore:

#### Verificare il Toolchain
Per ciascun progetto (client e server):
1. Click destro sul progetto → `Properties`
2. Andare in `C/C++ Build → Tool Chain Editor`
3. Verificare che il **Current toolchain** sia corretto per il proprio sistema operativo:
   - **Linux**: GCC
   - **macOS**: GCC
   - **Windows**: MinGW GCC (Assicurarsi di avere già installato MinGW o MinGW-w64)

#### Solo per Windows: Aggiungere libreria Winsock
Per compilare su Windows, è necessario linkare la libreria Winsock:
1. Click destro sul progetto → `Properties`
2. Andare in `C/C++ Build → Settings`
3. Selezionare `MinGW C Linker → Libraries`
4. In **Libraries (-l)**, cliccare su `Add` e inserire: `wsock32`
5. Applicare le modifiche e cliccare `OK`


## File Principali

### protocol.h
Contiene:
- **Costanti condivise**: numero di porta del server, dimensione del buffer, ecc.
- **Prototipi delle funzioni**: inserire qui le firme di tutte le funzioni implementate

Esempio:
```c
#define SERVER_PORT 27015
#define BUFFER_SIZE 512

// Prototipo funzione esempio
int connect_to_server(const char* server_address);
```

### main.c (Client e Server)
Contiene:
- **Codice boilerplate** per la portabilità cross-platform
- **Inizializzazione Winsock** su Windows
- **Sezioni TODO** dove implementare la logica dell'applicazione

## Specifiche dell'Assegnazione

[Protocollo applicativo e istruzioni per la consegna](Assegnazione.md)

## Lavorare con Git

### Workflow Consigliato

Per mantenere il repository sincronizzato e lavorare in modo efficace, si consiglia di seguire questo workflow:

#### 1. Prima di Iniziare a Lavorare

```bash
git pull
```
Scarica le ultime modifiche dal repository remoto. È importante farlo **prima** di iniziare a lavorare per evitare conflitti.

#### 2. Verificare lo Stato del Repository

```bash
git status
```
Mostra i file modificati, aggiunti o eliminati nel working directory.

#### 3. Aggiungere i File Modificati

```bash
git add <nome-file>
# oppure per aggiungere tutti i file modificati
git add .
```

#### 4. Creare un Commit

```bash
git commit -m "Descrizione chiara delle modifiche"
```

#### 5. Inviare le Modifiche al Repository Remoto

```bash
git push
```

#### 6. Rimanere sincronizzati se lavorare in coppia

```bash
git pull
```

## Licenza

Vedere il file [LICENSE](LICENSE) per i dettagli.
