/*  
 * [SIMULAZIONE DEL 18/12/14]
 * 
 * Si consideri un tratto di autostrada, nel quale si stanno svolgendo lavori di manutenzione. 
 * Per questo motivo, il tratto considerato e` soggetto a particolari condizioni di uso.
 * In particolare, sono presenti una corsia Nord e una corsia Sud, ciascuna a capacità
 * limitata MAX (che esprime il massimo numero di veicoli nella corsia). Nel tratto
 * considerato il traffico e` molto piu` intenso nella direzione NS (Nord->Sud) rispetto
 * alla direzione SN (Sud->Nord).
 * Pertanto, allo scopo di ridurre i problemi di congestionamento, la gestione del tratto
 * autostradale avviene secondo la seguente politica:
 * 
 * • la corsia Sud viene percorsa a senso unico nella sola direzione NS;
 * • la corsia Nord viene utilizzata a senso unico alternato (cioè: non può essere contemporaneamente percorsa da veicoli in direzioni opposte) secondo la
 *   seguente politica:
 * 
 *   • Quando la corsia Sud e` piena, i veicoli provenienti da nord vengono deviati nella corsia Nord;
 *   • Nell’accesso alla corsia Nord, si dia la precedenza ai veicoli provenienti da SUD.
 * 
 * Realizzare un’applicazione concorrente in GO oppure in ADA (a scelta), nella quale il
 * tratto autostradale sia gestito da un server, che rispetti tutte le specifiche date ed i
 * veicoli siano rappresentati da clienti distinti.
 */
package main

import(
	"fmt"
	"time"
	"math/rand"
)

//costanti
const SUD_MAX int = 20
const NORD_MAX int = 20
const N_AUTO_NS int = 10
const N_AUTO_SN int = 10
const MAX_BUFF int = 10
const CORSIA_NORD int = 0
const CORSIA_SUD int = 1


//canali
var done = make(chan bool)
var termina = make(chan bool)
var ingresso_NS = make(chan int, MAX_BUFF)
var ingresso_SN = make(chan int, MAX_BUFF)
var uscita_NS = make(chan biglietto, MAX_BUFF)
var uscita_SN = make(chan biglietto, MAX_BUFF)

//ack
var ack_ingresso_NS[N_AUTO_NS] chan int
var ack_ingresso_SN[N_AUTO_SN] chan int
var ack_uscita_NS[N_AUTO_NS] chan bool
var ack_uscita_SN[N_AUTO_SN] chan bool

func when(b bool, c chan int) chan int{
	if(!b){
		return nil
	}
	return c
}

type biglietto struct{
	id int
	corsia int
}

func automobile_SN(id int){
	var corsia int
	var conferma bool
	var ticket biglietto
	
	fmt.Printf("\n[Macchina SN %d] partita da SUD a NORD!\n", id);
	
	fmt.Printf("[Macchina SN %d] richiedo di entrare in autostrada\n", id)
	//chiedo il permesso di inserirmi in corsia 
	ingresso_SN <- id
	
	fmt.Printf("[Macchina SN %d] attendo risposta dal casello\n", id)
	
	//attendo l'ack
	corsia = <- ack_ingresso_SN[id]
	if corsia == -1 {
		done <- true
		return
	}
	fmt.Printf("[Macchina SN %d] percorro l'autostrada in corsia %d\n", id, corsia)
	time.Sleep(time.Second * time.Duration(rand.Intn(10)+1))
	
	ticket = biglietto{id, corsia}
	
	//avviso che sto uscendo
	uscita_SN <- ticket
	
	//attendo ack per uscire
	conferma = <- ack_uscita_SN[id]
	
	if(conferma){
		fmt.Printf("[Macchina SN %d] uscita dall'autostrada\n", id)
	}
	
	done <- true
}

func automobile_NS(id int){
	
	var corsia int
	var conferma bool
	var ticket biglietto
	
	fmt.Printf("\n[Macchina NS %d] partita da NORD a SUD!\n", id);
	 	
	fmt.Printf("[Macchina NS %d] richiedo di entrare in autostrada\n", id)
	
	//chiedo il permesso di inserirmi in corsia 
	ingresso_NS <- id
	
	fmt.Printf("[Macchina NS %d] attendo risposta dal casello\n", id)
	
	//attendo l'ack
	corsia = <- ack_ingresso_NS[id]
	if corsia == -1 {
		done <- true
		return
	}

	fmt.Printf("[Macchina NS %d] percorro l'autostrada in corsia %d\n", id, corsia)
	time.Sleep(time.Second * time.Duration(rand.Intn(10)+1))
	
	ticket = biglietto{id, corsia}
	
	//avviso che sto uscendo
	uscita_NS <- ticket
	
	//attendo ack per uscire
	conferma = <- ack_uscita_NS[id]
	
	if(conferma){
		fmt.Printf("[Macchina NS %d] uscita dall'autostrada\n", id)
	}
	
	done <- true
}

