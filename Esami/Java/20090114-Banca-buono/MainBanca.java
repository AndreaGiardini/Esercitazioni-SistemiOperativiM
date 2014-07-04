import java.util.Random;

class MainBanca
{
	private static final int MAX_CLIENTI = 5;
	public static void main(String[] args) throws Exception
	{
		Banca B = new Banca();
		Cliente[] cliente = new Cliente[MAX_CLIENTI];
		
		Random rand = new Random();
				
		for (int i = 0; i < MAX_CLIENTI; i++)
		{
			int tipo = rand.nextInt(2); // Random [0,1]
			cliente[i] = new Cliente(B,tipo);
			//Thread.sleep(100);
			//System.out.println("Thread creato (" + r + ")");
			cliente[i].start();		
		}
			
		for(int i = 0; i < MAX_CLIENTI; i++)
			cliente[i].join();	
		
			
		System.out.println("Fine");
	}
}