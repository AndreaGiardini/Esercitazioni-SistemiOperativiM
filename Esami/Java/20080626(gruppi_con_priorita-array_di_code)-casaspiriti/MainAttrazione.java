import java.util.Random;

class MainAttrazione
{
	private static final int MAX_SCOLARESCHE = 3;
	private static final int MAX_FAMIGLIE = 2;
	private static final int MAX_ADULTI = 5;		

	public static void main(String[] args) throws Exception
	{
		Attrazione A = new Attrazione();
		Visitatore[] scolaresca = new Visitatore[MAX_SCOLARESCHE];
		Visitatore[] famiglie = new Visitatore[MAX_FAMIGLIE];
		Visitatore[] adulti = new Visitatore[MAX_ADULTI];
		
		Random rand = new Random();
		
		for (int i = 0; i < MAX_ADULTI; i++)
		{
			adulti[i] = new Visitatore(A, A.ADULTO, 1);
			//Thread.sleep(100);
			//System.out.println("Thread creato");
			adulti[i].start();		
		}
		
		for (int i = 0; i < MAX_SCOLARESCHE; i++)
		{
			int r = rand.nextInt(A.MAX_PERSONE_IN_GRUPPO - 1) + 1; // Da 1 a MAX_PERSONE_IN_GRUPPO-1
			scolaresca[i] = new Visitatore(A, A.SCOLARESCA, r);
			//Thread.sleep(100);
			//System.out.println("Thread creato (" + r + ")");
			scolaresca[i].start();		
		}
		for (int i = 0; i < MAX_FAMIGLIE; i++)
		{
			int r = rand.nextInt(A.MAX_PERSONE_IN_GRUPPO - 1) + 1;// Da 1 a MAX_PERSONE_IN_GRUPPO-1
			famiglie[i] = new Visitatore(A, A.FAMIGLIA, r);
			//Thread.sleep(100);
			//System.out.println("Thread creato (" + r + ")");
			famiglie[i].start();		
		}

		
		for(int i = 0; i < MAX_SCOLARESCHE; i++)
			scolaresca[i].join();	
			
		for(int i = 0; i < MAX_FAMIGLIE; i++)
			famiglie[i].join();	
		
		for(int i = 0; i < MAX_ADULTI; i++)
			adulti[i].join();
			
		System.out.println("Fine");
	}
}
