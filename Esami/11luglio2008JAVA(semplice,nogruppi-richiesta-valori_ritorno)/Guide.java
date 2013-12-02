import java.util.concurrent.locks.*;

public class Guide {
	private final int NI = 3; // audioguide italiane
	private final int NE = 2; // audioguide inglesi
	private final int []N = { NI,NE };
	
	private final int ITA = 0;
    private final int ENG = 1;
	
	private Lock lock = new ReentrantLock();
	
	private Condition[][] codaAudioguida = new Condition[2][2]; // Una per audioguide italiane e una per audioguide inglesi, per ogni richiedente
	
	private int[] guideDisponibili = new int[2];
	
	private int[][] sospesiCoda = new int[2][2];
	
	public Guide()
	{ 
		codaAudioguida[ITA][ITA] = lock.newCondition();
		codaAudioguida[ITA][ENG] = lock.newCondition();
		codaAudioguida[ENG][ENG] = lock.newCondition();
		codaAudioguida[ENG][ITA] = lock.newCondition();
		
		guideDisponibili[ITA] = NI;
		guideDisponibili[ENG] = NE;
		
		sospesiCoda[ITA][ITA] = 0;
		sospesiCoda[ITA][ENG] = 0;
		sospesiCoda[ENG][ENG] = 0;
		sospesiCoda[ENG][ITA] = 0;
	
	}

	public int Noleggio(int turista, int richiesta, String tid) // turista ITA o ENG, richiesta ITA o ENG
	{
		/*
			Un turista che richiede una audioguida italiana si ferma se:
				- Non ci sono audioguide disponibili
				- Gli devo assegnare una audioguida straniera (o perchè la ha chiesta o perchè le ita erano finite) ma ci sono
					turisti stranieri in attesa e io sono italiano
			Un turista che richiede una audioguida straniera si ferma se:
				- Non ci sono audioguide straniere disponibili
				- Io non sono italiano e ci sono turisti italiani in attesa (per guide italine)
		*/
		int linguaOttenuta = richiesta;
		lock.lock();
		try {
		
			while ( richiesta == ITA && (guideDisponibili[ITA] + guideDisponibili[ENG] == 0 || // Guide finite
										(turista == ITA && guideDisponibili[ITA] == 0 && sospesiCoda[ENG][ENG] > 0) )
					||
					
					richiesta == ENG && (guideDisponibili[ENG] == 0 || // Guide finite
					(turista != ITA && sospesiCoda[ITA][ITA] > 0)) )
					
			{	
				// Mi sospendo sulla coda della mia richiesta
				sospesiCoda[turista][richiesta]++;
				System.out.println("(" + tid + ") Turista " + turista + " in attesa di guida " + richiesta);	
				codaAudioguida[turista][richiesta].await();
				
				sospesiCoda[turista][richiesta]--;
				System.out.println("(" + tid + ") Turista " + turista + " in attesa di guida " + richiesta + " risvegliato");
			}
			// Richiesta esaudita, quale lingua?
			// Se avevo richiesto italiana allora do italiana se ci sono, altrimenti eng
			if (richiesta == ITA)
			{
				if (guideDisponibili[ITA] > 0 )
				{
					guideDisponibili[ITA]--;
					System.out.println("(" + tid + ") Guida " + ITA + " assegnata a turista " + turista);
					linguaOttenuta = ITA;
				}
				else
				{
					guideDisponibili[ENG]--;
					System.out.println("(" + tid + ") Guida " + ENG + " assegnata a turista " + turista);
					linguaOttenuta = ENG;
				}
			} // Altrimenti avevo richiesto staniera e mi sono sbloccato perchè ce ne sono
			else
			{
				guideDisponibili[ENG]--;
				System.out.println("(" + tid + ") Guida " + ENG + " assegnata a turista " + turista);
				linguaOttenuta = ENG;
			}
			System.out.println(" ++++++++++++ guideDisponibili[ITA] " + guideDisponibili[ITA] + " guideDisponibili[ENG] " + guideDisponibili[ENG] + " ++++++++++++");
			
			
			// Ora che ho ricevuto la mia guida sblocco il turista più prioritario in attesa di audioguida
			// Se sono italiano e ho chiesto italiano posso sbloccare uno straniero che mi dava la precedenza (se non ci sono italiani in attesa)
			// Se sono straniero e ho preso una italiana allora non c'era nessuno in attesa altrimenti gli avrei dato precedenza
			
			if (turista == ITA && richiesta == ITA)
				codaAudioguida[ENG][ITA].signal();

		 
		} catch (Exception e) {}
		finally {
			lock.unlock();
		}
		
		return linguaOttenuta;
	}	
		
	public void Riconsegna(int audioguida, String tid)
	{
		lock.lock();
		
		try {		
			// Guida riconsegnata
			guideDisponibili[audioguida]++;

			System.out.println("(" + tid + ") Guidata " + audioguida + " riconsegnata da turista");
			
			// E' tornata una audioguita sblocco chi era in attesa
			if (sospesiCoda[audioguida][audioguida] > 0)
				codaAudioguida[audioguida][audioguida].signal();
			else 
				codaAudioguida[(audioguida + 1) %2][audioguida].signal();
				
		} catch (Exception e) {}
		finally {
			lock.unlock();
		}
	}
}