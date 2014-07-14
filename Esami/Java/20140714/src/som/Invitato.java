package som;

import java.util.Random;

public class Invitato extends Thread {
	private final Matrimonio matrimonio;
	private final int id;

	public Invitato(Matrimonio matrimonio, int id) {
		super("Invitato");

		this.matrimonio = matrimonio;
		this.id = id;
	}

	@Override
	public void run() {
		try {
			System.out.println(String.format("Sono l'invitato %d, voglio bere il prosecco", id));
			matrimonio.iniziaProsecco("Invitato");
			System.out.println(String.format("Sono l'invitato %d, inizio a bere il prosecco", id));
			Thread.sleep(2000);
			matrimonio.finisceProsecco("Invitato");
			System.out.println(String.format("Sono l'invitato %d, ho finito il prosecco", id));

			Thread.sleep(2000);
			System.out.println(String.format("Sono l'invitato %d, voglio pranzare", id));
			matrimonio.iniziaPranzo("Invitato");
			System.out.println(String.format("Sono l'invitato %d, inizio il pranzo", id));
			Thread.sleep(2000);
			matrimonio.finiscePranzo("Invitato");
			System.out.println(String.format("Sono l'invitato %d, ho finito il pranzo", id));
			
			Thread.sleep(2000);
			System.out.println(String.format("Sono l'invitato %d, voglio mangiare la torta", id));
			matrimonio.iniziaTorta();
			System.out.println(String.format("Sono l'invitato %d, inizio a mangiare la torta", id));
			Thread.sleep(2000);
			matrimonio.finisceTorta();
			System.out.println(String.format("Sono l'invitato %d, ho finito la torta", id));

		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
}
