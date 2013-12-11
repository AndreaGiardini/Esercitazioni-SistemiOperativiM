with Ada.Text_IO, Ada.Integer_Text_IO, Ada.Numerics.Discrete_Random;
use Ada.Text_IO, Ada.Integer_Text_IO;

procedure ponte is
        -- Costanti
        id_MIN : constant integer := 1; --id minimo
        id_MAX : constant integer := 25; --id massimo
        
        id_NORD : constant integer := 0; --direzione Nord
        id_SUD : constant integer := 1; --direzione Sud

        id_AUTO : constant integer := 0; --veicolo Auto
        id_CAMION : constant integer := 1; --veicolo Camion        
        
        timeThroughBridge : constant duration := 1.50;

        -- Definizioni
        type cliente_ID is range id_MIN..id_MAX;
        type direction is (Nord, Sud);
        type vehicle is (Automobile, Camion);
        
        autoSospese: integer;

        -- Direzione casuale
        subtype Direction_ID is integer range id_NORD..id_SUD;
        package DirectionRandomizator is new Ada.Numerics.Discrete_Random (Direction_ID);
        use DirectionRandomizator;
        G : Generator;        
        D : Direction_ID;

        -- Veicolo casuale
        subtype Vehicle_ID is integer range id_AUTO..id_CAMION;
        package VehicleRandomizator is new Ada.Numerics.Discrete_Random (Vehicle_ID);
        use VehicleRandomizator;
        V : Vehicle_ID;

        -- DEFINIZIONE GESTORE DEL PONTE
        task type server is
                -- Metodi del gestore del ponte
                entry entraAutoNORD(ID: in cliente_ID);
                entry entraCamionNORD(ID: in cliente_ID);
                entry entraAutoSUD(ID: in cliente_ID);
                entry entraCamionSUD(ID: in cliente_ID);
                
                entry esceNORD(ID: in cliente_ID; VTYPE: vehicle);
                entry esceSUD(ID: in cliente_ID; VTYPE: vehicle);
        end server;

        -- CORPO DEL GESTORE DEL PONTE
        task body server is
                -- Variabili di stato del ponte
                MAX : constant integer := 3; --capacità ponte

                sulPonteDirNord: integer; --veicoli sul ponte direzione nord
                sulPonteDirSud: integer; --veicoli sul ponte direzione sud
        begin
                -- Inizializzazione variabili di stato del ponte
          sulPonteDirNord :=0;
                sulPonteDirSud :=0;
                -- variabile per determinare se ci sono auto in attesa di entrare (hanno la priorità sui camion)
                autoSospese :=0;

          --Gestione richieste:
          loop
      select
                                -- accettazione richieste di accesso dir nord
                                -- priorità alle auto
                                when autoSospese > 0 and then sulPonteDirNord < MAX and then sulPonteDirSud = 0 =>
          accept entraAutoNORD(ID: cliente_ID) do 
            sulPonteDirNord:=sulPonteDirNord+1;
            Put_Line("AUTOMOBILE" & cliente_ID'Image (ID) & " dir NORD entrato!");
          end entraAutoNORD;
                                -- i camion dopo le auto
                                or when sulPonteDirNord < MAX and then sulPonteDirSud = 0 =>
                                        accept entraCamionNORD(ID: cliente_ID) do 
            sulPonteDirNord:=sulPonteDirNord+1;
            Put_Line("CAMION" & cliente_ID'Image (ID) & " dir NORD entrato!");
          end entraCamionNORD;
                                -- accettazione richieste di accesso dir sud
                                -- priorità alle auto
                                or when autoSospese > 0 and then sulPonteDirSud < MAX and then sulPonteDirNord = 0 =>
          accept entraAutoSUD(ID: cliente_ID) do 
            sulPonteDirSud:=sulPonteDirSud+1;
            Put_Line("AUTOMOBILE" & cliente_ID'Image (ID) & " dir SUD entrato!");
          end entraAutoSUD;
                                -- i camion dopo le auto
                                or when sulPonteDirSud < MAX and then sulPonteDirNord = 0 =>
                                        accept entraCamionSUD(ID: cliente_ID) do 
            sulPonteDirSud:=sulPonteDirSud+1;
            Put_Line("CAMION" & cliente_ID'Image (ID) & " dir SUD entrato!");
          end entraCamionSUD;
                                -- uscita dir nord
                                or accept esceNORD(ID: cliente_ID; VTYPE: vehicle) do 
                                    sulPonteDirNord:=sulPonteDirNord-1;
                                    Put_Line(vehicle'Image (VTYPE) & cliente_ID'Image (ID) & " dir NORD uscito!");
                                  end esceNORD;
                                -- uscita dir sud
                                or accept esceSUD(ID: cliente_ID; VTYPE: vehicle) do 
                        sulPonteDirSud:=sulPonteDirSud-1;
                        Put_Line(vehicle'Image (VTYPE) & cliente_ID'Image (ID) & " dir SUD uscito!");
                      end esceSUD;
                                -- termina i task appesi
                                or terminate;
      end select;
          end loop;
        end server;

        -- Inizializzazione ponte
        S: server; 
        
        -- DEFINIZIONE DEL TASK CLIENTE
        task type cliente (ID: cliente_ID; DIR: direction; VTYPE: vehicle);

        task body cliente is
        begin
                Put_Line(vehicle'Image (VTYPE)& " dir "& direction'Image (DIR) &" iniziato! (id:"& cliente_ID'Image (ID)&")");
                if DIR=NORD
                then
                        if VTYPE=AUTOMOBILE
                        then
                                -- segnala che l'auto è in sospeso fintanto che non entra
                                autoSospese:=autoSospese+1;
                                S.entraAutoNORD(ID);
                                -- quando l'auto entra non è più considerata in sospeso
                                autoSospese:=autoSospese-1;
                        else
                                S.entraCamionNORD(ID);
                        end if;
                        delay timeThroughBridge;
                        S.esceNORD(ID, VTYPE);
                else
                        if VTYPE=AUTOMOBILE
                        then
                                -- segnala che l'auto è in sospeso fintanto che non entra
                                autoSospese:=autoSospese+1;
                                S.entraAutoSUD(ID);
                                -- quando l'auto entra non è più considerata in sospeso
                                autoSospese:=autoSospese-1;
                        else
                                S.entraCamionSUD(ID);
                        end if;
                        delay timeThroughBridge;
                        S.esceSUD(ID, VTYPE);
                end if;
        end cliente;

        -- Riferimento ad un task cliente
        type ac is access cliente; 
        -- Dichiarazione cliente
        new_Client: ac;

-- MAIN DEL PROGRAMMA (begin di ponte)
begin
        Reset(G); -- Inizializza il generatore in uno stato diverso ad ogni run
        for I in cliente_ID'range loop -- ciclo creazione task
                -- Genero direzione casuale
                D        := Random(G);
                V := Random(G);
                -- creazione cliente I-esimo
                new_Client := new cliente(I, direction'Val(D), vehicle'Val(V)); 
                --exit new_Client;
        end loop;
end ponte;