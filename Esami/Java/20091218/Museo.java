import java.util.concurrent.locks.*;

public class Museo {
	private final int MAX = 6;	// capacità museo
	private final int NG = 2;	// guide totali
	public final int MAX_PERSONE_IN_GRUPPO = 3; // max persone in un gruppo di guidati
	
	public final int GUIDATI  = 0;
	public final int LIBERI = 1;
	
	private Lock lock = new ReentrantLock();
	
	private Condition[][] codaIngresso = new Condition[2][MAX_PERSONE_IN_GRUPPO];
	private Condition[] codaUscita = new Condition[MAX_PERSONE_IN_GRUPPO]; // I liberi non vanno in coda!
	
	private int guideDisponibili;
	private int postiOccupati[] = new int[2];
	
	private int[][] sospesiIngresso = new int[2][MAX_PERSONE_IN_GRUPPO]; 
	private int[] sospesiUscita = new int[MAX_PERSONE_IN_GRUPPO]; 

		
	public Museo()
	{ 
		for (int i = 0; i < MAX_PERSONE_IN_GRUPPO; i++)
		{ 
			codaIngresso[GUIDATI][i] = lock.newCondition();
			codaIngresso[LIBERI][i] = lock.newCondition();
			codaUscita[i] = lock.newCondition();
			sospesiIngresso[LIBERI][i] = 0;
			sospesiIngresso[GUIDATI][i] = 0;
			sospesiUscita[i] = 0;
		}
		guideDisponibili = NG;
		postiOccupati[GUIDATI] = 0;
		postiOccupati[LIBERI] = 0;
	}
	private int postiOccupati()
	{
		return (postiOccupati[GUIDATI] + postiOccupati[LIBERI]);
	}

	private boolean attivaPerIngresso(int visitatore) { return attivaPerIngresso(visitatore, MAX_PERSONE_IN_GRUPPO); }
	
	private boolean attivaPerIngresso(int visitatore, int max)
	{		
		for (int i = 0; i < max; i++)
		{
			if (sospesiIngresso[visitatore][i] > 0)
			{
				codaIngresso[visitatore][i].signal();
				return true;
				// Non continuo con i cicli perchè devo rispettare la priorità sulla dimensione:
				// Se un gruppo da 4 non può uscire per la G >= L mentre un gruppo da 2 potrebbe farlo
				// quello da 2 non deve essere risvegliato perchè bisogna rispettare la "escono prima i + numerosi"
			}
		}
		return false;			
	}
	private void attivaPerUscita() { attivaPerUscita(MAX_PERSONE_IN_GRUPPO); }
	private void attivaPerUscita(int dimensione)
	{
		for (int i = dimensione - 1; i >= 0; i--)
		{
			if (sospesiUscita[i] > 0)
			{
				codaUscita[i].signal();
				return;
			}
		}
	}
	
	// Cerco per il visitatore corrente se c'è qualcuno in attesa
	private boolean cercaPrioritariIngresso(int visitatore) { return cercaPrioritariIngresso(visitatore,MAX_PERSONE_IN_GRUPPO); }
	
	// Cerco per il visitatore corrente se c'è qualcuno più prioritario (quindi di dimensione minore) in attesa
	private boolean cercaPrioritariIngresso(int visitatore, int dimensione) // visitatore = {GUIDATI, LIBERI }
	{
		for (int i = 0; i < dimensione; i++)
		{
			if (sospesiIngresso[visitatore][i] > 0)
				return true;
		}
		return false;
	}
	private boolean cercaPrioritariUscita() { return cercaPrioritariUscita(0); }
	
	private boolean cercaPrioritariUscita(int dimensione)
	{
		for (int i = MAX_PERSONE_IN_GRUPPO - 1; i >= dimensione; i--)
		{
			if (sospesiUscita[i] > 0)
				return true;
		}
		return false;
	}

	public void EntraLibero(int tid, int dimensione)
	{
		/*
		Un gruppo libero non può entrare se:
			- Non c'è abbastanza spazio
			- Se ci sono guidati in attesa
			- Se ci sono liberi meno numerosi in attesa
			- Se si violerebbe la condizione G >= L
		*/
		lock.lock();
		int i;
		try {
				
			while ( 
					postiOccupati() + dimensione >= MAX ||
					cercaPrioritariIngresso(GUIDATI) ||
					cercaPrioritariIngresso(LIBERI, dimensione) ||
					postiOccupati[LIBERI] + dimensione > postiOccupati[GUIDATI]
					)
			{	
				// Mi sospendo sulla coda corretta
				sospesiIngresso[LIBERI][dimensione-1]++;
				Print(tid, "Gruppo liberi di dimensione " + dimensione + " sospeso all'ingresso");
				codaIngresso[LIBERI][dimensione-1].await();
					
				// Thread riattivato
				Print(tid, "Gruppo liberi di dimensione " + dimensione + " risvegliato");
				sospesiIngresso[LIBERI][dimensione-1]--;
			}
			// Gruppo entrato
			postiOccupati[LIBERI] += dimensione;
			Print(tid, "Gruppo liberi di dimensione " + dimensione + " entrato (posti disponibili: " + new Integer(MAX-postiOccupati()) +")");
			
			// Se sono entrato io di guidati in attesa non ce ne sono
			// Libero i liberi meno prioritari (meno numerosi) per l'ingresso
			// Visto che G >= L essendo entrato un libero non posso far uscire un guidato (L sale, G scende, G => L violata)
			
			// Cerco il gruppo di liberi più prioritario per l'ingresso
			attivaPerIngresso(LIBERI, dimensione);

		} catch (Exception e) { e.printStackTrace(); }
		finally {
			lock.unlock();
		}
	}
		
