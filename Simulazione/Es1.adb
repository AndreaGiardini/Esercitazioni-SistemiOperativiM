with Ada.Text_IO, Ada.Integer_Text_IO;
use Ada.Text_IO, Ada.Integer_Text_IO;

procedure ponte is

    type cliente_ID is range 1..10;
    type dir_ID is (NORD, SUD);

 -- TASK SERVER
    task type server is
        --entraNORD(ID: in cliente_ID;  direz: dir_ID )
        entry entraNORD(ID: in cliente_ID; direz: out dir_ID);
        entry entraSUD(ID: in cliente_ID);
        entry esceNORD(ID: cliente_ID);
        entry esceSUD(ID: cliente_ID; direz: out dir_ID);
    end server;

    S: server;

 -- TASK CLIENTE
    task type cliente (ID: cliente_ID; DIR: dir_ID);

    direz : dir_ID;

    task body cliente is
    begin
        Put_Line("gruppo" & cliente_ID'Image (ID) & " di "& dir_ID'Image (DIR) &"iniziato!");
        --loop
            if DIR=NORD
            then
                S.entraNORD(ID,direz);
                delay 1.0;
                S.esceSUD(ID, direz);
                delay 1.0;
            end if;
            if DIR=SUD
            then
                S.entraSUD(ID);
                delay 1.0;
                S.esceNORD(ID);
                delay 1.0;
            end if;
        --end loop;
    end;

 -- TASK STRUTTURA
    task body server is
        MAX : constant INTEGER := 3; --capacità corsie

        inSUD : INTEGER := 0; --persone direzione SN
        inNORDN : INTEGER := 0; --persone direzione SN
        inNORDS : INTEGER := 0; --persone direzione NS

        --sulPonte : INTEGER := 0; --persone sul ponte
        --sulPonteDirNord : INTEGER := 0; --persone sul ponte in direzione nord
        --sulPonteDirSud  : INTEGER := 0; --persone sul ponte in direzione sud

            -- <inserire variabili di stato del ponte>
        Begin
            -- <inizializzare variabili di stato del ponte>
            --Gestione richieste:
            loop
                select
                    when inNORDS < MAX and inNORDN = 0 =>
                            accept entraSUD(ID: cliente_ID) do 
                                inNORDS:=inNORDS + 1;
                                Put_Line(" - gruppo" & cliente_ID'Image (ID) & " - Corsia nord direzione SN!");
                            end entraSUD;
                        --<accettazione richieste di accesso da sud>
                    or
                    when inSUD < MAX =>
                        accept entraNORD(ID: in cliente_ID; direz: out dir_ID) do 
--entry entraNORD(ID: in cliente_ID; direz: out dir_ID);
                            inSUD := inSUD+1;
                            direz := SUD;
                            Put_Line(" - gruppo" & cliente_ID'Image (ID) & " di SUD entrato!");
                        end entraNORD;
                        --<accettazione richieste di accesso da sud>
                    or
                    when inSUD = MAX and inNORDN < MAX and inNORDS =0 =>
                        accept entraNORD(ID: in cliente_ID; direz: out dir_ID) do 
                            inNORDN := inNORDN + 1;
                            direz := NORD;
                            Put_Line(" - gruppo" & cliente_ID'Image (ID) & " di SUD entrato!");
                        end entraNORD;
                        --<accettazione richieste di accesso da sud>
                    or

                        accept esceNORD(ID: cliente_ID) do 
                            inNORDS:=inNORDS-1;
                            Put_Line(" - gruppo" & cliente_ID'Image (ID) & " uscito a NORD!");
                        end esceNORD;
                        --<accettazione richieste di uscita da nord>
                    or
                        accept esceSUD(ID: cliente_ID; direz: out dir_ID) do 
                            if direz = NORD
                            then
                                inNORDN := inNORDN -1;
                            else
                                inSUD := inSUD -1;
                            end if;
                            Put_Line(" - gruppo" & cliente_ID'Image (ID) & " uscito a SUD!");
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

