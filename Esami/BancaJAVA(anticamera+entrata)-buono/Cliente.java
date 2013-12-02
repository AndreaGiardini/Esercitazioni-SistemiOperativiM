import java.util.Random;
public class Cliente extends Thread {

    Banca B;
    int tipologia;
    
    public Cliente(Banca B, int tipologia)
	{
		this.B = B;
		this.tipologia = tipologia;
    }

	public void run() {
		try { 
			int tid = Integer.parseInt(this.getName().split("-")[1]); // La getName() torna una stringa del tipo Thread-X con X numerico
			
			B.Entra(tid,tipologia);
			sleep(new Random().nextInt(2)*1000); // Attende 0 o 1 secondi
			B.Esci(tid,tipologia);
			
		} catch (Exception e) {}
	}
}

