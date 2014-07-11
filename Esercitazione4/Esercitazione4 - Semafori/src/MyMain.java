import java.util.Random;


public class MyMain {

	/**
	 * Due tipologie di pazienti:
	 *  - Pazienti ortopedici
	 *  - Pazienti infettivi
	 *  
	 *  I pazienti accedono singolarmente alla piscina, vi rimangono per una quantità di tempo e poi escono
	 *  - MAXP - Capacità massima piscina
	 *  - Vietata la presenza contemporanea di pazienti ortopedici ed infettivi
	 *  - Pazienti ortopedici favoriti rispetto a quelli infettivi
	 * @throws InterruptedException 
	 */
	public static void main(String[] args) throws InterruptedException {
		
		int nPazienti = 20;
		Random rn = new Random();
		Thread[] pazienti = new Paziente[nPazienti];
		Piscina ps = new Piscina();
		
		for(int i = 0; i < nPazienti; i++){
			if ( rn.nextInt() % 2 == 0){
				pazienti[i] = new Paziente("Ortopedico", ps);
			} else {
				pazienti[i] = new Paziente("Infettivo", ps);
			}
		}

		for(int i = 0; i < nPazienti; i++){
			pazienti[i].start();
		}
		
		for(int i = 0; i < nPazienti; i++){
			pazienti[i].join();
		}
		
		System.out.println("Programma terminato");
		
	}

}