	public void EsceLibero(int tid, int dimensione)
	{
		lock.lock();
		
		// Un gruppo di liberi non si può bloccare in attesa	
		try {
			
			// Gruppo uscito
			postiOccupati[LIBERI] -= dimensione;

			Print(tid, "Gruppo liberi di dimensione " + dimensione + " uscito");
			// Essendo uscito posso risvegliare per l'ingresso i guidati più prioritari e se non ce ne sono
			// posso cercare tra i liberi
			// Posso anche risvegliare i guidati bloccati in uscita per la G >= L
			
			// Cerco i guidati (meno numerosi) per l'ingresso	
			if (attivaPerIngresso(GUIDATI) == false)
				// Cerco tra i liberi
				attivaPerIngresso(LIBERI);
			
			// Sveglio i guidati più numerosi bloccati in uscita
			attivaPerUscita();
			
			
		} catch (Exception e) { e.printStackTrace(); }
		finally {
			lock.unlock();
		}
	}
	
	public void EntraGuidato(int tid, int dimensione)
	{
		/*
			Un gruppo guidato non può entrare se:
				- Non ci sono più posti disponibili
				- Se ci sono gruppi di guidati meno numerosi in attesa
				- Se non ci sono guide disponibili
		*/
		lock.lock();
		int i;
		try {
				
			while ( 
					postiOccupati() + dimensione >= MAX ||
					cercaPrioritariIngresso(GUIDATI, dimensione) ||
					guideDisponibili == 0
					)
			{	
				// Mi sospendo sulla coda corretta
				sospesiIngresso[GUIDATI][dimensione-1]++;
				Print(tid, "Gruppo guidati di dimensione " + dimensione + " sospeso all'ingresso");
				codaIngresso[GUIDATI][dimensione-1].await();
					
				// Thread riattivato
				Print(tid, "Gruppo guidati di dimensione " + dimensione + " risvegliato");
				sospesiIngresso[GUIDATI][dimensione-1]--;
			}
			// Gruppo entrato
			postiOccupati[GUIDATI] += dimensione;
			guideDisponibili--;
			Print(tid, "Gruppo guidati di dimensione " + dimensione + " entrato (posti disponibili: " + new Integer(MAX-postiOccupati()) +")");
			
			// Cerco se ci sono guidati più numerosi di me in attesa di entrare,
			// se non ce ne sono cerco tra i liberi
			// Posso anche cercare di far uscire un gruppo di guidati (il più numeroso)
			if (attivaPerIngresso(GUIDATI, dimensione) == false)
				attivaPerIngresso(LIBERI);
			
			// Cerco di far uscire un gruppo guidati
			attivaPerUscita();
			
		} catch (Exception e) { e.printStackTrace(); }
		finally {
			lock.unlock();
		}
	}
	
	public void EsceGuidato(int tid, int dimensione)
	{
		lock.lock();
		
		/*
			Un gruppo di guidati non può uscire se:
				- Ci sono in attesa di uscire guidati più numerosi
				- Si violerebbe la condizione G => L
		*/
		try {
			
			while ( 
					cercaPrioritariUscita(dimensione) ||
					postiOccupati[GUIDATI] - dimensione < postiOccupati[LIBERI]
				)
			{	
				// Mi sospendo sulla coda corretta
				sospesiUscita[dimensione-1]++;
				Print(tid, "Gruppo guidati di dimensione " + dimensione + " sospeso in uscita");
				codaUscita[dimensione-1].await();
					
				// Thread riattivato
				Print(tid, "Gruppo guidati di dimensione " + dimensione + " risvegliato per l'uscita");
				sospesiUscita[dimensione-1]--;
			}
			// Gruppo uscito
			postiOccupati[GUIDATI] -= dimensione;
			guideDisponibili++;

			Print(tid, "Gruppo guidati di dimensione " + dimensione + " uscito");
			// Essendo uscito posso risvegliare per l'ingresso i guidati meno numerosi e se non ce ne sono
			// posso cercare tra i liberi
			// Posso anche cercare altri guidati bloccati in uscita con dimensione minore della mia
			
			// Cerco i guidati (meno numerosi) per l'ingresso	
			if (attivaPerIngresso(GUIDATI) == false)
				// Cerco tra i liberi
				attivaPerIngresso(LIBERI);
			
			// Sveglio i guidati più numerosi bloccati in uscita
			attivaPerUscita(dimensione);
			
			
		} catch (Exception e) { e.printStackTrace(); }
		finally {
			lock.unlock();
		}
	}
	
	private void Print(int tid, String message)
    {
        String a = "";
        for (int i = 0; i < tid; i++)
            a += "   ";
        System.out.println(a + "[" + tid + "] " + message);
    }

}