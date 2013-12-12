with Ada.Text_IO, Ada.Integer_Text_IO, Ada.Numerics.Discrete_Random;
use Ada.Text_IO, Ada.Integer_Text_IO;

procedure tavolo is
	-- Costanti
	MaxC : constant integer := 20; -- torte massime tavolo

	id_MIN : constant integer := 1; --id operaio minimo
	id_MAX : constant integer := 40; --id operaio massimo

	-- Definizioni
	type cliente_ID is range id_MIN..id_MAX;
	type torta is (cioccolata, marmellata);
	type confezione is (cioccolata, marmellata, famiglia);

	-- Generatore di torte casuali
	package Torta_Random is new Ada.Numerics.Discrete_Random(torta);
	use Torta_Random;
	TG: Torta_Random.Generator;

	-- Generatore di confezioni casuali
	package Confezione_Random is new Ada.Numerics.Discrete_Random(confezione);
	use Confezione_Random;
	CG: Confezione_Random.Generator;

	-- DEFINIZIONE GESTORE DEL TAVOLO
	task type server is 
		-- Metodi del gestore del tavolo
		entry deposito(torta)(ID: in cliente_ID);
		entry prelievo(confezione)(ID: in cliente_ID);
	end server;

	-- CORPO DEL GESTORE DEL PONTE
	task body server is
		-- Variabili di stato del tavolo
		torteCioccolata: integer; --numero di torte alla cioccolata sul tavolo
		torteMarmellata: integer; --numero di torte alla marmellata sul tavolo
	begin
		-- Inizializzazione variabili di stato del tavolo
		torteCioccolata :=0;
		torteMarmellata :=0;
		--Gestione richieste:
    loop
    	select
				-- ACCETTAZIONE RICHIESTE DEPOSITO TORTE SULLA TAVOLA
				-- priorità alla marmellata
				when (torteCioccolata + torteMarmellata) < MaxC =>
					accept deposito(marmellata)(ID: cliente_ID) do
						torteMarmellata:=torteMarmellata+1;
						Put_Line("OP (id:" & cliente_ID'Image (ID) & ") ha depositato una torta alla "&torta'Image (marmellata)&" sul tavolo");
					end deposito;
				-- se non ci sono depositi di marmellata in attesa, deposita torta alla cioccolata
				or when (torteCioccolata + torteMarmellata) < MaxC and deposito(marmellata)'COUNT = 0 =>
					accept deposito(cioccolata)(ID: cliente_ID) do
						torteCioccolata:=torteCioccolata+1;
						Put_Line("OP (id:" & cliente_ID'Image (ID) & ") ha depositato una torta alla "&torta'Image (cioccolata)&" sul tavolo");
					end deposito;
				-- ACCETTAZIONE RICHIESTE PRELIEVO TORTE DALLA TAVOLA PER CREARE LE CONFEZIONI
				-- priorità alle confezioni famiglia
				or when torteCioccolata > 0 and torteMarmellata > 0 =>
					accept prelievo(famiglia)(ID: cliente_ID) do
						torteMarmellata:=torteMarmellata-1;
						torteCioccolata:=torteCioccolata-1;
						Put_Line("OC (id:" & cliente_ID'Image (ID) & ") ha prelevato una torta per tipo per creare una confezione "&confezione'Image (famiglia));
					end prelievo;
				-- tra le confezioni semplici, priorità alla marmellata
				or when torteMarmellata > 0 and prelievo(famiglia)'COUNT = 0 =>
					accept prelievo(marmellata)(ID: cliente_ID) do
						torteMarmellata:=torteMarmellata-1;
						Put_Line("OC (id:" & cliente_ID'Image (ID) & ") ha prelevato una torta alla "&confezione'Image (marmellata)&" per creare una confezione");
					end prelievo;
				-- se non ci sono altre richieste in sospeso, crea confezione cioccolata
				or when torteCioccolata > 0 and prelievo(famiglia)'COUNT = 0 and prelievo(marmellata)'COUNT = 0 =>
					accept prelievo(cioccolata)(ID: cliente_ID) do
						torteCioccolata:=torteCioccolata-1;
						Put_Line("OC (id:" & cliente_ID'Image (ID) & ") ha prelevato una torta alla "&confezione'Image (cioccolata)&" per creare una confezione");
					end prelievo;
				or terminate;
			end select;
		end loop;
	end server;

	-- Dichiarazione gestore del tavolo
	S: server; 

	-- DEFINIZIONE DEI TASK CLIENTI
	task type clienteOP (ID: cliente_ID; T: torta);
	task type clienteOC (ID: cliente_ID; C: confezione);

	-- CORPO DEI TASK CLIENTI
	task body clienteOP is
	begin
		Put_Line("L'operaio OP (id:"& cliente_ID'Image (ID)&") vuole depositare una torta alla "&torta'Image (T));
		S.deposito(T)(ID);
	end clienteOP;

	task body clienteOC is
	begin
		Put_Line("L'operaio OC (id:"& cliente_ID'Image (ID)&") vuole prelevare una confezione "&confezione'Image (C));
		S.prelievo(C)(ID);
	end clienteOC;

	-- Riferimenti ai task clienti
	type OP is access clienteOP;
	type OC is access clienteOC;
	-- Dichiarazioni clienti
	operaiOP: array(cliente_ID) of OP;
	operaiOC: array(cliente_ID) of OC;

-- MAIN DEL PROGRAMMA (begin di tavolo)
begin
	-- Inizializza i generatori in uno stato diverso ad ogni run
	Reset(TG);
	Reset(CG);
	for I in cliente_ID'range loop -- ciclo creazione task
		-- creazione operaio I-esimo
		-- gli operai dispari depositano
		if I rem 2 = 1
		then
			-- Genero torta casuale
			operaiOP(I) := new clienteOP(I, Random(TG));
		-- gli operai pari prelevano
		else
			-- Genero confezione casuale
			operaiOC(I) := new clienteOC(I, Random(CG)); 
		end if;
	end loop;
end tavolo;
