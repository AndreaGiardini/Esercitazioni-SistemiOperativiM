public class Turista extends Thread 
{

	private Guide G;
	private int linguaRichiesta;
	private int turista;
	
	public Turista(Guide G, int turista, int linguaRichiesta){
		this.G = G;
		this.turista = turista;
		this.linguaRichiesta = linguaRichiesta;
	}
	
	public void run() {
		try { 
			String tid = this.getName();
			System.out.println("(" + tid + ") --------- Turista " + turista + " richiede guida " + linguaRichiesta + " -------------");
			int linguaOttenuta = G.Noleggio(turista,linguaRichiesta, tid);
			sleep((int)(Math.random()*1000));
			G.Riconsegna(linguaOttenuta, tid);
			
		} catch (Exception e) {}
	}
}