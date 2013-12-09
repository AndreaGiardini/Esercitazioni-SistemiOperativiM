--------------------------------------------------------------------------------
--  *  Prog name pool1.adb
--  *  Project name pool1
--  *
--  *  Version 1.0
--  *  Last update 19/12/12
--  *
--  *  Created by anna ciampolini on 19/12/12.
--  *  Copyright (c) 2012 __MyCompanyName__.
--  *  All rights reserved.
--  *    or (keep only one line or write your own)
--  *  GNAT modified GNU General Public License
--  *
--------------------------------------------------------------------------------

with Ada.Text_IO;  use Ada.Text_IO;
procedure poolADA is
 
   type cliente_ID is range 1..10;			-- 10 clienti
   type ris_ID is range 1..5;			-- 5 risorse nel pool
 
   task type cliente (ID: cliente_ID);		-- task che rappresenta il generico cliente
   type ac is access cliente;				-- riferimento ad un task cliente
   
   task type risorsa (ID: ris_ID) is	-- task che rappresenta il generico gestore di 1 risorsa
	entry operazione(ID: in cliente_ID; OK: out Boolean);
   end risorsa;
   
   type ar is access all risorsa;				-- riferimento ad un task gestore di risorsa
   
   type pool  is array(ris_ID'Range) of ar;
   P:  pool;

   -- processo gestore del pool
   task type server is
      entry Richiesta (ID: in cliente_ID; RIS: out ar; IND: out ris_ID);
      entry Rilascio(ID: in cliente_ID; IND: in ris_ID);
   end server;
 
  
  S: server;	-- creazione server

  
   ------ definizione gestore della risorsa ------
   task body risorsa is
    mioid: ris_ID;
	begin
      Put_Line ("risorsa  " & ris_ID'Image (ID) & " iniziata ! ");
		mioid:=ID;
	  loop  
		select 
			accept operazione (ID: in cliente_ID; OK: out Boolean) do
				Put_Line ("risorsa " & ris_ID'Image (mioid) & " in uso da " & cliente_ID'Image (ID) & " ! ");
				delay 1.0;
				OK:=True;
			end operazione;
		end select;
	  end loop;
 
   end risorsa;

------ definizione PROCESSO server: 
   task body server   is
   disp: Integer;
   
   k: ris_ID;
   libere: array(ris_ID'Range) of Boolean;
   begin
    k:=2; 
	Put_Line ("SERVER iniziato!");
	  --inizializzazione stato pool:
	disp:=5;  
	for i in ris_ID'Range loop          
      libere(i):=True;
     end loop;
    
	loop
         select                      
		   when disp > 0 =>  -- c'è almeno una risorsa libera
             accept Richiesta (ID: in cliente_ID; RIS: out ar; IND: out ris_ID) do
				Put_Line ("server ha ricevuto richiesta dal cliente "& cliente_ID'Image(ID) &" !");

               --  individuazione risorsa libera 
               for k in ris_ID'Range loop          
					if libere(k)=True
					then 
						Put_Line ("server ha trovato la risorsa" & ris_ID'Image(k) & " per il cliente!");
						libere(k):=False;
    					 disp:=disp-1;
						IND:=k;
						RIS :=P(k); 
						exit;
					end if;
				end loop;

			end Richiesta;               -- end of the synchronised part
         or
            accept  Rilascio(ID: in cliente_ID; IND: in ris_ID) do  ---- QUI!
               --aggiornamento stato pool P:
			   libere(IND):=True;
			   disp:=disp+1;
            end Rilascio;
        end select;
      end loop;
   end;
  
 ------------------processo cliente: acquisisce e rilascia risorse
   task body cliente is
   GR: ar;  
   INDEX: ris_ID;
   OK: Boolean;
   begin
      Put_Line ("cliente" & cliente_ID'Image (ID) & " iniziato!");
	  S. Richiesta(ID, GR, INDEX);
	  Put_Line ("cliente ha ottenuto una risorsa!");

	  GR.operazione(ID, OK); ---Accesso alla risorsa
	  S.Rilascio(ID, INDEX);
      Put_Line ("cliente terminato!");
	end;
------------------------------- main: 
   New_client: ac;
  
begin -- equivale al main
	for I in ris_ID'Range loop
		P(I):=new risorsa(I);
	end loop;
	
   for I in cliente_ID'Range loop  -- ciclo creazione task
      New_client := new cliente (I); -- creazione cliente I-simo
--   delay 3.0;
   end loop;
end poolADA;
