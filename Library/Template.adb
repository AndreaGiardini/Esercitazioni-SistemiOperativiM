with Ada.Text_IO, Ada.Integer_Text_IO;
use Ada.Text_IO, Ada.Integer_Text_IO;

procedure Template is
    type client_ID is range 1..20;

    task type server is
    end server;

    S: server;

    task type client (ID: client_ID);

    task body client is
    begin
        null;
    end;

    task body server is
    begin
        loop
            null;
        end loop;
    end;
begin
    for I in client_ID'Range loop
        null;
    end loop;
end Template;
