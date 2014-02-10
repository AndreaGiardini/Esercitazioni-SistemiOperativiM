with Ada.Text_IO, Ada.Integer_Text_IO;
use Ada.Text_IO, Ada.Integer_Text_IO;

procedure es6 is

    type cliente_ID is range 1..10;
    type torta is (cioccolata, marmellata);
    type confezione is (cioc, marm, family);

    task type server is
        entry deposito(torta) (ID: in cliente_ID); 
        entry prelievo(confezione) (ID: in cliente_ID);
    end server;

    task body server is
        --<variabili locali per rappr. Stato risorsa>
        MaxC : constant INTEGER := 10;
        ciocT : INTEGER := 0;
        marmT : INTEGER := 0;
        begin
        loop
            select
                --<accettazione/definizione entries>
                when maxC > ciocT + marmT =>
                    accept deposito(marmellata)(ID: cliente_ID) do 
                        marmT := marmT + 1;
                        Put_Line("Depositata MARMELLATA da " & cliente_ID'Image (ID));
                    end deposito;
                or
                when maxC > ciocT + marmT and deposito(marmellata)'COUNT = 0 =>
                    accept deposito(cioccolata)(ID: cliente_ID) do 
                        ciocT := ciocT + 1;
                        Put_Line("Depositata CIOCCOLATA da " & cliente_ID'Image (ID));
                    end deposito;
                or
                when marmT > 0 and ciocT > 0 =>
                    accept prelievo(family)(ID: cliente_ID) do 
                        ciocT := ciocT - 1;
                        marmT := marmT - 1;
                        Put_Line("Prelevata FAMILY da " & cliente_ID'Image (ID));
                    end prelievo;
                or
                when marmT > 0 and ciocT > 0 and prelievo(family)'COUNT = 0 =>
                    accept prelievo(marm)(ID: cliente_ID) do 
                        marmT := marmT - 1;
                        Put_Line("Prelevata MARMEL da " & cliente_ID'Image (ID));
                    end prelievo;
                or
                when marmT > 0 and ciocT > 0 and prelievo(family)'COUNT = 0 and prelievo(marm)'COUNT = 0 =>
                    accept prelievo(cioc)(ID: cliente_ID) do 
                        ciocT := ciocT - 1;
                        Put_Line("Prelevata CIOCCO da " & cliente_ID'Image (ID));
                    end prelievo;
                or
                terminate;
            end select;
        end loop;
    end server;

    S : server;

    task type clienteOP (ID: cliente_ID; T:torta);
    task body clienteOP is
    begin
        S. deposito(T)(ID);
    end;

    task type clienteOC(ID: cliente_ID; C:confezione);
    task body clienteOC is
    begin
        S. prelievo(C)(ID);
    end;

 -- STRUTTURA
    type op is access clienteOP; -- riferimento ad un task deposito
    New_clientOP: op;
    type oc is access clienteOC; -- riferimento ad un task prelievo
    New_clientOC: oc;

    begin
        for I in cliente_ID'Range loop -- ciclo creazione task
            --if I rem 2 = 0
            --then
                New_clientOP := new clienteOP(I, cioccolata);
                New_clientOP := new clienteOP(I, marmellata);
                New_clientOP := new clienteOP(I, cioccolata);
                New_clientOP := new clienteOP(I, marmellata);
            --else
                New_clientOC := new clienteOC(I, marm);
                New_clientOC := new clienteOC(I, cioc);
                New_clientOC := new clienteOC(I, family);
            --end if;
        end loop;
    Put_Line("LOL");
end es6;
