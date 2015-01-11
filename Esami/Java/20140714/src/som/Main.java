package som;

public class Main {
	private final static int N = 20;

	/**
	 * @param args
	 * @throws InterruptedException 
	 */
	public static void main(String[] args) throws InterruptedException {
		Thread threads[] = new Thread[N + 2];

		Matrimonio matrimonio = new Matrimonio();

		int i;
		for (i = 0; i < threads.length - 2; i++) {
			threads[i] = new Invitato(matrimonio, i);
		}

		threads[i++] = new Sposi(matrimonio);
		threads[i++] = new Cameriere(matrimonio, N);

		for (Thread t : threads) {
			t.start();
		}
		
		for (Thread t : threads) {
			t.join();
		}
	}
}
