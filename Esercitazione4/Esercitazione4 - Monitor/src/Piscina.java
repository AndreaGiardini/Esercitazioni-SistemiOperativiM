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
	
	private Lock lock = new ReentrantLock();
	private Condition codaOrto = lock.newCondition();
	private Condition codaInfe = lock.newCondition();
	
	private void signalFunc(){
		if (sospOrto > 0){
			codaOrto.signal();
		} else {
			codaInfe.signal();
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
	 * Paziente entra in piscina
	 */
	public void entra( String tipo ) throws InterruptedException{
		lock.lock();
		if(tipo.equals("Ortopedico")){
			/*
			 * Paziente ortopedico
			 */
			while (isFull() || altriInPiscina(tipo)){
				sospOrto++;
				codaOrto.await();
				sospOrto--;
			}			
			ortoInPiscina++;inPiscina++;
		} else {
			/*
			 * Paziente infettivo
			 */
			
			while (isFull() || altriInPiscina(tipo) || sospOrto > 0){
				sospInfe++;
				codaInfe.await();
				sospInfe--;
			}			
			infeInPiscina++;inPiscina++;
			codaOrto.signal();
		}
		signalFunc();
		lock.unlock();
	}
	
	/*
	 * Paziente esce dalla piscina
	 */
	public void esce( String tipo ){
		lock.lock();
		if( tipo.equals("Ortopedico")){
			ortoInPiscina--; inPiscina--;
		} else {
			infeInPiscina--; inPiscina--;
		}
		
		signalFunc();
		
		lock.unlock();
	}

}
