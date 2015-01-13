/*
 * 
 * CONSEGNA COMPITO 
 * 
 */

package main

import(
	"fmt"
	"time"
	"math/rand"
)

//definizione tipo
type nuovo_tipo struct{
	id int
	dato int
}

//costanti
const COSTANTE1 int = 20
const COSTANTE2 int = 20
const MAX_ATTESA int = 10
const MAX_BUFF int = 20

//canali
var done = make(chan bool)
var termina = make(chan bool)
var canale1 = make(chan int, MAX_BUFF)
var canale2 = make(chan nuovo_tipo, MAX_BUFF)

//ack
var ack_canale1[COSTANTE1] chan int
var ack_canale2[COSTANTE1] chan int

//funzioni utility
func when(b bool, c chan int) chan int{
	if(!b){
		return nil
	}
	return c
}

//ridefinire una when per ogni nuovo tipo definito (se occorre)

func thread1(id int){
	
	int risposta
	var tempo_di_attesa int
	
	//faccio una richiesta
	canale1 <- id
	
	//attendo la risposta
	risposta = ack_canale1[id]
	
	//faccio cose
	tempo_di_attesa = rand.Intn(MAX_ATTESA)+1 //+1 perchè randomizza da 0 a MAX_ATTESA
	time.Sleep(time.Duration(tempo_di_attesa) * time.Second)
	
	//faccio una richiesta
	canale2 <- id
	
	//attendo la risposta
	risposta = ack_canale2[id]
	
	//ho finito il mio lavoro
	done <- true
}

func server(){
	
	//ciclo di vita del server
	for{
		select{
			case x:=<-when((condizione), canale1):
				ack_canale1[x]
			case x:=<-when((condizione), canale2):
				ack_canale2[x]
			case x:=<-termina:
				<-done
				return
		}
	}
}

func main(){
	
	fmt.Printf("Programma avviato")
	rand.Seed(time.Now().Unix())
	
	//inizializzo canali ack
	for i:=0; i<COSTANTE1;i++{
		ack_canale1[i] = make(chan int, MAX_BUFF)
		ack_canale2[i] = make(chan nuovo_tipo, MAX_BUFF)
	}
	
	//lancio threads
	for i:=0; i<COSTANTE1;i++{
		go thread1(i)
	}
	
	//lancio il server
	go server()
	
	//attendo la terminazione dei clients
	for i:=0; i<COSTANTE1; i++{
		<-done
	}
	
	//avviso il server di terminare
	termina <- true
	
	//attendo la terminazione del server
	<-done
	
	fmt.Printf("Programma terminato\n")
}
