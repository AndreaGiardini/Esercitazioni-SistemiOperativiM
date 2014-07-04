import java.util.concurrent.locks.*;
import java.util.Random;

public class Banca {
    private final int N = 3; // capacità banca
    public final int SENIOR = 0;
    public final int STANDARD = 1;
	private final int IN = 0;
	private final int OUT = 1;
    
    private Lock lock = new ReentrantLock();
    private Condition[][] coda = new Condition[2][2]; // Per SENIOR,STANDARD, IN,OUT
	
    private int postiOccupati;
	
    private int sospesi[][] = new int[2][2];
	
    private boolean mDetectorOccupato;
	private Random tempo = new Random();
    
    public Banca()
    {
    	coda[IN] [SENIOR]   = lock.newCondition();
    	coda[OUT][SENIOR]   = lock.newCondition();
    	coda[IN] [STANDARD] = lock.newCondition();
    	coda[OUT][STANDARD] = lock.newCondition();
    	sospesi[IN] [SENIOR]   = 0;
    	sospesi[OUT][SENIOR]   = 0;
    	sospesi[IN] [STANDARD] = 0;
    	sospesi[OUT][STANDARD] = 0;
        postiOccupati = 0;
        mDetectorOccupato = false;
    }
	
	public void Entra(int tid, int tipologia) // implica entrare prima nell'anticamera
	{
		boolean entrato;
		do
		{
			entrato = EntraMetalDetector(tid,tipologia);
			if (!entrato)
			{
				try {
					Thread.sleep(tempo.nextInt(2)*1000); // Attesa che il cliente si liberi del metallo
				} catch (Exception e) { e.printStackTrace(); }
			}	
		} while (entrato == false);
		
		try {
			Thread.sleep(tempo.nextInt(2)*1000); // Attesa nel metaldetector per entrare in banca
		} catch (Exception e) {}
		EntraBanca(tid,tipologia);
	}
	public void Esci (int tid, int tipologia) // implica entrare prima nell'anticamera
    {
		EsciBanca(tid,tipologia); // Entro nel metalDetector
		try {
			Thread.sleep(tempo.nextInt(2)*1000); // Attesa nel metaldetector per uscire dalla banca
		} catch (Exception e) {}
		EsciMetalDetector();
    }
    private boolean EntraMetalDetector (int tid, int tipologia)
	// tipologia = {STANDARD, SENIOR) 
    {
		// Si entra nel metalDetector per entrare nella banca
		
		/* All'ingresso del metal detector ci si ferma se:
			- Il metal detector è occupato
			- Ci sono clienti in attesa di uscire
			- Non c'è spazio nella banca
			- Se non sono senior e ci sono senior che vogliono entrare
		*/
		lock.lock();
		boolean metalli = false;
		try {
			while (
					mDetectorOccupato ||
					sospesi[OUT][SENIOR] + sospesi[OUT][STANDARD] != 0 ||
					postiOccupati == N ||
					(tipologia != SENIOR && sospesi[IN][SENIOR] != 0)
				   )
			{			
				Print(tid,"Cliente tipo " + tipologia + " in attesa per accedere al metaldetector -> verso banca");
				sospesi[IN][tipologia]++; 
				coda[IN][tipologia].await();
				// Risveglio
				sospesi[IN][tipologia]--;	
				Print(tid,"Cliente tipo " + tipologia + " risvegliato per accedere al metaldetector -> verso banca");
			}    	
			
			// Controllo se l'utente possedeva oggetti mettalici, in questo caso deve tornare fuori
			// (e non ha senso metterlo in coda, chi li risveglierebbe se è l'unico thread? - deve proprio richiedere nuovamente l'accesso)
			if (rilevatiMetalli())
			{
				metalli = true;
				Print(tid,"Cliente tipo " + tipologia + " ha dei metallli, richiedere nuovamente l'accesso!");
				// Se non sono entrato perchè avevo oggetti metallici il mdetector è libero, devo provre a svegliare qualcun altro
				Risveglia();
			}
			else
			{
				mDetectorOccupato = true;
				Print(tid,"Cliente tipo " + tipologia + " entrato nel metalDetector -> verso banca");
			}
			
			// Non risveglio nessuno: col metaldetector occupato sono tutti fermi
		} catch (Exception e) { e.printStackTrace(); }
		finally {
			lock.unlock();
		}
		return !metalli;
    }
    private void EntraBanca (int tid,int tipologia)
    {
    	// Si esce dal metaldetector per entrare in banca
		lock.lock();
		try {
			EsciMetalDetector();
			// Se sono entrato nel metaldetector c'era posto nella banca, posso entrare senza controlli
			postiOccupati++;

		} catch (Exception e) { e.printStackTrace(); }
		finally {
			lock.unlock();
		}
    }
    private void EsciBanca (int tid, int tipologia)
    {
		// Esce dalla banca per entrare nell'anticamera
		/* Non posso uscire dalla banca se:
			- Il metalDetector è occupato
			- Non sono SENIOR e ci sono clienti senior in attesa di uscire
		*/
		lock.lock();
		try {
			while (
					mDetectorOccupato ||
				   (tipologia != SENIOR && sospesi[OUT][SENIOR] > 0)
				   )
			{			
				Print(tid,"Cliente tipo " + tipologia + " in attesa di uscire dalla banca");
				sospesi[OUT][tipologia]++; 
				coda[OUT][tipologia].await();	
				// Risveglio
				sospesi[OUT][tipologia]--;	
				Print(tid,"Cliente tipo " + tipologia + " risvegliato per uscire dalla banca");
			}   
			
			postiOccupati--;
			Print(tid,"Cliente tipo " + tipologia + " entrato nel metalDetector -> verso uscita"); 	
			mDetectorOccupato = true;
		
			// Non risveglio nessuno: col metaldetector occupato sono tutti fermi    	
		} catch (Exception e) { e.printStackTrace(); }
		finally {
			lock.unlock();
		}
    }

