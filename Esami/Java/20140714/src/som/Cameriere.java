package som;

public class Cameriere extends Thread {
	private final Matrimonio matrimonio;
	private final int numeroInvitati;

	public Cameriere(Matrimonio matrimonio, int numeroInvitati) {
		this.matrimonio = matrimonio;
		this.numeroInvitati = numeroInvitati;
	}

	@Override
	public void run() {

		try {
			for (int i = 0; i < numeroInvitati; i++) {
				System.out.println(String.format("Sono il cameriere e servo il prosecco, i = %d", i));
				matrimonio.serveProsecco("Invitato");
			}
			
			System.out.println("Sono il cameriere e servo il prosecco agli sposi");
			matrimonio.serveProsecco("Sposi");
			
			for (int i = 0; i < numeroInvitati; i++) {
				System.out.println(String.format("Sono il cameriere e servo il pranzo, i = %d", i));
				matrimonio.servePranzo("Invitato");
			}
			matrimonio.servePranzo("Sposi");
			
			for (int i = 0; i < numeroInvitati; i++) {
				System.out.println(String.format("Sono il cameriere e servo la torta, i = %d", i));
				matrimonio.serveTorta();
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
}
