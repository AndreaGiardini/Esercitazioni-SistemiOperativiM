/*

 * Si consideri un magazzino dedicato al commercio di attrezzi da lavoro.
 * Per semplicità si assuma che il catalogo degli articoli venduti dal magazzino contenga
 * 3 articoli diversi: martello, tenaglia, badile.
 * Per ognuno dei 3 articoli in catalogo, il magazzino è in grado di contenerne al
 * massimo MAX esemplari.
 * I clienti del magazzino possono appartenere a 2 diverse categorie:
 *
 * • Negozianti, che fanno acquisti all’ingrosso; ciò significa che ogni negoziante
 *   acquista almeno un numero K di esemplari dell’articolo richiesto (K è una
 *   costante data).
 * • Privati, che comprano articoli al dettaglio; ciò significa che ogni privato
 *   acquisterà un articolo alla volta.
 * 
 * Si assuma inoltre che ogni cliente (Negoziante o privato) potrà acquistare un solo tipo
 * di articolo alla volta.
 * L’approvigionamento del magazzino avviene tramite dei fornitori: ogni fornitore
 * periodicamente consegna al magazzino un lotto di al più MAX esemplari di un solo articolo.
 * Realizzare un’applicazione ADA nella quale Magazzino, Clienti e Fornitori siano
 * rappresentati da TASK distinti.
 * La politica di gestione del magazzino dovrà privilegiare i clienti negozianti rispetto
 * ai privati.
 * Inoltre, tra i fornitori si adotti il seguente criterio di priorità:
 *
 * 1.Fornitori di martelli
 * 2.Fornitori di tenaglie
 * 3.Fornitori di badili
 * 
 */

package main

import(
	"fmt"
	"time"
	"math/rand"
)


//costanti
const MAX_MARTELLI int = 100
const MAX_TENAGLIE int = 100
const MAX_BADILI int = 100

const INGROSSO_K int = 3

const N_PRIVATI int = 3
const N_NEGOZIANTI int = 2
const N_FORNITORI int = 2

const MARTELLO int = 0
const TENAGLIA int = 1
const BADILE int = 2

const MAX_ATTREZZI_NEGOZIANTE int = 5
const MAX_ATTREZZI_PRIVATO int = 3
const MAX_ATTESA int = 10
const MAX_BUFF int = 20

//canali
var done = make(chan bool)
var termina = make(chan bool)
var richiesta_negoziante_martello = make(chan int, MAX_BUFF)
var richiesta_negoziante_tenaglia = make(chan int, MAX_BUFF)
var richiesta_negoziante_badile = make(chan int, MAX_BUFF)

var richiesta_privato_martello = make(chan int, MAX_BUFF)
var richiesta_privato_tenaglia = make(chan int, MAX_BUFF)
var richiesta_privato_badile = make(chan int, MAX_BUFF)

var deposito_martello = make(chan int, MAX_BUFF)
var deposito_tenaglia = make(chan int, MAX_BUFF)
var deposito_badile = make(chan int, MAX_BUFF)

//ack
var ack_negoziante_martello[N_NEGOZIANTI] chan bool
var ack_negoziante_tenaglia[N_NEGOZIANTI] chan bool
var ack_negoziante_badile[N_NEGOZIANTI] chan bool

var ack_privato_martello[N_PRIVATI] chan bool
var ack_privato_tenaglia[N_PRIVATI] chan bool
var ack_privato_badile[N_PRIVATI] chan bool

var ack_deposito_martello[N_FORNITORI] chan bool
var ack_deposito_tenaglia[N_FORNITORI] chan bool
var ack_deposito_badile[N_FORNITORI] chan bool


//funzioni utility
func when(b bool, c chan int) chan int{
	if(!b){
		return nil
	}
	return c
}

//ridefinire una when per ogni nuovo tipo definito (se occorre)