    private void EsciMetalDetector()
    {    	
		lock.lock();
		try {
			mDetectorOccupato = false;
			
			Risveglia();
			
		} catch (Exception e) { e.printStackTrace(); }
		finally {
			lock.unlock();
		}
    }
	
	private void Risveglia()
	{
		System.out.println("------- Liberato MetalDetector -----------"); 
		System.out.println("	dentro banca: " + postiOccupati);
		System.out.println("	attesa IN SENIOR: " + sospesi[IN][SENIOR] + " attesa IN STANDARD: " + sospesi[IN][STANDARD]);
		System.out.println("	attesa OUT SENIOR: " + sospesi[OUT][SENIOR] + " attesa OUT STANDARD: " + sospesi[OUT][STANDARD]);
		
		// Ho liberato il metaldetector, posso risvegliare:
		// - Clienti in attesa di uscire (senior e se non ce ne sono standard), altrimenti
		// - Clienti in attesa di entrare (senior e se non ce ne sono standard)
		if	(sospesi[OUT][SENIOR] > 0)
		{
			System.out.println(" coda[OUT][SENIOR].signal()"); 
			coda[OUT][SENIOR].signal();	
		}
		else if (sospesi[OUT][STANDARD] > 0)
		{
			System.out.println(" coda[OUT][STANDARD].signal()"); 
			coda[OUT][STANDARD].signal();	
		}
		else if	(sospesi[IN][SENIOR] > 0)
		{
			System.out.println(" coda[IN][SENIOR].signal()"); 
			coda[IN][SENIOR].signal();	
		}
		else if	(sospesi[IN][STANDARD] > 0)
		{
			System.out.println(" coda[IN][STANDARD].signal()"); 
			coda[IN][STANDARD].signal();
		}
	}

	
	private void Print(int tid, String message)
    {
        String a = "";
        for (int i = 0; i < tid; i++)
            a += " ";
        System.out.println(a + "[" + tid + "] " + message);
    }
	private boolean rilevatiMetalli()
	{
		return (new Random().nextInt(2) > 0 ? true : false); 
	}
}

