import java.util.Random;

class MainGuida
{
	public static final int N = 6;
	private static final int MAX_TURISTI  = 7;


	public static void main(String[] args) throws Exception
	{
		Guide G = new Guide();
		Turista[] tur = new Turista[MAX_TURISTI];
		
		Random rand = new Random();
		
		for (int i = 0; i < MAX_TURISTI; i++)
		{
			int r = rand.nextInt(2);
			int t = rand.nextInt(2);
			tur[i] = new Turista(G,t,r);
			//Thread.sleep(100);
			//System.out.println("Thread creato (" + r + ")");
			tur[i].start();		
		}

		for(int i = 0; i < MAX_TURISTI; i++)
			tur[i].join();	
		
		System.out.println("Fine");
	}
}
