/*
 * [COMPITO DEL 22/12/14]
 * 
 * Si consideri un laboratorio artigianale di pasta fresca presso il quale svolgono attività alcune dipendenti (sfogline) dedicate alla produzione di pasta.
 * Nel periodo natalizio il laboratorio ha attivato un servizio di prenotazione di tortellini, realizzato da un apposito sistema di gestione.
 * A questo scopo, si assuma che i tortellini siano venduti ai clienti in confezioni di un unico formato di peso standard (ad esempio 1 kg).
 * Il servizio di prenotazione prevede un protocollo secondo il quale ogni cliente:
 * 
 * 1. Prenota la sua confezione e ottiene un ticket con un identificatore unico della prenotazione. A questo scopo si assuma che il laboratorio non accetti 
 * 	prenotazioni oltre un quantitativo totale di tortellini pari a MAX (cioè, MAX è il massimo numero di confezioni complessivamente prenotabili). 
 * 	Pertanto, ogni cliente che richiede di prenotare oltre il limite MAX, ottiene una risposta negativa da parte del gestore.
 * 2. Ritira la sua confezione, fornendo il numero di ticket ottenuto in fase di prenotazione. Nel caso in cui il ticket non sia valido 
 * 	(ad esempio perché già utilizzato) il cliente ottiene una risposta negativa, altrimenti ottiene i tortellini.
 * 
 * Il sistema di gestione del laboratorio, quindi, raccoglie le prenotazioni dei clienti e gestisce i tortellini man mano prodotti dalle sfogline.
 * In particolare, si assuma che:
 * 
 * - Ogni sfoglina lavori autonomamente e in parallelo con le altre producendo una confezione alla volta.
 * - Ogni sfoglina, per ogni nuova confezione prodotta, depositi la propria confezione in un frigorifero contenente i tortellini disponibili per il ritiro.
 * - Il frigorifero per i tortellini disponibili alla vendita (pronti, cioè, per il ritiro) ha una capacità massima pari a N (N<MAX).
 * 
 * Realizzare un’applicazione a scelta nel linguaggio go oppure nel linguaggio Ada, nella quale clienti, sfogline e gestore del laboratorio siano rappresentati da processi (goroutine o task) concorrenti.
 * La sincronizzazione tra i processi dovrà tenere conto dei vincoli dati.
 * Inoltre, il gestore servirà le richieste applicando il seguente criterio di priorità: 
 * 
 * 1. Richieste di prenotazione da parte di clienti;
 * 2. Richieste di ritiro da parte dei clienti prenotati;
 * 3. Richieste di deposito da parte delle sfogline.
 */ 

package main

import (
	"fmt"
	"math/rand"
	"time"
)

//definizione costanti
const MAX_CLIENTI = 100
const MAX_SFOGLINE = 50
const MAX_PRENOTAZIONI = 60
const MAX_FRIGO = 20
const MAX_BUFF = 100 //per send asincrona
const MAX_ATTESA = 10 //massimo 10 secondi di coda

//struttura messaggio per il ritiro
type biglietto struct { 
	id_cliente int
	numero_prenotazione int
}

//canali
var done = make(chan bool) //canale di sincronizzazione
var termina = make(chan bool)
var prenotazioni = make(chan int, MAX_BUFF) //necessità di accodamento per priorità
var ritiri = make(chan biglietto, MAX_BUFF) //canale per il ritiro del prodotto
var deposita = make(chan int, MAX_BUFF) //canale per il deposito del prodotto

var ACK_PRENOTAZIONI [MAX_CLIENTI]chan int //risposta prenotazioni
var ACK_RITIRI [MAX_CLIENTI] chan bool //risposta ritiri
var ACK_DEPOSITI [MAX_SFOGLINE] chan bool

func when(b bool, c chan int) chan int {
	if !b {
		return nil
	}
	return c
}

func whenBiglietto(b bool, r chan biglietto) chan biglietto {
	if !b {
		return nil
	}
	return r
}

func cliente(id int){
	
	var tempo_di_attesa int
	var ticket int
	var biglietto_ritiro biglietto
	var risposta bool
	
	fmt.Printf("Cliente [%d] entrato nel negozio...\n", id)
	
	//attesa per prenotazione
	tempo_di_attesa = rand.Intn(MAX_ATTESA)+1 //+1 perchè randomizza da 0 a MAX_ATTESA
	time.Sleep(time.Duration(tempo_di_attesa) * time.Second)
	
	//mando prenotazione (asincrona)
	prenotazioni <- id 
	fmt.Printf("[cliente %d] richiesta prenotazione inviata\n", id)
	
	//attendo ticket o errore
	ticket = <- ACK_PRENOTAZIONI[id]
	fmt.Printf("[cliente %d] ottenuto ticket %d\n", id, ticket)
	
	if ticket == -1 {
		//troppe prenotazioni... esco
		done <- true
		return
	}
	
	//attesa per ritiro
	tempo_di_attesa = rand.Intn(MAX_ATTESA)+1 //+1 perchè randomizza da 0 a MAX_ATTESA
	time.Sleep(time.Duration(tempo_di_attesa) * time.Second)
	
	//creo il biglietto
	biglietto_ritiro = biglietto{id, ticket}
	
	//faccio richiesta per il ritiro e ottengo risposta
	ritiri <- biglietto_ritiro
	risposta = <- ACK_RITIRI[id]
	
	if risposta {
		fmt.Printf("[Cliente %d] Ritiro effettuato!\n", id)
	}else{
		fmt.Printf("[Cliente %d] Ritiro negato!\n", id)
	}
	
	//fine cliente
	done <- true
}

