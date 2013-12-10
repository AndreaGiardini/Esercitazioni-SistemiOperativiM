import java.util.concurrent.Semaphore;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;


public class Piscina {

	private final int MAXP = 3;
	private int inPiscina = 0;
	private int ortoInPiscina = 0;
	private int infeInPiscina = 0;
	private int sospOrto = 0;
	private int sospInfe = 0;
	
	Semaphore Sm;
	Semaphore So;
	Semaphore Si;
	
	public Piscina(){
		Sm = new Semaphore(1);
		Si = new Semaphore(0);
		So = new Semaphore(0);
	}
	
	private void signalFunc(){
		if (sospOrto > 0){
			So.release();
		} else {
			Si.release();
		}
	}
	
	private boolean altriInPiscina(String tipo){
		if (tipo.equals("Ortopedico") && infeInPiscina > 0){
			return true;
		}
		if (tipo.equals("Infettivo") && ortoInPiscina > 0){
			return true;
		}
		return false;
	}
	
	private boolean isFull(){
		if (inPiscina == MAXP){
			return true;
		}
		return false;
	}
	
	/*
	 * Schema con attesa circolare
	 */
	
	/*
	 * Paziente entra in piscina
	 */
	public void entra( String tipo ) throws InterruptedException{
		Sm.acquire();
		if(tipo.equals("Ortopedico")){
			/*
			 * Paziente ortopedico
			 */
			while (isFull() || altriInPiscina(tipo)){
				sospOrto++;
				//codaOrto.await();
				Sm.release();
				So.acquire();
				Sm.acquire();
				sospOrto--;
			}			
			ortoInPiscina++;inPiscina++;
		} else {
			/*
			 * Paziente infettivo
			 */
			
			while (isFull() || altriInPiscina(tipo) || sospOrto > 0){
				sospInfe++;
				//codaInfe.await();
				Sm.release();
				Si.acquire();
				Sm.acquire();
				sospInfe--;
			}			
			infeInPiscina++;inPiscina++;
		}
		signalFunc();
		Sm.release();
	}
	
	/*
	 * Paziente esce dalla piscina
	 */
	public void esce( String tipo ) throws InterruptedException{
		Sm.acquire();
		if( tipo.equals("Ortopedico")){
			ortoInPiscina--; inPiscina--;
		} else {
			infeInPiscina--; inPiscina--;
		}
		
		signalFunc();
		
		Sm.release();
	}

}
