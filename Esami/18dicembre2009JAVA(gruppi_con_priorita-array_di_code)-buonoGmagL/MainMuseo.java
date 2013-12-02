import java.util.Random;

class MainMuseo
{
	private static final int MAX_LIBERI = 3;
	private static final int MAX_GUIDATI = 2;
	

	public static void main(String[] args) throws Exception
	{
		Museo M = new Museo();
		Visitatore[] liberi = new Visitatore[MAX_LIBERI];
		Visitatore[] guidati = new Visitatore[MAX_GUIDATI];
		
		Random rand = new Random();
				
		for (int i = 0; i < MAX_LIBERI; i++)
		{
			int r = rand.nextInt(M.MAX_PERSONE_IN_GRUPPO) + 1; // Da 1 a MAX_PERSONE_IN_GRUPPO-1
			liberi[i] = new Visitatore(M, M.LIBERI, r);
			//Thread.sleep(100);
			//System.out.println("Thread creato (" + r + ")");
			liberi[i].start();		
		}
		for (int i = 0; i < MAX_GUIDATI; i++)
		{
			int r = rand.nextInt(M.MAX_PERSONE_IN_GRUPPO) + 1;// Da 1 a MAX_PERSONE_IN_GRUPPO-1
			guidati[i] = new Visitatore(M, M.GUIDATI, r);
			//Thread.sleep(100);
			//System.out.println("Thread creato (" + r + ")");
			guidati[i].start();		
		}

		
		for(int i = 0; i < MAX_LIBERI; i++)
			liberi[i].join();	
			
		for(int i = 0; i < MAX_GUIDATI; i++)
			guidati[i].join();	
		
			
		System.out.println("Fine");
	}
}
