import java.util.concurrent.locks.*;

public class Attrazione {
	private final int N = 6;	// capacità attrazione
	public final int MAX_PERSONE_IN_GRUPPO = 3; // max persone in un gruppo di guidati
	
	private final int JUNIOR  = 0;
	private final int EXTREME = 1;
	private final int INVALID = 2;
	

	public final int FAMIGLIA   = 0;
	public final int SCOLARESCA = 1;
	public final int ADULTO     = 2;
	
	private Lock lock = new ReentrantLock();
	
	private Condition[][] condGruppi = new Condition[2][MAX_PERSONE_IN_GRUPPO];
	// Per gli adulti basta una variabile, non importa un array
	// (in realtà fare un array [3] avrebbe semplificato i controlli dopo.. ma sarebbe stato uno spreco)
	private Condition condAdulti;
	
	private int programma; // il programma correntemente attivato
	private int[] postiOccupati = new int[3];
	
	private int[][] sospesiGruppi = new int[2][MAX_PERSONE_IN_GRUPPO]; 
	private int sospesiAdulti;

		
	public Attrazione()
	{ 
		int i;
		for (i = 0; i < MAX_PERSONE_IN_GRUPPO; i++)
		{ 
			sospesiGruppi[FAMIGLIA][i] = 0;
			sospesiGruppi[SCOLARESCA][i] = 0;
			condGruppi[FAMIGLIA][i] = lock.newCondition();
			condGruppi[SCOLARESCA][i] = lock.newCondition();
		}
		sospesiAdulti = 0;
		condAdulti = lock.newCondition();
		programma = INVALID;	
		postiOccupati[FAMIGLIA] = 0;
		postiOccupati[SCOLARESCA] = 0;
		postiOccupati[ADULTO] = 0;
	}

	private int postiOccupati()
	{
		return postiOccupati[FAMIGLIA] + postiOccupati[SCOLARESCA] + postiOccupati[ADULTO];
	}
	private int totaleSospesi(int visitatore)
	{
		if (visitatore == ADULTO)
			return sospesiAdulti;
		int ret = 0;
		for (int i = 0; i < MAX_PERSONE_IN_GRUPPO; i++)
			ret += sospesiGruppi[visitatore][i];
		return ret;
	}
	
	// Cerco per il visitatore corrente se c'è qualcuno in attesa
	private boolean cercaPrioritari(int visitatore) { return cercaPrioritari(visitatore,-1); }
	// Cerco per il visitatore corrente se c'è qualcuno più prioritario (quindi di dimensione maggiore) in attesa
	private boolean cercaPrioritari(int visitatore, int dimensione) // visitatore = {FAMIGLIA, SCOLARESCA }
	{
		int trovateProritarie = 0, trovate = 0;
		for (int i = MAX_PERSONE_IN_GRUPPO - 1; i >= 0; i--)
		{
			if (sospesiGruppi[visitatore][i] > 0)
			{
				trovate = 1;
				System.out.println("Trovato " + visitatore + " in attesa");
				if (dimensione > 0 && i > dimensione)
				{
					System.out.println("Trovato " + visitatore + " più numeroso in attesa");
					trovateProritarie = 1;
					break;
				}
			}
		}
		// Potrei fare una condizione unica, ma cosi si legge meglio
		if (dimensione > 0) return (trovateProritarie != 0);
		else return (trovate != 0);
	}
	
