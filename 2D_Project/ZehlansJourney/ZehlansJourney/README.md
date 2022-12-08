# Progetto 1:

Classi e suddivisioni file:
- *Lib.h* contiene diversi "#include" e l'uso di namespace per facilitare e semplificare lo stile di programmazione.
- *Values.h* contiene diverse definizioni di variabili che permettono di manipolare le geometrie in modo semplificato modificando un solo valore.
- *Shape.h* definisce un tamplate di base per ciascuna delle entità in gioco all'interno del progetto: ogni Shape ha i suoi valori che vengono definiti man mano da classi e funzioni.
- *ShaderMaker.h* e *ShaderMaker.cpp* permettono di interagire con gli shader e di poter programmare la pipeline.
- *Hermitte.h* e *Hermitte.cpp* definiscono le funzioni di base del polinomio interpolante di Hermitte, defininedo una sola funzione richiamabile quandolo si ritiene opportuno.
- *EventHandler.h* e *EventHandler.cpp* gesticono una serie di eventi a run-time, tutti dichiarati e spiegati nel file header.
- *CreateGeometries.h* e *CreateGeometries.cpp* sono i file che si occupano di dichiarare e definire le funzione che definiscono i vertici di tutte le entità in gioco.
- *Elements.h* e *Elements.cpp* sono rispettivamente interfaccia e classe usate per incapsulare e gestire le entità in gioco. Simula una collezione di entità.
- *main.cpp* si occupa di:
    - Istanziare la classe di interazione con gli shader.
    - Inizializzare il Vertex Array Object.
    - Popolare il Vertex Array Object.
    - Disegnare le geometrie che saranno poi mandate in pasto al *vertexShader* e al *fragmentShader*.
    - Richiamare delle funzioni evento che saranno gestite in altre classi.
    - Definire parametri di base per poter lavorare con OpenGL.

Interazione con l'applicativo:
- *Tasto sx del mouse*: mostra geometrie di base della nave e hit box del proiettile e del blob.
- *spazio*: spara un proiettile.
- *a*: sposta la nave a sinistra.
- *d*: sposta la nave a destra.
- *z*: panic button che fa crashare l'applicativo nel caso di problemi di lag.