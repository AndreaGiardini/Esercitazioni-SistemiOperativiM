
public class Paziente extends Thread {

	private String tipo;
	private Piscina ps;
	
	public Paziente(String tipo, Piscina ps){
		this.tipo = tipo;
		this.ps = ps;
	}
	
	public void run(){
		try {
			ps.entra(tipo);
			System.out.println("Paziente " + tipo + "entrato");
			Thread.sleep(2000);
			ps.esce(tipo);
			System.out.println("Paziente " + tipo + "uscito");
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
}
