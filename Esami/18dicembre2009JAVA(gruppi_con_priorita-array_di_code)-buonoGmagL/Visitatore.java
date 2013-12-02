public class Visitatore extends Thread 
{

	private Museo M;
	private int dimensione;
	private int tipo;
	
	public Visitatore(Museo M, int tipo, int dimensione) {
		this.M = M;
		this.tipo = tipo;
		this.dimensione = dimensione;
	}
	
	public void run() {
		try { 
			int tid = Integer.parseInt(this.getName().split("-")[1]); // La getName() torna una stringa del tipo Thread-X con X numerico
			if (tipo == M.LIBERI)
				M.EntraLibero(tid, dimensione);
			else
				M.EntraGuidato(tid, dimensione);
			sleep((int)(Math.random()*1000));
			if (tipo == M.LIBERI)
				M.EsceLibero(tid, dimensione);
			else
				M.EsceGuidato(tid, dimensione);
			
		} catch (Exception e) {}
	}
}