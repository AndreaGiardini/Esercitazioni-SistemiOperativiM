package som;

public class Sposi extends Thread {
	private final Matrimonio matrimonio;

	public Sposi(Matrimonio matrimonio) {
		this.matrimonio = matrimonio;
	}

	@Override
	public void run() {
		try {
			System.out.println("Siamo gli sposi, vogliamo il prosecco");
			matrimonio.iniziaProsecco("Sposi");
			System.out.println("Siamo gli sposi, abbiamo iniziato il prosecco");
			sleep(2000);
			matrimonio.finisceProsecco("Sposi");
			System.out.println("Siamo gli sposi, abbiamo finito il prosecco");
			sleep(2000);
			System.out.println("Siamo gli sposi, vogliamo pranzare");
			matrimonio.iniziaPranzo("Sposi");
			System.out.println("Siamo gli sposi, abbiamo iniziato a pranzare");
			sleep(2000);
			matrimonio.finiscePranzo("Sposi");
			System.out.println("Siamo gli sposi, abbiamo finito di pranzare");
			sleep(2000);
			System.out.println("Siamo gli sposi, vogliamo tagliare la torta");
			matrimonio.tagliaTorta();
			System.out.println("Siamo gli sposi, abbiamo tagliato la torta");
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
}
