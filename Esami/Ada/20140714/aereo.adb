with Ada.Text_IO, Ada.Integer_Text_IO;
use Ada.Text_IO, Ada.Integer_Text_IO;

procedure aereo is
    TS: constant INTEGER := 30;
    TI: constant INTEGER := 30;
    type verso_ID is (SALI, SCENDI);

    type viaggiatore_ID is range 1..20;
    type equipaggio_ID is range 1..20;

    task type scala is
        entry entraViaggiatore(verso_ID) (ID: in viaggiatore_ID);
        entry esceViaggiatore(verso_ID) (ID: in viaggiatore_ID);
        entry entraEquipaggio(verso_ID) (ID: in equipaggio_ID);
        entry esceEquipaggio(verso_ID) (ID: in equipaggio_ID);
    end scala;

    S: scala;

    task type viaggiatore (ID: viaggiatore_ID; DIR: verso_ID);

    task body viaggiatore is
    begin
        S.entraViaggiatore(DIR)(ID);
        delay 1.0;
        S.esceViaggiatore(DIR)(ID);
    end;

    task type equipaggio (ID: equipaggio_ID; DIR: verso_ID);

    task body equipaggio is
    begin
        S.entraEquipaggio(DIR)(ID);
        delay 1.0;
        S.esceEquipaggio(DIR)(ID);
    end;

    task body scala is
        cur_DIR: verso_ID := SCENDI;
        num_sup: INTEGER := 30;
        num_inf: INTEGER := 30;
        dentro: INTEGER := 0;
    begin
        loop
            select
            when dentro > 0 =>
                accept esceEquipaggio(SCENDI) (ID: in equipaggio_ID)
                do
                    dentro := dentro - 1;
                end esceEquipaggio;
            or
            when dentro > 0 =>
                accept esceViaggiatore(SCENDI) (ID: in viaggiatore_ID)
                do
                    dentro := dentro - 1;
                end esceViaggiatore;
            or
            when dentro > 0 =>
                accept esceEquipaggio(SALI) (ID: in equipaggio_ID)
                do
                    dentro := dentro - 1;
                end esceEquipaggio;
            or
            when dentro > 0 =>
                accept esceViaggiatore(SALI) (ID: in viaggiatore_ID)
                do
                    dentro := dentro - 1;
                end esceViaggiatore;
            or
            when dentro = 0 and then num_sup > num_inf =>
                accept entraEquipaggio(SCENDI) (ID: in equipaggio_ID)
                do
                    cur_DIR := SCENDI;
                    dentro := dentro + 1;
                    num_sup := num_sup - 1;
                    num_inf := num_inf + 1;
                end entraEquipaggio;
            or
            when dentro = 0 and then num_sup > num_inf =>
                accept entraViaggiatore(SCENDI) (ID: in viaggiatore_ID)
                do
                    cur_DIR := SCENDI;
                    dentro := dentro + 1;
                    num_sup := num_sup - 1;
                    num_inf := num_inf + 1;
                end entraViaggiatore;
            or
            when dentro = 0 and then num_sup <= num_inf =>
                accept entraEquipaggio(SALI) (ID: in equipaggio_ID)
                do
                    cur_DIR := SALI;
                    dentro := dentro + 1;
                    num_sup := num_sup + 1;
                    num_inf := num_inf - 1;
                end entraEquipaggio;
            or
            when dentro = 0 and then num_sup <= num_inf =>
                accept entraViaggiatore(SALI) (ID: in viaggiatore_ID)
                do
                    cur_DIR := SALI;
                    dentro := dentro + 1;
                    num_sup := num_sup + 1;
                    num_inf := num_inf - 1;
                end entraViaggiatore;
            or
            when cur_DIR = SALI and then num_sup < TS =>
                accept entraEquipaggio(SALI) (ID: in equipaggio_ID)
                do
                    dentro := dentro + 1;
                    num_sup := num_sup + 1;
                    num_inf := num_inf - 1;
                end entraEquipaggio;
            or
            when cur_DIR = SALI and then num_sup < TS =>
                accept entraViaggiatore(SALI) (ID: in viaggiatore_ID)
                do
                    dentro := dentro + 1;
                    num_sup := num_sup + 1;
                    num_inf := num_inf - 1;
                end entraViaggiatore;
            or
            when cur_DIR = SCENDI and then num_inf < TI =>
                accept entraEquipaggio(SCENDI) (ID: in equipaggio_ID)
                do
                    dentro := dentro + 1;
                    num_inf := num_inf + 1;
                    num_sup := num_sup - 1;
                end entraEquipaggio;
            or
            when cur_DIR = SCENDI and then num_inf < TI =>
                accept entraViaggiatore(SCENDI) (ID: in viaggiatore_ID)
                do
                    dentro := dentro + 1;
                    num_inf := num_inf + 1;
                    num_sup := num_sup - 1;
                end entraViaggiatore;
            end select;
        end loop;
    end;

    type av is access viaggiatore;
    type am is access equipaggio;

    new_viaggiatore: av;
    new_equipaggio: am;
begin
    for I in viaggiatore_ID'Range loop
        -- todo creare con verso random
        new_viaggiatore := new viaggiatore(I, SALI);
    end loop;

    for I in equipaggio_ID'Range loop
        -- todo creare con verso random
        new_equipaggio := new equipaggio(I, SCENDI);
    end loop;
end aereo;