func negoziante(id int){
	
	var attrezzi_acquistati int = 0
	for{
		
		time.Sleep(time.Duration(rand.Intn(10)) * time.Second)
		scelta := rand.Intn(3)	
		
		//faccio una richiesta
		switch scelta{

			case MARTELLO:
				richiesta_negoziante_martello <- id	
				fmt.Printf("[Negoziante %d] attrezzo MARTELLO richiesto\n", id)		
				//attendo la risposta
				<- ack_negoziante_martello[id]

			case TENAGLIA:
				richiesta_negoziante_tenaglia <- id
				fmt.Printf("[Negoziante %d] attrezzo TENAGLIA richiesto\n", id)		
				//attendo la risposta
				<- ack_negoziante_tenaglia[id]

			case BADILE:
				richiesta_negoziante_badile <- id	
				fmt.Printf("[Negoziante %d] attrezzo BADILE richiesto\n", id)		
				//attendo la risposta
				<- ack_negoziante_badile[id]	
		}

		attrezzi_acquistati++
		fmt.Printf("[Negoziante %d] attrezzo comprato. Ho acquistato %d attrezzi\n", id, attrezzi_acquistati)

		//Se ho comprato tutti quelli che mi servono esco
		if(attrezzi_acquistati == MAX_ATTREZZI_NEGOZIANTE){
			break
		}
	}
	
	fmt.Printf("[Negoziante %d] ho comprato tutti gli attrezzi che mi servivano... buonanotte!\n", id)
	//ho finito il mio lavoro
	done <- true
}

func privato(id int){

	var attrezzi_acquistati int = 0	

	for{

		time.Sleep(time.Duration(rand.Intn(10)) * time.Second)
		scelta := rand.Intn(3)	

		//faccio una richiesta
		switch scelta{

			case MARTELLO:
				richiesta_privato_martello <- id	
				fmt.Printf("[Privato %d] attrezzo MARTELLO richiesto\n", id)		
				//attendo la risposta
				<- ack_privato_martello[id]

			case TENAGLIA:
				richiesta_privato_tenaglia <- id
				fmt.Printf("[Privato %d] attrezzo TENAGLIA richiesto\n", id)		
				//attendo la risposta
				<- ack_privato_tenaglia[id]

			case BADILE:
				richiesta_privato_badile <- id	
				fmt.Printf("[Privato %d] attrezzo BADILE richiesto\n", id)		
				//attendo la risposta
				<- ack_privato_badile[id]	
		}
	
		attrezzi_acquistati++
		fmt.Printf("[Privato %d] attrezzo comprato. Ho acquistato %d attrezzi\n", id, attrezzi_acquistati)

		//Se ho comprato tutti quelli che mi servono esco
		if(attrezzi_acquistati == MAX_ATTREZZI_PRIVATO){
			break
		}
	}	
	fmt.Printf("[Privato %d] ho comprato tutti gli attrezzi che mi servivano... buonanotte!\n", id)
	done <- true
}

func fornitore(id int){

	for{
	
		time.Sleep(time.Duration(rand.Intn(10)) * time.Second)
		scelta := rand.Intn(3)	

		//faccio una richiesta
		switch scelta{

			case MARTELLO:
				deposito_martello <- id	
				fmt.Printf("[Fornitore %d] richiedo il deposito di un MARTELLO\n", id)		
				//attendo la risposta
				<- ack_deposito_martello[id]

			case TENAGLIA:
				deposito_tenaglia <- id
				fmt.Printf("[Fornitore %d] richiedo il deposito di una TENAGLIA\n", id)
		
				//attendo la risposta
				<- ack_deposito_tenaglia[id]

			case BADILE:
				deposito_badile <- id	
				fmt.Printf("[Fornitore %d] richiedo il deposito di un BADILE\n", id)		
				//attendo la risposta
				<- ack_deposito_badile[id]	
		}
	
		fmt.Printf("[Fornitore %d] attrezzo depositato\n", id)
	}
}

