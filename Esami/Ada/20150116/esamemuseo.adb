with Ada.Text_IO, Ada.Integer_Text_IO;
use Ada.Text_IO, Ada.Integer_Text_IO;

procedure EsameMuseo is

-- Tipi
type Biglietto is (Museo, Mostra, Combo);
type Sala is (SalaMuseo, SalaMostra);

task type ComplessoMuseale is
	entry entra(Biglietto);
	entry esci(tipoBiglietto: in Biglietto);
	entry entraAddetto(tipoSala: in Sala);
	entry esciAddetto(Sala);
end ComplessoMuseale;

task type Visitatore (tipoBiglietto: Biglietto; ID: Integer);
type AccessVisitatore is access Visitatore;

task type Operatore (tipoSala: Sala; ID: Integer);
type AccessOperatore is access Operatore;

-- Vars
CM: ComplessoMuseale;
V: AccessVisitatore;
O: AccessOperatore;

-- Complesso Museale
task body ComplessoMuseale is
	maxMuseo: constant Integer := 5;
	maxMostra: constant Integer := 5;
	visitatori: array(Sala'Range) of Integer;
	addetti: array(Sala'Range) of Integer;
begin
	-- Init
	Put_Line("ComplessoMuseale init...");
	for i in Sala'Range loop
		visitatori(i) := 0;
		addetti(i) := 0;
	end loop;

	-- Implementazione
	loop
		select
			when visitatori(SalaMuseo) < maxMuseo and addetti(SalaMuseo) > 1 =>
				accept entra(Museo) do
					visitatori(SalaMuseo) := visitatori(SalaMuseo) + 1;
				end entra;
		or
			when visitatori(SalaMostra) < maxMostra and addetti(SalaMostra) > 1 and entra(Museo)'COUNT = 0 =>
				accept entra(Mostra) do
					visitatori(SalaMostra) := visitatori(SalaMostra) + 1;
				end entra;
		or
			when visitatori(SalaMuseo) < maxMuseo and addetti(SalaMuseo) > 1 and visitatori(SalaMostra) < maxMostra and addetti(SalaMostra) > 1 and entra(Museo)'COUNT = 0 and entra(Mostra)'COUNT = 0 =>
				accept entra(Combo) do
					visitatori(SalaMuseo) := visitatori(SalaMuseo) + 1;
					visitatori(SalaMostra) := visitatori(SalaMostra) + 1;
				end entra;
		or
			accept esci(tipoBiglietto: in Biglietto) do
				if tipoBiglietto = Museo or tipoBiglietto = Combo then
					visitatori(SalaMuseo) := visitatori(SalaMuseo) - 1;
				end if;
				if tipoBiglietto = Mostra or tipoBiglietto = Combo then
					visitatori(SalaMostra) := visitatori(SalaMostra) - 1;
				end if;
			end esci;
		or
			accept entraAddetto(tipoSala: in Sala) do
				addetti(tipoSala) := addetti(tipoSala) + 1;
			end entraAddetto;
		or
			when visitatori(SalaMuseo) = 0 or addetti(SalaMuseo) > 1 =>
				accept esciAddetto(SalaMuseo) do
					addetti(SalaMuseo) := addetti(SalaMuseo) - 1;
				end esciAddetto;
		or
			when visitatori(SalaMostra) = 0 or addetti(SalaMostra) > 1 =>
				accept esciAddetto(SalaMostra) do
					addetti(SalaMostra) := addetti(SalaMostra) - 1;
				end esciAddetto;
		end select;
	end loop;
end;

-- Visitatore
task body Visitatore is
begin
	Put_Line("Visitatore con biglietto " & Biglietto'Image(tipoBiglietto) & Integer'Image(ID));
	CM.entra(tipoBiglietto);
	Put_Line("Visitatore con biglietto " & Biglietto'Image(tipoBiglietto) & Integer'Image(ID) & " e' entrato.");
	delay 1.0;
	CM.esci(tipoBiglietto);
	Put_Line("Visitatore con biglietto " & Biglietto'Image(tipoBiglietto) & Integer'Image(ID) & " e' uscito.");
end;

-- Operatore
task body Operatore is
begin
	loop
		Put_Line("Operatore di sala " & Sala'Image(tipoSala) & Integer'Image(ID));
		CM.entraAddetto(tipoSala);
		Put_Line("Operatore di sala " & Sala'Image(tipoSala) & Integer'Image(ID) & " e' entrato.");
		delay 5.0;
		CM.esciAddetto(tipoSala);
		Put_Line("Operatore di sala " & Sala'Image(tipoSala) & Integer'Image(ID) & " e' uscito.");
	end loop;
end;

-- Main body
begin
	Put_Line("Creazione visitatori...");
	for b in Biglietto'Range loop
		for i in 0..5 loop
			V := new Visitatore(b, i);
		end loop;
	end loop;

	delay 0.5;
	
	Put_Line("Creazione operatori...");
	for s in Sala'Range loop
		for i in 0..2 loop
			O := new Operatore(s, i);
		end loop;
	end loop;
end EsameMuseo;