	public void Entra(int tid, int visitatore, int dimensione) // visitatore = {ADULTO, FAMIGLIA, SCOLARESCA }
	{
		/*
		Un visitatore non può entrare se:
		- Non c'è abbastanza spazio
		- Se è adulto:
			- Dentro il programma != EXTREME (allora ci sono FAMIGLIE/SCOLARESCHE)
			- Se ci sono in attesa FAMIGLIE/SCOLARESCHE
		- Se è famiglia:
			- Dentro il programma != JUNIOR (allora dentro ci sono adulti)
			- Ci sono scolaresche in attesa
			- Ci sono gruppi di famiglie più numerosi di me
		- Se è scolaresca:
			- Dentro il programma != JUNIOR (allora dentro ci sono adulti)
			- Ci sono gruppi di scolaresche più numerosi di me		
		*/
		lock.lock();
		int i, trovateFamiglieProritarie = 0, trovateScolarescheProritarie = 0, trovateScolaresche = 0, trovateFamiglie = 0;
		try {
			// Non posso fare dei cicli per scorrere gli arrai in cerca di famiglie e scolaresce, questi
			// devono essere fatti ogni volta che si testa la condizione del while, quindi anche quando il
			// processo si risveglia! Delego alla funzione cercaPrioritari
				
			while ( 
					postiOccupati() == N ||
					(visitatore == ADULTO && (cercaPrioritari(SCOLARESCA) || cercaPrioritari(FAMIGLIA) || (programma != INVALID && programma != EXTREME))) ||
					(visitatore == FAMIGLIA && (cercaPrioritari(SCOLARESCA) || cercaPrioritari(FAMIGLIA, dimensione) || (programma != INVALID && programma != JUNIOR))) ||
					(visitatore == SCOLARESCA && (cercaPrioritari(SCOLARESCA, dimensione) || (programma != INVALID && programma != JUNIOR)))
					)
			{	
				// Mi sospendo sulla coda corretta
				if (visitatore == ADULTO)
				{
					sospesiAdulti++;
					condAdulti.await();
				}
				else
				{
					sospesiGruppi[visitatore][dimensione]++;
					condGruppi[visitatore][dimensione].await();
				}
					
				// Thread riattivato
				Print(tid, "Visitatore " + visitatore + " di dimensione " + dimensione + " risvegliato");
				if (visitatore == ADULTO)
					sospesiAdulti--;
				else
					sospesiGruppi[visitatore][dimensione]--;
			}
			// Visitatore entrato
			postiOccupati[visitatore] += dimensione;
			Print(tid, "Visitatore " + visitatore + " di dimensione " + dimensione + " entrato (posti disponibili: " + new Integer(N-postiOccupati()) +")");
			// Imposto il programma corretto
			if (visitatore == ADULTO)
			{
				System.out.println("Programma corrente: EXTREME");
				programma = EXTREME;
			}
			else
			{
				System.out.println("Programma corrente: JUNIOR");
				programma = JUNIOR;
			}
			// Debug
			System.out.println("---- Visistatori " + SCOLARESCA + " in attesa: " + totaleSospesi(SCOLARESCA));
			System.out.println("---- Visistatori " + FAMIGLIA + " in attesa: " + totaleSospesi(FAMIGLIA));
			System.out.println("---- Visistatori " + ADULTO + " in attesa: " + totaleSospesi(ADULTO));

			// Risveglio la scolaresca (che sono più prioritarie) + numerosa se c'è, altrimenti la famiglia più numerosa
			// se non ci sono ne famiglie ne scolaresche cerco tra gli adulti
			// Risveglio a catena: il primo a entrare chiamerà gli altri
			
			trovateScolaresche = trovateFamiglie = 0;
			// Cerco la scolareca più pioritaria
			for (i = MAX_PERSONE_IN_GRUPPO - 1; i >= 0 && trovateScolaresche == 0; i--)
			{
				if (sospesiGruppi[SCOLARESCA][i] > 0) // Non faccio controlli sui postiOccupati, li fa il thread quando entra
				{
					condGruppi[SCOLARESCA][i].signal();
					trovateScolaresche = 1;
				}
			}
			if (trovateScolaresche == 0)
			{
				for (i = MAX_PERSONE_IN_GRUPPO - 1; i >= 0 && trovateFamiglie == 0; i--)
				{
					if (sospesiGruppi[FAMIGLIA][i] > 0) // Non faccio controlli sui postiOccupati, li fa il thread quando entra
					{
						condGruppi[FAMIGLIA][i].signal();
						trovateFamiglie = 1;
					}
				}
			}
			if (trovateFamiglie == 0 && trovateScolaresche == 0)
				if (sospesiAdulti > 0) // Non faccio controlli sui postiOccupati, li fa il thread quando entra
					condAdulti.signal();
			
			// In realtà il controllo sospesiAdulti, sospesiAdulti e sospesiGruppi è inutile: la signal() se non c'è nessuno in coda non fa nulla
		 
		} catch (Exception e) { e.printStackTrace(); }
		finally {
			lock.unlock();
		}
	}
	
		
	public void Esci(int tid, int visitatore, int dimensione) // visitatore = {ADULTO, FAMIGLIA, SCOLARESCA }
	{
		lock.lock();
		int i, trovateScolaresche, trovateFamiglie;	
		
		try {
			
			// Gruppo uscito
			postiOccupati[visitatore] -= dimensione;

			Print(tid, "Visitatore " + visitatore + " di dimensione " + dimensione + " uscito");
			
			// In uscita controllo prima i le scolaresche (+ numerose), se non ce ne sono allora passo alle famiglie e infine adulti
			// anche qui faccio un controllo preliminare sul fatto che io sia adulto o meno, ma è omissibile
			trovateScolaresche = trovateFamiglie = 0;
			// Cerco la scolareca più pioritaria
			for (i = MAX_PERSONE_IN_GRUPPO - 1; i >= 0 && trovateScolaresche == 0; i--)
			{
				if (sospesiGruppi[SCOLARESCA][i] > 0) // Non faccio controlli sui postiOccupati, li fa il thread quando entra
				{
					condGruppi[SCOLARESCA][i].signal();
					trovateScolaresche = 1;
				}
			}
			if (trovateScolaresche == 0)
			{
				for (i = MAX_PERSONE_IN_GRUPPO - 1; i >= 0 && trovateFamiglie == 0; i--)
				{
					if (sospesiGruppi[FAMIGLIA][i] > 0) // Non faccio controlli sui postiOccupati, li fa il thread quando entra
					{
						condGruppi[FAMIGLIA][i].signal();
						trovateFamiglie = 1;
					}
				}
			}
			if (trovateFamiglie == 0 && trovateScolaresche == 0)
				if (sospesiAdulti > 0) // Non faccio controlli sui postiOccupati, li fa il thread quando entra
					condAdulti.signal();
			
			// Se sono l'ultimo della mia tipologia ad uscire "disabilito" il programma
			if (
				(visitatore == ADULTO && postiOccupati[ADULTO] == 0) ||
				(visitatore != ADULTO && postiOccupati[FAMIGLIA] + postiOccupati[SCOLARESCA] == 0)
				)
			{
				Print(tid, "Ultimo visitatore " + visitatore + " uscito");
				programma = INVALID;
			}
			
		} catch (Exception e) { e.printStackTrace(); }
		finally {
			lock.unlock();
		}
	}
	
	private void Print(int tid, String message)
    {
        String a = "";
        for (int i = 0; i < tid; i++)
            a += " ";
        System.out.println(a + "[" + tid + "] " + message);
    }

}