func magazzino(){
	
	var martelli_magazzino int = 0
	var badili_magazzino int = 0
	var tenaglie_magazzino int = 0	

	//ciclo di vita del magazzino
	for{
		select{

			//se arriva una richiesta da un negoziante la servo
			case x:=<-when((martelli_magazzino >= INGROSSO_K), richiesta_negoziante_martello):
				martelli_magazzino = martelli_magazzino - INGROSSO_K
				ack_negoziante_martello[x] <- true
				fmt.Printf("[Magazzino] %d martelli, %d tenaglie, %d badili in negozio\n", martelli_magazzino, tenaglie_magazzino, badili_magazzino)

			case x:=<-when((tenaglie_magazzino >= INGROSSO_K), richiesta_negoziante_tenaglia):
				tenaglie_magazzino = tenaglie_magazzino - INGROSSO_K
				ack_negoziante_tenaglia[x] <- true
				fmt.Printf("[Magazzino] %d martelli, %d tenaglie, %d badili in negozio\n", martelli_magazzino, tenaglie_magazzino, badili_magazzino)

			case x:=<-when((badili_magazzino > INGROSSO_K), richiesta_negoziante_badile):
				badili_magazzino = badili_magazzino - INGROSSO_K
				ack_negoziante_badile[x] <- true
				fmt.Printf("[Magazzino] %d martelli, %d tenaglie, %d badili in negozio\n", martelli_magazzino, tenaglie_magazzino, badili_magazzino)

			//se arriva una richiesta da un privato e ci sono negozianti in attesa servo i negozianti
			case x:=<-when(((len(richiesta_negoziante_martello) == 0) && (martelli_magazzino > 0)), richiesta_privato_martello):
				martelli_magazzino--
				ack_privato_martello[x] <- true	
				fmt.Printf("[Magazzino] %d martelli, %d tenaglie, %d badili in negozio\n", martelli_magazzino, tenaglie_magazzino, badili_magazzino)

			case x:=<-when(((len(richiesta_negoziante_tenaglia) == 0) && (tenaglie_magazzino > 0)), richiesta_privato_tenaglia):
				tenaglie_magazzino--
				ack_privato_tenaglia[x] <- true	
				fmt.Printf("[Magazzino] %d martelli, %d tenaglie, %d badili in negozio\n", martelli_magazzino, tenaglie_magazzino, badili_magazzino)			

			case x:=<-when(((len(richiesta_negoziante_badile) == 0) && (badili_magazzino > 0)), richiesta_privato_badile):
				badili_magazzino--
				ack_privato_badile[x] <- true	
				fmt.Printf("[Magazzino] %d martelli, %d tenaglie, %d badili in negozio\n", martelli_magazzino, tenaglie_magazzino, badili_magazzino)			

			//fornitori
			case x:=<-when((martelli_magazzino < MAX_MARTELLI), deposito_martello):
				martelli_magazzino++
				ack_deposito_martello[x] <- true
				fmt.Printf("[Magazzino] %d martelli, %d tenaglie, %d badili in negozio\n", martelli_magazzino, tenaglie_magazzino, badili_magazzino)

			case x:=<-when(((len(deposito_martello) == 0) && (tenaglie_magazzino < MAX_TENAGLIE)), deposito_tenaglia):
				tenaglie_magazzino++
				ack_deposito_tenaglia[x] <- true
				fmt.Printf("[Magazzino] %d martelli, %d tenaglie, %d badili in negozio\n", martelli_magazzino, tenaglie_magazzino, badili_magazzino)

			case x:=<-when(((len(deposito_martello) == 0) && (len(deposito_tenaglia) == 0) && (badili_magazzino < MAX_BADILI)), deposito_badile):
				badili_magazzino++
				ack_deposito_badile[x] <- true
				fmt.Printf("[Magazzino] %d martelli, %d tenaglie, %d badili in negozio\n", martelli_magazzino, tenaglie_magazzino, badili_magazzino)

			case <-termina:
				<-done
				return
		}
	}
}

func main(){
	
	fmt.Printf("Programma avviato\n")
	rand.Seed(time.Now().Unix())
	
	//inizializzo canali ack
	for i:=0; i<N_NEGOZIANTI;i++{
		ack_negoziante_martello[i] = make(chan bool, MAX_BUFF)
		ack_negoziante_tenaglia[i] = make(chan bool, MAX_BUFF)
		ack_negoziante_badile[i] = make(chan bool, MAX_BUFF)
	}

	for i:=0; i<N_PRIVATI;i++{
		ack_privato_martello[i] = make(chan bool, MAX_BUFF)
		ack_privato_tenaglia[i] = make(chan bool, MAX_BUFF)
		ack_privato_badile[i] = make(chan bool, MAX_BUFF)
	}

	for i:=0; i<N_FORNITORI;i++{
		ack_deposito_martello[i] = make(chan bool, MAX_BUFF)
		ack_deposito_tenaglia[i] = make(chan bool, MAX_BUFF)
		ack_deposito_badile[i] = make(chan bool, MAX_BUFF)
	}
	
	//lancio threads
	for i:=0; i<N_NEGOZIANTI;i++{
		go negoziante(i)
	}

	for i:=0; i<N_PRIVATI;i++{
		go privato(i)
	}

	for i:=0; i<N_FORNITORI;i++{
		go fornitore(i)
	}
	
	//lancio il server
	go magazzino()
	
	//attendo la terminazione dei clients
	for i:=0; i<N_NEGOZIANTI+N_PRIVATI+N_FORNITORI; i++{
		<-done
	}
	
	//avviso il magazzino di terminare
	termina <- true
	
	//attendo la terminazione del magazzino
	<-done
	
	fmt.Printf("Programma terminato\n")
}
