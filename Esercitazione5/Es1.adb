with Ada.Text_IO, Ada.Integer_Text_IO;
use Ada.Text_IO, Ada.Integer_Text_IO;

procedure ponte is

    type cliente_ID is range 1..10;
    type dir_ID is (NORD, SUD);

 -- TASK SERVER
    task type server is
        entry entraNORD(ID: in cliente_ID );
        entry esceNORD(ID: in cliente_ID );
        entry entraSUD(ID: in cliente_ID );
        entry esceSUD(ID: in cliente_ID );
    end server;

    S: server;

 -- TASK CLIENTE
    task type cliente (ID: cliente_ID; DIR: dir_ID);

    task body cliente is
    begin
        Put_Line("gruppo" & cliente_ID'Image (ID) & " di "& dir_ID'Image (DIR) &"iniziato!");
        --loop
            if DIR=NORD
            then
                S.entraNORD(ID);
                delay 1.0;
                S.esceNORD(ID);
                delay 1.0;
            end if;
            if DIR=SUD
            then
                S.entraSUD(ID);
                delay 1.0;
                S.esceSUD(ID);
                delay 1.0;
            end if;
        --end loop;
    end;

 -- TASK STRUTTURA
    task body server is
        MAX : constant INTEGER := 3; --capacità ponte
        sulPonte : INTEGER := 0; --persone sul ponte
        sulPonteDirNord : INTEGER := 0; --persone sul ponte in direzione nord
        sulPonteDirSud  : INTEGER := 0; --persone sul ponte in direzione sud

            -- <inserire variabili di stato del ponte>
        Begin
            -- <inizializzare variabili di stato del ponte>
            --Gestione richieste:
            loop
                select
                    when sulPonte < MAX and then sulPonteDirNord = 0 =>
                        --when sulPonteDirNord == 0 =>
                            accept entraNORD(ID: cliente_ID) do 
                                sulPonte:=sulPonte+1;
                                sulPonteDirSud:=sulPonteDirSud+1;
                                Put_Line(" - gruppo" & cliente_ID'Image (ID) & " di NORD entrato!");
                            end entraNORD;
                        --<accettazione richieste di accesso da nord>
                    or
                    when sulPonte < MAX and then sulPonteDirSud = 0 =>
                            accept entraSUD(ID: cliente_ID) do 
                                sulPonte:=sulPonte+1;
                                sulPonteDirNord:=sulPonteDirNord+1;
                                Put_Line(" - gruppo" & cliente_ID'Image (ID) & " di SUD entrato!");
                            end entraSUD;
                        --<accettazione richieste di accesso da sud>
                    or
                    --when sulPonte < MAX =>
                            accept esceNORD(ID: cliente_ID) do 
                                sulPonte:=sulPonte-1;
                                sulPonteDirSud:=sulPonteDirSud-1;
                                Put_Line(" - gruppo" & cliente_ID'Image (ID) & " di NORD uscito!");
                            end esceNORD;
                        --<accettazione richieste di uscita da nord>
                    or
                    --when sulPonte < MAX =>
                            accept esceSUD(ID: cliente_ID) do 
                                sulPonte:=sulPonte-1;
                                sulPonteDirNord:=sulPonteDirNord-1;
                                Put_Line(" - gruppo" & cliente_ID'Image (ID) & " di SUD uscito!");
                            end esceSUD;
                        --<accettazione richieste di uscita da sud>
                end select;
            end loop;
        end;

 -- STRUTTURA
    type ac is access cliente; -- riferimento ad un task cliente
        New_client: ac;
        begin -- equivale al main
            for I in cliente_ID'Range loop -- ciclo creazione task
                if I rem 2 = 0
                then
                    New_client := new cliente(I, NORD); -- creazione cliente I-simo
                else
                    New_client := new cliente(I, SUD); -- creazione cliente I-simo
                end if;
            end loop;
end ponte;


