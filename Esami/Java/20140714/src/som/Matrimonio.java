package som;

import java.util.concurrent.Semaphore;

public class Matrimonio {
	private final Semaphore semMutex = new Semaphore(1);
	
	private final Semaphore semBeviProsecco = new Semaphore(0);
	private final Semaphore semServiProsecco = new Semaphore(1);
	
	private final Semaphore semMangiaPranzo = new Semaphore(0);
	private final Semaphore semServiPranzo = new Semaphore(1);
	
	private final Semaphore semMangiaTorta = new Semaphore(0);
	private final Semaphore semServiTorta = new Semaphore(1);
	
	private final Semaphore semTortaTagliata = new Semaphore(0);
	
	private final Semaphore semFase2 = new Semaphore(0);
	private final Semaphore semFase3 = new Semaphore(0);

	private boolean tortaTagliata = false;
	private boolean puoServireIlPranzo = false;
	private boolean puoTagliareTorta = false;

	public void iniziaProsecco(String tipo) throws InterruptedException {
		semBeviProsecco.acquire();
		semServiProsecco.release();
	}

	public void finisceProsecco(String tipo) throws InterruptedException {
		semMutex.acquire();
		if ("Invitato".equals(tipo) && !puoServireIlPranzo) {
			puoServireIlPranzo = true;
			semFase2.release();
		}
		semMutex.release();
	}

	public void iniziaPranzo(String tipo) throws InterruptedException {
		semMangiaPranzo.acquire();
		semServiPranzo.release();
	}

	public void finiscePranzo(String tipo) throws InterruptedException {
		semMutex.acquire();
		if ("Invitato".equals(tipo) && !puoTagliareTorta) {
			puoTagliareTorta = true;
			semFase3.release();
		}
		semMutex.release();
	}

	public void iniziaTorta() throws InterruptedException {
		semMangiaTorta.acquire();
		semServiTorta.release();
	}

	public void finisceTorta() throws InterruptedException {
	}

	public void tagliaTorta() throws InterruptedException {
		semMutex.acquire();
		
		if (!puoTagliareTorta) {
			semMutex.release();
			semFase3.acquire();
			semMutex.acquire();
		}
		
		tortaTagliata = true;
		semTortaTagliata.release();
		
		semMutex.release();
	}

	public void serveProsecco(String tipo) throws InterruptedException {
		semServiProsecco.acquire();
		semBeviProsecco.release();
	}

	public void servePranzo(String tipo) throws InterruptedException {
		semMutex.acquire();
		if(!puoServireIlPranzo) {
			semMutex.release();
			semFase2.acquire();
			semMutex.acquire();
		}
		semMutex.release();
		
		semServiPranzo.acquire();
		semMangiaPranzo.release();
	}

	public void serveTorta() throws InterruptedException {
		semMutex.acquire();
		if (!tortaTagliata) {
			semMutex.release();
			semTortaTagliata.acquire();
			semMutex.acquire();
		}
		semMutex.release();

		semServiTorta.acquire();
		semMangiaTorta.release();
	}
}
