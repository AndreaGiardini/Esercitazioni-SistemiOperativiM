public class Visitatore extends Thread 
{

	private Attrazione A;
	private int dimensione;
	private int tipo;
	
	public Visitatore(Attrazione A, int tipo, int dimensione){
		this.A = A;
		this.tipo = tipo;
		if (tipo == A.ADULTO)
			this.dimensione = 1;
		else
			this.dimensione = dimensione;
	}
	
	public void run() {
		try { 
			int tid = Integer.parseInt(this.getName().split("-")[1]); // La getName() torna una stringa del tipo Thread-X con X numerico
			A.Entra(tid, tipo, dimensione);
			sleep((int)(Math.random()*1000));
			A.Esci(tid, tipo, dimensione);
			
		} catch (Exception e) {}
	}
}