func sfoglina(id int){
	
	var tempo_di_creazione int
	var risposta bool
	
	//ciclo di produzione infinito
	for{
		//faccio i tortellini
		tempo_di_creazione = rand.Intn(MAX_ATTESA)+1
		time.Sleep(time.Second * time.Duration(tempo_di_creazione))
		
		//richiedo il deposito e ottengo risposta
		deposita <- id
		risposta = <- ACK_DEPOSITI[id]
		 
		if risposta {
			 fmt.Printf("[sfoglina %d] ho depositato i tortellini in frigo.\n", id)
		} else {
			fmt.Printf("[sfoglina %d]  termino\n", id)
			done <- true
			return
		}
	}
}

func laboratorio(clienti int, sfogline int){
	
	var in_frigo int = 0
	var ticket [MAX_CLIENTI] int //ticket assegnati
	var ret_value int
	var da_ritirare int
	var prenotazione int = -1
	var fine bool = false
	var contatore int = 0
	var i int
	
	//ciclo di riproduzione infinito
	for{
		//logica del laboratorio
		select
		{
			// se c'è spazio in frigo x assumerà il valore dell'id della sfoglina che vuole depositare i tortellini
			case x:= <-when(((in_frigo < MAX_FRIGO) && (fine == false)), deposita):
				in_frigo++
				fmt.Printf("[laboratorio] Sfoglina %d depositi pure i tortellini in frigo\n", x)
				ACK_DEPOSITI[x] <- true
				
			//se non 
			case x:= <-when((fine == true), deposita):
				fmt.Printf("Spazio terminato... la sfoglina puoò andare a casa\n")
				ACK_DEPOSITI[x] <- false
				
			//se ho una prenotazione
			case x:= <- prenotazioni:
				if contatore < MAX_PRENOTAZIONI {
					 //accetta la prenotazione
					 ticket[x] = contatore 
					 ret_value = contatore
					 contatore++
					 da_ritirare++
					 fmt.Printf("[laboratorio] Prenotata confezione per cliente %d: assegnato ticket %d\n", x, ret_value)
				} else {
					//Troppe prenotazioni
					ret_value = -1
					fmt.Printf("[laboratorio] Troppe prenotazioni: rifiutata prenotazione al cliente %d\n", x)
				}
				
				//mando la risposta
				ACK_PRENOTAZIONI[x] <- ret_value
			
			//se è richiesto un ritiro
			case x:= <- whenBiglietto(((in_frigo > 0) && (len(prenotazioni) == 0)), ritiri):
				for i=0; i<clienti; i++{
					if x.numero_prenotazione == ticket[i] { //il ticket esiste ed è valido!
						prenotazione = i
					}
				}
				
				if prenotazione >=0 {
					//ritiro valido
					in_frigo--
					da_ritirare--
					ticket[prenotazione] = 0 //biglietto usato
					fmt.Printf("[laboratorio]  cliente %d ha ritirato!\n", i)
					if da_ritirare == 0 {
						fine = true
						fmt.Printf("[laboratorio] completati ritiri!\n")
					}
					
					ACK_RITIRI[x.id_cliente] <- true
					
				} else {
					//ritiro non valido
					fmt.Printf("[laboratorio] cliente %d - RITIRO NEGATO!\n", i)
					ACK_RITIRI[x.id_cliente] <- false
				}
			case <- termina:
				//è ora di chiudere il laboratorio
				fmt.Printf("[laboratorio] FINE (sono rimaste %d confezioni in frigo..)!\n", in_frigo)
				done <- true
				return
		}
	}
}

func main(){
	
	var clienti int
	var sfogline int

	//ottengo informazioni
	fmt.Printf("Quanti clienti (max %d)? ", MAX_CLIENTI)
	fmt.Scanf("%d", &clienti)
	fmt.Printf("Quante sfogline (max %d)? ", MAX_SFOGLINE)
	fmt.Scanf("%d", &sfogline)
	
	//inizializzo canali
	for i:=0; i<MAX_CLIENTI; i++ {
		ACK_PRENOTAZIONI[i] = make(chan int, MAX_BUFF) //asincrono
		ACK_RITIRI[i] = make(chan bool, MAX_BUFF) //asincrono
	}
	for i:=0; i<MAX_SFOGLINE; i++{
		ACK_DEPOSITI[i] = make(chan bool, MAX_BUFF)
	}
	
	//inizializzo il seed per il random
	rand.Seed(time.Now().Unix())
	
	//faccio partire il laboratorio
	go laboratorio(clienti, sfogline)
	
	//faccio partire i clienti
	for i:=0; i<clienti;i++{
		go cliente(i)
	}
	
	for i:=0; i<sfogline; i++{
		go sfoglina(i)
	}
	
	//attendo la terminazione di tutti i clienti e di tutte le sfogline
	for i:=0; i< sfogline+clienti; i++{
		<-done
	}
	
	//avviso il laboratorio di chiudere
	termina <- true
	
	//aspetto che il laboratorio chiuda
	<-done
	
	fmt.Printf("Giorno lavoro completato!\n")
}