func autostrada(){
	
	var in_corsia_nord[NORD_MAX] bool
	var in_corsia_sud[SUD_MAX] bool
	
	var in_corsia_nord_n int = 0
	var in_corsia_sud_n int  = 0
	var fine bool = false
	
	//ciclo autostrada
	for{
		select{
			
			//se arriva un auto da sud e c'è spazio in corsia sud -> vai sulla corsia sud
			case x:= <- when((!fine && (in_corsia_sud_n < SUD_MAX)), ingresso_SN):
			
				fmt.Printf("[Autostrada] ingresso macchina %d in corsia SUD\n", x)
				in_corsia_sud[x] = true
				in_corsia_sud_n++
				ack_ingresso_SN[x] <- CORSIA_SUD
				fmt.Printf("[Autostrada] %d auto in corsia NORD e %d auto in corsia SUD\n", in_corsia_nord_n, in_corsia_sud_n)
			
			//se arriva un auto da sud e non c'è spazio in corsia SUD -> errore
			case x:= <- when((!fine && (in_corsia_sud_n == SUD_MAX)), ingresso_SN):
				//errore! troppa gente
				fmt.Printf("[Autostrada] corsia SUD congestionata\n");
				ack_ingresso_SN[x] <- -1
				fmt.Printf("[Autostrada] %d auto in corsia NORD e %d auto in corsia SUD\n", in_corsia_nord_n, in_corsia_sud_n)
			
			//se arriva un auto da nord e c'è spazio in corsia nord -> vai sulla corsia nord
			case x:= <- when((!fine && (in_corsia_nord_n < NORD_MAX)), ingresso_NS):
			
				fmt.Printf("[Autostrada] ingresso macchina %d in corsia NORD\n", x)
				in_corsia_nord[x] = true
				in_corsia_nord_n++
				ack_ingresso_NS[x] <- CORSIA_NORD
				fmt.Printf("[Autostrada] %d auto in corsia NORD e %d auto in corsia SUD\n", in_corsia_nord_n, in_corsia_sud_n)
				
			//se arriva un auto da nord e non c'è spazio in corsia NORD, se c'è spazio in corsia sud vai in corsia sud altrimenti errore
			case x:= <- when((!fine && (in_corsia_nord_n == NORD_MAX)), ingresso_NS):
				fmt.Printf("[Autostrada] corsia NORD piena... proviamo la corsia SUD\n")
				if(in_corsia_sud_n < SUD_MAX){
					fmt.Printf("[Autostrada] ingresso macchina %d in corsia SUD per strada congestionata\n", x)
					in_corsia_sud_n++
					in_corsia_sud[x] = true
					ack_ingresso_NS[x] <- CORSIA_SUD
					fmt.Printf("[Autostrada] %d auto in corsia NORD e %d auto in corsia SUD\n", in_corsia_nord_n, in_corsia_sud_n)
				}
			
			//auto sta uscenda da SUD
			case x:= <- uscita_NS:
			
				fmt.Printf("[Autostrada] rilevata auto %d uscire da NORD dalla corsia %d\n", x.id, x.corsia)
				if(x.corsia == 0){
					in_corsia_nord_n--
					in_corsia_nord[x.id] = false
					
				}else{
					in_corsia_sud_n--
					in_corsia_sud[x.id] = false
				}
				
				ack_uscita_NS[x.id] <- true
				fmt.Printf("[Autostrada] %d auto in corsia NORD e %d auto in corsia SUD\n", in_corsia_nord_n, in_corsia_sud_n)
				
			//auto sta uscendo da NORD
			case x:= <- uscita_SN:
			
				fmt.Printf("[Autostrada] rilevata auto %d uscire da NORD dalla corsia %d\n", x.id, x.corsia)
				in_corsia_sud_n--
				in_corsia_sud[x.id] = false
				
				ack_uscita_SN[x.id] <- true
				fmt.Printf("[Autostrada] %d auto in corsia NORD e %d auto in corsia SUD\n", in_corsia_nord_n, in_corsia_sud_n)
				
			case <- termina:
			
				fine = true
				fmt.Printf("[Autostrada] ricevuto ordine di chiusura...\n")
				done <- true
				return
		}
	}
}

func main(){

	rand.Seed(time.Now().Unix())
	
	//inizializzo canali
	for i:=0; i<N_AUTO_NS;i++{
		ack_ingresso_NS[i] = make(chan int, MAX_BUFF)
		ack_uscita_NS[i] = make(chan bool, MAX_BUFF)
	}
	for i:=0; i<N_AUTO_SN;i++{
		ack_ingresso_SN[i] = make(chan int, MAX_BUFF)
		ack_uscita_SN[i] = make(chan bool, MAX_BUFF)
	}
	
	for i:=0; i<N_AUTO_NS;i++{
		go automobile_NS(i)
	}
	
	for i:=0; i<N_AUTO_SN;i++{
		go automobile_SN(i)
	}
	
	go autostrada()
	
	for i:=0; i<N_AUTO_NS+N_AUTO_SN; i++{
		<-done
	}
	
	//avviso l'autostrada di chiudere
	termina <- true
	
	<-done
	fmt.Printf("Autostrada chiusa.\n")
}
